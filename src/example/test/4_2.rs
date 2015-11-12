
void main()
{
	int a
	int b
	[int,=,[a,1]]
	[int,=,[b,2]]
	
	[main,printl,[[int,+,[a,b]]]]
	main.printl(a+b)
	
	[main,printl,[[int,!,[a]]]]
	
	[main,func,[2]].printl
}

int func(int a)
{
	return a*2
}