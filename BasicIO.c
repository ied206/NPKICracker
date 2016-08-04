#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "BasicIO.h"

int scanfile (const char name[]) // 파일의 존재 여부 검사
{
	FILE *fp;
	int is_file_exist = TRUE;

	fp = fopen(name, "r");
	if (fp == NULL)
		is_file_exist = FALSE;
	else
		fclose(fp);
	return is_file_exist;
}

int WhatBitOS() // 몇비트 컴파일인가 알아내기
{
	if (sizeof(int*) == 4)
		return 32;
	else if (sizeof(int*) == 8)
		return 64;
	else
		return (sizeof(int*) * 8);
}

uint8_t BytePrefix(uint64_t sizelen)
{
	uint8_t whatbyte;

	if (sizelen < BIN_KILOBYTE) // Byte
		whatbyte = ENUM_BYTE;
	else if (sizelen < BIN_MEGABYTE) // KB
		whatbyte = ENUM_KILOBYTE;
	else if (sizelen < BIN_GIGABYTE) // MB
		whatbyte = ENUM_MEGABYTE;
	else // GB
		whatbyte = ENUM_GIGABYTE;

	return whatbyte;
}

int CompileYear () // 컴파일한 년도
{
	const char macro[16] = __DATE__;
	char stmp[8] = {0}; // 전체 0으로 초기화

	stmp[0] = macro[7];
	stmp[1] = macro[8];
	stmp[2] = macro[9];
	stmp[3] = macro[10];
	stmp[4] = '\0';

	return atoi(stmp);
}

int CompileMonth ()
{
	// 컴파일한 월 표시
	const char macro[16] = __DATE__;
	const char smonth[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	int i = 0;

// 월 감지
	for (i = 0; i < 12; i++)
	{
		if (strstr(macro, smonth[i]) != NULL)
			return i + 1;
	}

// -1이 반환되는 경우는 월 인식 불가
	return -1;
}

int CompileDate ()
{
	// 컴파일한 일 표시
	const char macro[16] = __DATE__;
	char stmp[4] = {0}; // 전체 0으로 초기화

// 일 감지
	stmp[0] = macro[4];
	stmp[1] = macro[5];
	stmp[2] = '\0';
	return atoi(stmp);
}

long ReadFileSize(const char* filename)
{
	FILE *fp = NULL;
	long filesize;

	if (!(scanfile(filename)))
	{
		// 파일이 존재하지 않다면
		fprintf(stderr, "Error in ReadFileSize()\n%s not exists!\n", filename);
		return -1;
	}

	fp = fopen(filename, "r"); // fp에서 파일을 연 뒤
	fseek(fp, 0, SEEK_END); // 포인터를 맨 뒤로 이동
	filesize = ftell(fp); // 크기를 sizelen에 넣는다.
	fclose(fp); // 파일 닫기
	return filesize;
}

// Linux-Style Hex Dump
void DumpBinary(const uint8_t buf[], const uint32_t bufsize)
{
	uint32_t base = 0;
	uint32_t interval = 16;
	while (base < bufsize)
	{
		if (base + 16 < bufsize)
			interval = 16;
		else
			interval = bufsize - base;

		printf("0x%04x:   ", base);
		for (uint32_t i = base; i < base + 16; i++) // i for dump
		{
			if (i < base + interval)
				printf("%02x", buf[i]);
			else
			{
				putchar(' ');
				putchar(' ');
			}

			if ((i+1) % 2 == 0)
				putchar(' ');
			if ((i+1) % 8 == 0)
				putchar(' ');
		}
		putchar(' ');
		putchar(' ');
		for (uint32_t i = base; i < base + 16; i++) // i for dump
		{
			if (i < base + interval)
			{
				if (0x20 <= buf[i] && buf[i] <= 0x7E)
					printf("%c", buf[i]);
				else
					putchar('.');
			}
			else
			{
				putchar(' ');
				putchar(' ');
			}

			if ((i+1) % 8 == 0)
				putchar(' ');
		}
		putchar('\n');


		if (base + 16 < bufsize)
			base += 16;
		else
			base = bufsize;
	}

	return;
}

// Padding Compile로는 부족. 앞이 3082인지도 검사
int IsPKCS5PaddingOK(const uint8_t* buf, const uint32_t buflen)
{
	if (buf[0] != 0x30 || buf[1] != 0x82)
		return FALSE;

	for (int i = 1; i < buf[buflen-1]; i++)
	{
		if (buf[buflen-1-i] != buf[buflen-1])
			return FALSE;
	}

	if (buf[buflen-1] == 0)
		return FALSE;

	return TRUE;
}

uint64_t ipow (uint32_t low, uint32_t upper)
{
	uint64_t result = 1;
	for (uint32_t i = 0; i < upper; i++)
		result *= low;
	return result;
}
