#include "kernel_editted.h"

__kernel void bruteForceInKernel (__global uint8_t* salt_pkey,
								uint16_t itercount_pkey,
								__global uint8_t *crypto_pkey,
								long crypto_len_pkey,
								__global char* pw_charset_bforce,
								uint32_t pw_min_len_bforce,
								uint32_t pw_max_len_bforce,
								uint64_t pw_cursor_bforce,
								__global uint8_t *plain,
								__global char* password_out,
								__global uint* iter_out,
								uint32_t charset_len,
								__global uint64_t* max_cursor,
								__global uint* end)
{
	uint threadIdx = get_global_id(X) + get_global_size(X) * (get_global_id(Y) + get_global_size(Y) * get_group_id(Z));
	uint iterSize = get_global_size(X) *get_global_size(Y) * get_global_size(Z);
	char password[MAX_PASSWORD];
	NPKIPrivateKey pkey_real;
	NPKIBruteForce bforce_real;
	NPKIPrivateKey* pkey;
	NPKIBruteForce* bforce;
	bforcePkeyInit(   salt_pkey,
					itercount_pkey,
					crypto_pkey,
					crypto_len_pkey,
					plain+crypto_len_pkey*threadIdx,
					&(password[0]),
					pw_charset_bforce,
					pw_min_len_bforce,
					pw_max_len_bforce,
					pw_cursor_bforce,
					pkey,
					bforce);
	NPKIPrivateKey* ikey;



	pkey = &pkey_real;
	bforce = &bforce_real;
	ikey = pkey;

	uint64_t base_cursor = bforce->pw_cursor;
	bforce->pw_cursor+=threadIdx;

	while (bforce->pw_cursor < max_cursor[MAX_PASSWORD])
	{
		PasswordGenerate(bforce, password, max_cursor, charset_len);
		NPKIDecrypt(ikey, password);

	// 제대로 디코딩되었다면
		if(IsPKCS5PaddingOK(&(pkey->plain[0]), pkey->crypto_len))
		{
			{
				memcpy_private_to_global(bforce->password, password_out, MAX_PASSWORD);
				bforce->password[MAX_PASSWORD-1] = '\0';
				bforce->pw_cursor = max_cursor[MAX_PASSWORD]; // Go out of the loop!
				*end = 1;
			}
		}
		bforce->pw_cursor+=iterSize;
		iter_out[threadIdx]+=1;
	}
}