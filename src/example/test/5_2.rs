/*
rush支持各种花括号风格，
也支持类似python的控制结构省略花括号，
注意表达式过长折行的时候不能省略括号
*/

void main()
{
	if(1||
		2)
		printl "true"
		
	if 1
		printl 2
		printl 3
		
	if 1 {
		printl 2
		printl 3
	}
	
	if(1) 
	{
		printl(2);
		printl(3);
	}
	
	if 1 {printl(2);}

	func(1,
		2)

	func(1,2
		)
}

void func(int a,
	int b)
{
	printl a+b
}