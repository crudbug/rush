#pragma once

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define uint unsigned int
#define ushort unsigned short
#define CHAR char
#define UCHAR unsigned char

#ifndef int64

#ifdef _MSC_VER
typedef __int64 int64;
typedef unsigned __int64 uint64;
#else
typedef long long int64;
typedef unsigned long long uint64;
#endif

#endif

typedef int64 int8;

/*#ifndef WCHAR

#ifdef _MSC_VER
typedef wchar_t WCHAR;
#else
typedef ushort WCHAR;
#endif

#endif*/

#define jmp goto
#define this _THIS
//#define ifn(a) if(!(a))
#define ebx _EBX
#define rdp void*
#define R_SIZE(a) ((int)sizeof(a))
#define R_P *

#define R_LL <<
#define R_RR >>
#define R_OR |
#define R_AND &
#define R_XOR ^

#pragma pack(1)
