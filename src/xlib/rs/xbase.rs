#include "rp.rs"
#include "char.rs"
#include "int.rs"
#include "uint.rs"
#include "ushort.rs"
#include "int64.rs"
#include "double.rs"
#include "float.rs"

#include "ralgo.h"
#include "rbuf.h"
#include "rstr.h"
#include "rstrw.h"
#include "rcode.h"
#include "rf.h"
#include "rmutex.h"

#define vector rbuf
#define string rstr
#define bool int
#define rbool int
#define long int64
#define uchar char
#define wchar ushort
#define r_size sizeof
#define r_move
#define r_ceil_div(a,b) r_cond(0==(a)%(b),(a)/(b),(a)/(b)+1)

#define r_max(a,b) r_cond(a>b,a,b)
#define r_min(a,b) r_cond(a<b,a,b)

/*#define r_to_int(a) ((int)(a))
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
#define r_to_prstr(a) ((rstr*)(a))*/

#define true 1
#define false 0

#define null 0p
#define NULL 0p

#define elif else if
#define while for
#define default else
#define namespace friend class
#define goto jmp
#define struct class
#define static friend
#define const
#define inline
#define asm
#define auto
#define then
#define volatile
#define register
#define signed
#define unsigned
#define operator
#define typename

#define <- =
#define :: .

//与do...while相反
mac until(a)
{
	if(a)
	{
		break;
	}
}

mac$ @ _word ( _mword )
{
	stdcall[#$0,$1]
}

mac$ @@ _word ( _mword )
{
	cdecl[#$0,$1]
}

cfunc void main_c()
{
	sub esp,4
	call &main
	add esp,4
}

int r_cond(bool cond,int a,int b)
{
	if(cond)
	{
		return a
	}
	return b
}

T r_cond<T>(bool cond,T a,T b)
{
	if(cond)
	{
		return a
	}
	return b
}

char r_char(rstr s)
{
	return s[0]
}

void print(rstr s)
{
	rf.print(s);
}

void printl(rstr s=rstr())
{
	rf.printl(s);
}

void* findf(rstr s)
{
	return xf.find_func(s.cstr)
}

//推荐使用函数进行位运算，
//如果一定要使用位运算符请自行DIY位运算符和优先级
int NOT(int a)
{
	bnot a
	mov s_ret,a
}

int AND(int a,int b)
{
	band a,b
	mov s_ret,a
}

int OR(int a,int b)
{
	bor a,b
	mov s_ret,a
}

int ORM(int num)
{
	int* p=&num+1
	int ret=*p
	p++
	for(i=1;i<num;i++)
		ret=OR(ret,*p)
		p++
	*p=ret
	mov ecx,sizeof(int)
	imul ecx,num
	add ecx,4
	pop ebp
	add esp,sizeof(s_local)
	mov eax,[esp]
	_reti(eax,ecx)
}

int XOR(int a,int b)
{
	bxor a,b
	mov s_ret,a
}

int SHL(int a,int b)
{
#ifdef _RVM
	bshl a,b
	mov s_ret,a
#else
	push b
	push a
	calle "bshl"
	mov s_ret,eax
	add esp,8
#endif
}

int SHR(int a,int b)
{
#ifdef _RVM
	bshr a,b
	mov s_ret,a
#else
	push b
	push a
	calle "bshr"
	mov s_ret,eax
	add esp,8
#endif
}

int SAR(int a,int b)
{
#ifdef _RVM
	bsar a,b
	mov s_ret,a
#else
	push b
	push a
	calle "bsar"
	mov s_ret,eax
	add esp,8
#endif
}

rbuf<T> array<T>(int num)
{
	T* p=&num+1
	rbuf<T>* pret=p+num
	pret->rbuf<T>()
	for(i=0;i<num;i++)
	{
		pret->push(*p)
		//T& temp
		//mov temp,p
		T.~T(*p)
		p++
	}
	mov ecx,sizeof(T)
	imul ecx,num
	add ecx,4
	pop ebp
	add esp,sizeof(s_local)
	mov eax,[esp]
	_reti(eax,ecx)
}

//采用类似的办法可以回溯函数的调用栈，
//即可从被调用函数访问到调用函数的局部变量
_reti(int addr,int i)
{
	mov ecx,addr
	mov eax,i
	pop ebp
	add esp,16//4+8+4
	add esp,eax
#ifdef _RNASM
	rn jmp ecx
#endif
#ifdef _RJIT
	jmp ecx
#endif
#ifdef _RVM
	jmp ecx
#endif
}

int stdcall(int count)
{
	count4=count*4
	char* name=(*(&count+1)).to<char*>
	pret=&count+count+1
	pfunc=xf.find_dll_func(name)
	if pfunc==null
		printl(name)
		return 0
	pstart=&count
	
	mov ebx,count
	sub ebx,1
	mov ecx,count4
	mov esi,pstart
	add esi,ecx
next:
	jebxz end
	push [esi]
	sub ebx,1
	sub esi,4
	jmp next
end:
	call pfunc
	mov edi,pret
	mov [edi],eax

	mov ecx,4
	imul ecx,count
	add ecx,4
	pop ebp
	add esp,sizeof(s_local)
	mov eax,[esp]
	_reti(eax,ecx)
}

void __declare()
{
	//todo:
	rbuf<char*> temp
}
