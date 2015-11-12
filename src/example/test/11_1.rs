/*
宏
*/

void main()
{
	c=fadd(1,2)*fadd(1,2)
	printl c

	c=fadd2(1,2)*fadd2(1,2)
	printl c

	fpro(+=)
	fpro(*=)
}

mac fadd(a,b) a+b

mac fadd2(a,b)
{
	a+b
}

mac fpro(T)
{
	pro=1;
	for(i=2;i<=10;i++)
	{
		pro T i;
	}
	printl(pro);
}