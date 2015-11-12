/*
拆分宏的另一种用法：
括号里如果是取地址符号加类名将展开为该类的所有函数成员的指针
*/

void main()
{
	#func(&B)
}

void func(void* p)
{
	printl int[p,10]
}

namespace B
{
	int fmul(int a)
	{
		return a*2
	}
	
	int fdiv(int a)
	{
		return a/2
	}
	
	int fsquare(int a)
	{
		return a*a
	}
}