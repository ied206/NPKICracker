#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <omp.h>

#include "Seed.h"
#include "Hash.h"
#include "BasicIO.h"
#include "ErrorHandle.h"
#include "NPKICrack.h"

// 이 static 전역변수들은 한번 값을 쓰면 변경되지 않는다.
static uint8_t charset_dic[0x100] = {0};
static uint32_t charset_len;
static uint64_t max_cursor[MAX_PASSWORD+1] = {0};

// 한 번 호출하면 내부적으로 쓰레드를 만들어 전부 다 돌려본다
void BruteForceIterate (NPKIPrivateKey *pkey, NPKIBruteForce *bforce)
{
	// 시작하는 위치. -i 옵션 때문에 넣음
	uint64_t base_cursor = bforce->pw_cursor;
	// PRINT_INTERVAL 초마다 한번 표시해야 하기 때문에 넣음
	time_t prev_time = time(NULL) - bforce->print_interval;

	char** passwords = NULL;
	NPKIPrivateKey** ikeys = NULL;
	// omp 함수들을 #pragma omp 밖에서 호출할 경우, 컴파일러에 따라 제대로 된 값이 반환되지 않는 경우가 있다.
	#pragma omp parallel
	{
		if (omp_get_thread_num() == 0)
		{
			// NPKIPrivateKey, password는 각 쓰레드마다 별도로 넣어줌.
			passwords = (char**) malloc(omp_get_max_threads() * sizeof(char*));
			ikeys = (NPKIPrivateKey**) malloc(omp_get_max_threads() * sizeof(NPKIPrivateKey*));
			for (uint32_t i = 0; i < omp_get_max_threads(); i++)
			{
				passwords[i] = (char*) malloc(MAX_PASSWORD * sizeof(char));
				ikeys[i] = (NPKIPrivateKey*) malloc(sizeof(NPKIPrivateKey));
				NPK_Duplicate(ikeys[i], pkey);
			}
		}
	}

//	for (uint64_t i = base_cursor; i < max_cursor[MAX_PASSWORD]; i++)
	// OpenMP 쓰레드 생성
	#pragma omp parallel
	{
		while (bforce->pw_cursor < max_cursor[MAX_PASSWORD])
		{
			char *password = passwords[omp_get_thread_num()];
			NPKIPrivateKey *ikey = ikeys[omp_get_thread_num()];
			PasswordGenerate(bforce, password);
			NPKIDecrypt(ikey, password);

			// Thread 0일때, PRINT_INTERVAL초마다 현황 출력
#ifndef _DEBUG
			if (!omp_get_thread_num() && time(NULL) - prev_time >= bforce->print_interval)
#endif
			{
				// PRIu64 를 쓰는 이유 -> uint64_t는 윈도우에서 %I64u, 리눅스에서 %llu라 서로 다름
				uint32_t elasped = time(NULL) - bforce->starttime + 1; // 1 안 붙이면 DIV/0 발생
				uint64_t remains = ((max_cursor[MAX_PASSWORD] - bforce->pw_cursor) / (bforce->pw_cursor - base_cursor + 1) * elasped); // 시간 = 거리 / 속도
				printf(	"Now Calculating Password \'%s\'\n"
						"  Job Done  : %2"PRIu64".%02"PRIu64"%%\n"
						"  Job Speed : %"PRId64" key/sec\n"
						"  Elapsed   : %02ud %02uh %02um %02us\n"
						"  Remain    : %02"PRIu64"d %02"PRIu64"h %02"PRIu64"m %02"PRIu64"s\n",
						password,
						bforce->pw_cursor * 100 / max_cursor[MAX_PASSWORD],
						(bforce->pw_cursor * 10000 / max_cursor[MAX_PASSWORD]) % 100,
						(bforce->pw_cursor - base_cursor) / elasped,
						sec2day(elasped), sec2hour(elasped), sec2min(elasped), sec2sec(elasped),
						sec2day(remains), sec2hour(remains), sec2min(remains), sec2sec(remains));
				prev_time = time(NULL);
#ifdef _DEBUG
				getchar();
#endif
			}

			// 공인인증서 비밀번호를 찾았다면 password를 기록하고 while loop 탈출
			if(IsPKCS5PaddingOK(pkey->plain, pkey->crypto_len))
			{
				// 사실 이 if문은 단 한 스레드만 들어올 수 있기에 critical을 꼭 안 써도 된다.
				#pragma omp critical
				{
					// 찾아낸 비밀번호를 bforce에 복사하고 (main에 전달해야 한다)
					strncpy(bforce->password, password, MAX_PASSWORD);
					bforce->password[MAX_PASSWORD-1] = '\0';
					// main에 찾았다고 알린다
					bforce->decrypt = TRUE;
					// While 반복문 조건을 깨서 루프 탈출
					bforce->pw_cursor = max_cursor[MAX_PASSWORD];
				}
			}

			// atomic이 critical보다 더 빠름
			#pragma omp atomic
			bforce->pw_cursor++;
		}
	}

	// 동적할당된 구조체/배열 해제
	for (uint32_t i = 0; i < omp_get_max_threads(); i++)
	{
		free(passwords[i]);
		free(ikeys[i]);
	}
	free(passwords);
	free(ikeys);
	// Use After Free 방지
	passwords = NULL;
	ikeys = NULL;
}

void NPKIDecrypt (NPKIPrivateKey *pkey, const char* password)
{
// 변수 선언 및 초기화
	uint8_t dkey[20] = {0}, div[20] = {0}, buf[20] = {0}, iv[16] = {0}, seedkey[20] = {0}; // dkey, div, buf is temporary
	uint32_t roundkey[32] = {0};

// Get SEED Key
	// 비밀번호 길이는 최대 64자리까지 제한되어 있다.
	JV_PBKDF1(dkey, (uint8_t*)password, strlen(password), pkey->salt, sizeof(pkey->salt), pkey->itercount);
	memcpy(seedkey, dkey, 16);
// Get SEED IV
	memcpy(buf, dkey+16, 4);
	JV_SHA1(div, buf, 4);
	memcpy(iv, div, 16);

#ifdef _DEBUG
	puts("\n== SEED Key ==");
	DumpBinary(seedkey, 16);
	puts("\n== IV ==");
	DumpBinary(iv, 16);
#endif

	JV_SeedRoundKey(roundkey, seedkey);

	// JV_SEED_CBC128_Decrypt_Serial(pkey->crypto, pkey->plain, pkey->crypto_len, roundkey, iv);

	uint8_t* virt_in = (uint8_t*) malloc(pkey->crypto_len + 16); // len of in + iv
	for (uint32_t x = 0; x < 16; x++)
		virt_in[x] = iv[x];
	for (uint32_t x = 0; x < pkey->crypto_len; x++)
	{
		virt_in[x + 16] = pkey->crypto[x];
	}
	JV_SEED_CBC128_Decrypt_NoBranch(virt_in, pkey->plain, pkey->crypto_len, roundkey);
	free(virt_in);
	/*
		const uint32_t looplen = pkey->crypto_len;
	// 여기에 OpenMP를 쓰면 쓰레드 생성/해제를 너무 자주 해서 오버헤드로 인해 느려짐
	// #pragma omp parallel for
		for (uint32_t i = 0; i < looplen; i += SeedBlockSize)
			JV_SEED_CBC128_Decrypt_OneBlock(pkey->crypto + i, pkey->plain + i, roundkey, i ? pkey->crypto + (i-SeedBlockSize) : iv);
	*/
}

char* PasswordGenerate (NPKIBruteForce *bforce, char password[])
{
	uint32_t i = 0;
	uint64_t len_cursor = bforce->pw_cursor;
	uint32_t pw_now_len = 0;
	for (i = bforce->pw_min_len; i <= bforce->pw_max_len; i++)
	{
		if (max_cursor[i] <= len_cursor)
			len_cursor -= max_cursor[i];
		else
		{
			pw_now_len = i;
			break;
		}

	}

	for (i = 0; i < pw_now_len; i++)
		password[i] = bforce->pw_charset[(len_cursor % ipow(charset_len, i+1)) / ipow(charset_len, i)];
	password[i] = '\0';
	return password;
}



void NPK_Init (NPKIPrivateKey *pkey)
{
	pkey->rawkey = NULL;
	pkey->rawkey_len = 0;
	for (int i = 0; i < 8; i++)
		pkey->salt[i] = 0;
	pkey->itercount = 0;
	pkey->crypto = NULL;
	pkey->crypto_len = 0;
	pkey->plain = NULL;
}

void NPK_Duplicate (NPKIPrivateKey *dest, NPKIPrivateKey *src)
{
	dest->rawkey = src->rawkey;
	dest->rawkey_len = src->rawkey_len;
	for (int i = 0; i < 8; i++)
		dest->salt[i] = src->salt[i];
	dest->itercount = src->itercount;
	dest->crypto = src->crypto;
	dest->crypto_len = src->crypto_len;
	dest->plain = src->plain;
}

int NPK_ReadRaw (NPKIPrivateKey *pkey, const char* PrivateKeyPath)
{
	FILE *fp = fopen(PrivateKeyPath, "rb");
	pkey->rawkey_len = ReadFileSize(PrivateKeyPath);
	pkey->rawkey = (uint8_t *)malloc(pkey->rawkey_len * sizeof(uint8_t));
	if (fread((void*) (pkey->rawkey), 1, pkey->rawkey_len, fp) != pkey->rawkey_len)
		JV_WarnHandle(JVWARN_RESULT_NOT_OK_FREAD);
	fclose(fp);

#ifdef _DEBUG
	puts("== Private Key ==");
	DumpBinary(pkey->rawkey, pkey->rawkey_len);
#endif

	return 0;
}

void NPK_Parse (NPKIPrivateKey *pkey)
{
// salt <- PrivateKeyBuf, rawkey[20]-[27] // 21-28바이트
	memcpy((void*) (pkey->salt), (void*) (pkey->rawkey+20), 8);
// itercount <- rawkey[30]-[31] // 31-32바이트
	pkey->itercount = (pkey->rawkey[30] << 8) + pkey->rawkey[31];

#ifdef _DEBUG
	puts("\n== Salt ==");
	DumpBinary(pkey->salt, 8);
	printf(	"\n== Itercount : %d ==\n", pkey->itercount);
#endif

// crypted <- rawkey[36]-[End] // 37바이트부터
	pkey->crypto_len = pkey->rawkey_len - 36;
	pkey->crypto = (uint8_t *)malloc(pkey->crypto_len * sizeof(uint8_t));
	pkey->plain = (uint8_t *)malloc(pkey->crypto_len * sizeof(uint8_t));
	memcpy((void*) pkey->crypto, (void*) (pkey->rawkey + 36), pkey->crypto_len);

#ifdef _DEBUG
	puts("\n== Encrypted Data ==");
	DumpBinary(pkey->crypto, pkey->crypto_len);
#endif
}

void NPK_Free (NPKIPrivateKey *pkey)
{
	free(pkey->rawkey);
	free(pkey->crypto);
	free(pkey->plain);
}


void NBF_Init (NPKIBruteForce *bforce)
{
	for (uint32_t i = 0; i < MAX_PASSWORD; i++)
		bforce->password[i] = 0;
	for (uint32_t i = 0; i < MAX_PW_CHARSET; i++)
		bforce->pw_charset[i] = 0;
	bforce->pkey_path = NULL;
	bforce->pw_charset_path = NULL;// Password Charset
	bforce->pw_init = NULL;
	bforce->pw_min_len = 0;		// Password Minimum Length
	bforce->pw_max_len = 0;		// Password Maximum Length
	bforce->pw_cursor = 0;		// Now, n'th pw is to iterate?
	bforce->starttime = 0;
	bforce->decrypt = FALSE;
	bforce->use_opencl = FALSE;
	bforce->print_interval = DEFAULT_PRINT_INTERVAL;
}

int NBF_ReadCharset (NPKIBruteForce *bforce)
{
	FILE *fp = NULL;
	long cslen = 0;

// 파일 길이
	cslen = ReadFileSize(bforce->pw_charset_path);
	if (MAX_PW_CHARSET < cslen) // 그대로 두면 Overflow 발생 -> Abort
		JV_ErrorHandle(JVERR_PW_CHARSET_TOO_LONG);

// 내용을 읽는다.
	fp = fopen(bforce->pw_charset_path, "rt");
	for (long i = 0; i < cslen; i++)
		bforce->pw_charset[i] = fgetc(fp);
	fclose(fp);

// 중복되는 문자가 없는지 검사
	for (long i = 0; i < cslen; i++)
		charset_dic[(uint8_t) bforce->pw_charset[i]]++;

	for (long i = 0; i < cslen; i++)
	{
		if (2 <= charset_dic[i])
			return FALSE; // 검증 실패
	}
// 길이 저장
	charset_len = strlen(bforce->pw_charset);

	return TRUE;
}

//
int NBF_ValidateInitPW (NPKIBruteForce *bforce)
{
	if (strlen(bforce->pw_init) < bforce->pw_min_len)
		JV_ErrorHandle(JVERR_PW_INITIAL_TOO_SHORT);
	if (bforce->pw_max_len < strlen(bforce->pw_init))
		JV_ErrorHandle(JVERR_PW_INITIAL_TOO_LONG);
	for (size_t i = 0; i < strlen(bforce->pw_init); i++)
	{
		if (charset_dic[(uint8_t) bforce->pw_init[i]] == 0) // charset에 없는게 pw_init에 있다
			return FALSE;
	}

	return TRUE;
}

uint64_t NBF_SetCursorFromInitPW (NPKIBruteForce *bforce)
{
	uint32_t pw_now_len = strlen(bforce->pw_init);
	bforce->pw_cursor = 0;
	for (uint32_t i = bforce->pw_min_len; i < pw_now_len; i++)
		bforce->pw_cursor += max_cursor[i];

	for (uint32_t i = 0; i < pw_now_len; i++)
		bforce->pw_cursor += (NBF_GetCharsetNumber(bforce, bforce->pw_init[i]) * ipow(charset_len, i));

	strncpy(bforce->password, bforce->pw_init, MAX_PASSWORD);
	bforce->password[MAX_PASSWORD-1] = '\0';

	return bforce->pw_cursor;
}

uint32_t NBF_GetCharsetNumber(NPKIBruteForce *bforce, const char tofind)
{
	char* address = strchr(bforce->pw_charset, tofind);
	return (uint32_t) (address - bforce->pw_charset);
}

void NBF_Ready (NPKIBruteForce *bforce)
{
	bforce->pw_cursor = 0;
}

void GetMaxCursor(NPKIBruteForce *bforce)
{
	for (uint32_t i = bforce->pw_min_len; i <= bforce->pw_max_len; i++)
	{
		// i는 비번길이
		max_cursor[i] = 1;
		for (uint32_t d = 0; d < i; d++) // d는 자릿수
			max_cursor[i] += NBF_GetCharsetNumber(bforce, bforce->pw_charset[charset_len-1]) * ipow(charset_len, d);
		// 여기는 전체의 커서 길이
		max_cursor[MAX_PASSWORD] += max_cursor[i];
#ifdef _DEBUG
		printf("max_cursor for len %2u : %"PRIu64"\n", i, max_cursor[i]);
#endif
	}

	return;
}

void SetStartTime(NPKIBruteForce *bforce)
{
	bforce->starttime = time(NULL);
}

void NBF_PrintEnvInfo(NPKIBruteForce *bforce)
{
	char pw[MAX_PASSWORD];
	puts(	"= BruteForce Environment Info =");
	if (bforce->use_opencl) // Use OpenCL
		puts("Which Device to Use         : GPU (OpenCL)");
	else // Use OpenMP
	{
		#pragma omp barrier
		{
			printf("Which Device to Use         : CPU (OpenMP, %d Threads)\n", omp_get_max_threads());
		}
	}
	printf(	"Password Minimum Length     : %u\n"
			"Password Maximum Length     : %u\n"
			"Status Print Interval       : %u sec\n"
			"Number of keys to calculate : %"PRIu64"\n"
			"Initial Password to try     : \'%s\'\n"
			"Password Charsets to try\n  ",
			bforce->pw_min_len, bforce->pw_max_len,
			bforce->print_interval,
			max_cursor[MAX_PASSWORD],
			PasswordGenerate(bforce, pw));
	for (uint32_t i = 0; i < charset_len; i++)
		putchar(bforce->pw_charset[i]);
	putchar('\n');
}
