#pragma once

#ifdef _MSC_VER

#pragma warning(disable:4996)
#pragma warning(disable:4345)

#pragma comment(lib,"Ws2_32")

#include <winsock2.h>
#include <windows.h>
#include <wchar.h>
#include <conio.h>
#include <io.h>
#include <process.h>
#else
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include <semaphore.h>
#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

#include <utility>
#include <new>

#pragma pack(1)

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef ushort
typedef unsigned short ushort;
#endif

#ifndef uchar
typedef unsigned char uchar;
#endif

#ifndef rbool
typedef int rbool;
#endif

#ifndef int64

#ifdef _MSC_VER
typedef __int64 int64;
typedef unsigned __int64 uint64;
#else
typedef long long int64;
typedef unsigned long long uint64;
#endif

#endif

#ifndef wchar

#ifdef _MSC_VER
typedef wchar_t wchar;
#else
typedef ushort wchar;
#endif

#endif

#ifdef _MSC_VER
typedef CRITICAL_SECTION R_CRITICAL_SECTION;
typedef WIN32_FIND_DATAW R_WIN32_FIND_DATAW;
#else
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
struct R_WIN32_FIND_DATAW
{
	int dwFileAttributes;
	int64 ftCreationTime;
	int64 ftLastAccessTime;
	int64 ftLastWriteTime;
	int nFileSizeHigh;
	int nFileSizeLow;
	int dwReserved0;
	int dwReserved1;
	wchar cFileName[260];
	wchar cAlternateFileName[14];
};
struct R_CRITICAL_SECTION
{
	char buf[24];
};
#endif

#define null nullptr
#define elif else if
#define ifn(a) if(!(a))

#define r_char(a) a
#define r_size(a) ((int)sizeof(a))
#define r_move(a) (std::move(a))
#define r_max(a,b) ((a)>(b)?(a):(b))
#define r_min(a,b) ((a)>(b)?(b):(a))
#define r_cond(cond,a,b) ((cond)?(a):(b))
#define r_ceil_div(a,b) r_cond(0==(a)%(b),(a)/(b),(a)/(b)+1)

#define r_to_int(a) ((int)(a))
#define r_to_uint(a) ((uint)(a))
#define r_to_char(a) ((char)(a))
#define r_to_uchar(a) ((uchar)(a))
#define r_to_int64(a) ((int64)(a))
#define r_to_double(a) ((double)(a))

#define r_to_pint(a) ((int*)(a))
#define r_to_puint(a) ((uint*)(a))
#define r_to_pchar(a) ((char*)(a))
#define r_to_puchar(a) ((uchar*)(a))
#define r_to_pint64(a) ((int64*)(a))
#define r_to_pdouble(a) ((double*)(a))
#define r_to_prstr(a) ((rstr*)(a))

#define AND(a,b) ((a)&(b))
#define OR(a,b) ((a)|(b))
#define XOR(a,b) ((a)^(b))
#define NOT(a) (~(a))

#define SHL(a,b) ((int)((int)(a)<<(int)(b)))
#define SHR(a,b) ((int)((uint)(a)>>(uint)(b)))
#define SAR(a,b) ((int)((int)(a)>>(int)(b)))
