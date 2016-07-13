#ifndef __BasicInput_h
#define __BasicInput_h

int scanfile (const char name[]); // 파일의 존재 여부 검사

int WhatBitOS (); // 몇비트 컴파일인가 알아내기
uint8_t BytePrefix(uint64_t sizelen);
int CompileYear ();
int CompileMonth ();
int CompileDate ();
long ReadFileSize(const char* filename);
void DumpBinary(const uint8_t buf[], const uint32_t bufsize);
int IsPKCS5PaddingOK(const uint8_t* buf, const uint32_t buflen);
uint64_t ipow (uint32_t low, uint32_t upper);

#define ms2rsec(X) ((X) / 1000)
#define ms2rmin(X) ((X) / 1000 / 60)
#define ms2rhour(X) ((X) / 1000 / 60 / 60)

#define ms2sec(X) (((X) / 1000) % 60)
#define ms2min(X) (((X) / 1000 / 60) % 60)
#define ms2hour(X) (((X) / 1000 / (60 * 60)) % 24)

#define sec2sec(X) ((X) % 60)
#define sec2min(X) (((X) / 60) % 60)
#define sec2hour(X) (((X) / (60 * 60)) % 24)
#define sec2day(X) ((X) / (60 * 60 * 24))

#define rsec2sec(X) ((X) % 60)
#define rsec2min(X) ((X) / 60)
#define rsec2hour(X) ((X) / 60 / 60)

#define BIN_KILOBYTE 1024
#define BIN_MEGABYTE (1024 * 1024)
#define BIN_GIGABYTE (1024 * 1024 * 1024)
#define STR_KILOBYTE "KB"
#define STR_MEGABYTE "MB"
#define STR_GIGABYTE "GB"
#define ENUM_BYTE 		1
#define ENUM_KILOBYTE 	2
#define ENUM_MEGABYTE 	3
#define ENUM_GIGABYTE 	4
#define ENUM_TERABYTE 	5

#define TRUE 1
#define FALSE 0

#define MAJOR_VER  1
#define MINOR_VER  0
// #define _DEBUG

#endif
