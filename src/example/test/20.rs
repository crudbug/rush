/*
rush的默认参数不仅可以访问到前面的参数，
还可以访问到类的成员变量
*/

int func(int a,int b=a)
{
	return a+b;
}

void main()
{
	printl func(2)
	printl func(1,2)
}