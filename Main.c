#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Seed.h"
#include "Hash.h"
#include "BasicIO.h"
#include "ErrorHandle.h"
#include "NPKICrack.h"

void JV_Help ();

int main (int argc, char* argv[])
{
// This file is saved with PKCS#8 file format, and the key is encrypted with PKCS#5's PBKDF1
// 변수 선언 및 초기화
    NPKIPrivateKey pkey; 	// NPKI Private Key Struct
    NPKIBruteForce bforce;	// NPKI Brute Force Struct

	// 찾았다면 TRUE로 SET
	int arg_file	= FALSE;
	int arg_min		= FALSE;
	int arg_max 	= FALSE;
	int arg_charset	= FALSE;
	int arg_initial	= FALSE;

	NPK_Init(&pkey);
    NBF_Init(&bforce);

// Welcome Print
	printf(	"Joveler and joonji's NPKI Craker for Inc0gntio 2015\n"
			"Version %u.%02u (%dbit Build, Compile Date %04d.%02d.%02d)\n\n",
			MAJOR_VER, MINOR_VER, WhatBitOS(), CompileYear(), CompileMonth(), CompileDate());

// ./NPKICracker -f [PrivateKeyFile] -m [MinPWLen] -M [MaxPWLen] -c [PWCharset] -i [StartFrom]
	if (argc == 1)
	{
		JV_WarnHandle(JVWARN_NOT_ENOUGH_ARGV);
		putchar('\n');
		JV_Help();
		exit(JVWARN_NOT_ENOUGH_ARGV);
	}

	// Search for arguments
	for (int i = 1; i < argc; i++)
	{ // 이 루프를 돌며 -f, -m, -M, -c, -i, -p, -cl을 찾는다.
		if (strcmp(argv[i], "-f") == 0)
		{  // -f signPri.key, 파일 존재
			if (i+1 != argc && scanfile(argv[i+1]))
			{
				arg_file = TRUE;
				bforce.pkey_path = argv[i+1];
			}
			else
				JV_ErrorHandle(JVERR_PRIVATE_KEY_NOT_EXIST);
		}
		else if (strcmp(argv[i], "-m") == 0)
		{ // -m 10
			if (i+1 != argc)
			{
				if (atoi(argv[i+1]) < 1)
					JV_ErrorHandle(JVERR_PW_MIN_LENGTH_TOO_SHORT);
				bforce.pw_min_len = atoi(argv[i+1]);
				arg_min = TRUE;
			}
			else
				JV_ErrorHandle(JVERR_PW_MIN_LENGTH_NOT_EXIST);
		}
		else if (strcmp(argv[i], "-M") == 0)
		{ // -m 15
			if (i+1 != argc && 0 < atoi(argv[i+1]))
			{
				if (atoi(argv[i+1]) < 1)
					JV_ErrorHandle(JVERR_PW_MAX_LENGTH_TOO_SHORT);
				if (MAX_PASSWORD <= atoi(argv[i+1]))
					JV_ErrorHandle(JVERR_PW_MAX_LENGTH_TOO_LONG);
				bforce.pw_max_len = atoi(argv[i+1]);
				arg_max = TRUE;
			}
			else
				JV_ErrorHandle(JVERR_PW_MAX_LENGTH_NOT_EXIST);
		}
		else if (strcmp(argv[i], "-c") == 0)
		{ // -c Charset.txt
			if (i+1 != argc && scanfile(argv[i+1]))
			{
				arg_charset = TRUE;
				bforce.pw_charset_path = argv[i+1];
			}
			else
				JV_ErrorHandle(JVERR_PW_CHARSET_NOT_EXIST);
		}
		else if (strcmp(argv[i], "-i") == 0)
		{ // -i pwpigeon
			if (i+1 != argc)
			{
				arg_initial = TRUE;
				bforce.pw_init = argv[i+1];
			}
			else
				JV_ErrorHandle(JVERR_PW_INITIAL_NOT_EXIST);
		}
		else if (strcmp(argv[i], "-p") == 0)
		{ // -i pwpigeon
			if (i+1 != argc)
			{
				int print_interval = atoi(argv[i+1]);
				if (print_interval < 1)
					JV_ErrorHandle(JVERR_PRINT_INTERVAL_TOO_SHORT);
				if (3600 < print_interval) // 최대
					JV_ErrorHandle(JVERR_PRINT_INTERVAL_TOO_LONG);
				bforce.print_interval = print_interval;
			}
			else
				JV_ErrorHandle(JVERR_PRINT_INTERVAL_NOT_EXIST);
		}
		else if (strcmp(argv[i], "-cl") == 0)
		{ // -cl, for OpenCL
			bforce.use_opencl = TRUE;
		}
		else if (!strcmp(argv[i], "--help") && !strcmp(argv[i], "-h") && !strcmp(argv[i], "/?"))
		{ // Help Message
			JV_Help();
			return 0;
		}
	}

	if (!arg_file) // PrivateKeyFile이 없다면
		JV_ErrorHandle(JVERR_PRIVATE_KEY_NOT_EXIST);
	if (!arg_min) // Default
		bforce.pw_min_len = 10;
	else
	{
		if (bforce.pw_max_len < bforce.pw_min_len)
			JV_ErrorHandle(JVERR_PW_MAX_LENGTH_TOO_SHORT);
	}
	if (!arg_max) // Default
		bforce.pw_max_len = 16;
	if (!arg_charset)
		JV_ErrorHandle(JVERR_PW_CHARSET_NOT_EXIST);

// Read and Parse NPKI Private Key Struct
	NPK_ReadRaw(&pkey, bforce.pkey_path);
	NPK_Parse(&pkey);
	puts("= Reading NPKI Private Key File Complete =");

// Read and Parse Password Charset
	if (NBF_ReadCharset(&bforce) == FALSE)
		JV_ErrorHandle(JVERR_PW_CHARSET_DUPLICATE);
	puts("= Reading Password Charset File Complete =");


// Validate Initial Password
	if (arg_initial)
	{
		if (NBF_ValidateInitPW(&bforce) == FALSE)
			JV_ErrorHandle(JVERR_PW_INITIAL_NOT_VALID);
		NBF_SetCursorFromInitPW(&bforce);
		puts("= Reading Initial Password Complete =");
	}
	else
		NBF_Ready(&bforce);

// Calculate MAX Cursor -> Used for calculating percent realtime
	GetMaxCursor(&bforce);
	SetStartTime(&bforce);

// Print Session Info
    NBF_PrintEnvInfo(&bforce);
    putchar('\n');
	putchar('\n');

// Start BruteForce!
	printf("= Press Enter to start BruteForce... =\n");
	getchar();
	putchar('\n');

	if (bforce.use_opencl)  // Use OpenCL
	{
//		BruteForceIterateOpenCL(&pkey, &bforce, kernel, context);
	}
	else // Use OpenMP
	{
		BruteForceIterate(&pkey, &bforce);
	}
	if (bforce.decrypt) // Found it
	{
		printf(	"\n= Decrypt Success =\n"
				"Password is \'%s\'\n\n", bforce.password);

#ifdef _DEBUG
		puts("\n== Decrypted Data ==");
		DumpBinary(pkey.plain, pkey.crypto_len);
#endif
	}
	else
	{
		printf(	"\n= Decrypt Failed =\n"
				"Sorry, please change password range or charset and run again.\n\n");
	}


// NPKI Private Key Struct 동적할당 해제
	NPK_Free(&pkey);
	return 0;
}


void JV_Help ()
{
	printf(	"= Usage =\n"
			"./NPKICracker -f <PrivateKey> -m <MinPWLen> -M <MaxPWLen> [Optionial...]\n"
			"\n"
			"= Must-need Arguments =\n"
			"-f  : Must Provide NPKI's private key file path\n"
			"-m  : Minimum length of attacking password (default is 8)\n"
			"-M  : Maximum length of attacking password (default is 12)\n"
			"-c  : Charset pool of attacking password\n"
			"\n"
			"= Optionial Arguments =\n"
			"-i  : Start from this initial password\n"
			"        Usage : -i <InitialPassword>\n"
			"-p  : Use custom print interval\n"
			"        Usage : -p <PrintInterval>\n"
			"-cl : Use OpenCL\n"
			"        If OpenCL is enabled, cpu multithreading will be disabled.\n"
            "        OpenCL functionality is not available now.\n");
}
