/*
函数指针不需要声明即可使用，
参数可以由编译器自动推断出类型
*/

void main()
{
	printl int[&fadd,1,2]
	
	p=&fadd
	printl int[p,1,2]

	printl int[&fadd(int,int),1,2]

	printl int[&main.fadd(int,int),1,2]
}

int fadd(int a,int b)
{
	return a+b
}