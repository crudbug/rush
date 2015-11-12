
void main()
{
	test1
	test2
	test3
	test4
	test5
	test6
}

//浮点测试
void test1()
{
	printl 0.2+0.3
	printl 0.2*0.3
	printl 0.2/0.3
	printl 0.2-0.3
	printl 0.2<0.3
	printl 0.2>0.3
	a=99.2*20
	printl a
	printl(-2)
}

//逻辑运算测试
void test2()
{
	printl 0&&0
	printl 1&&0
	printl 0&&1
	printl 1&&1
	printl 0||0
	printl 1||0
	printl 0||1
	printl 1||1
}

//自动类型转换测试
void test3()
{
	uint a=1
	int c=5
	printl a+sizeof(int)!=c
}

//64位运算测试
void test4()
{
	int64 a=9
	int64 b=3
	printl a/b
	printl a*b
	printl a%b
	printl a-b
	printl a<b
	printl a>b
}

void test5()
{
	if (2)==2
		printl('ok',,,)
}

void test6()
{
	printl ORM[1,2]
	printl ORM[1,2,4,8]
	printl ORM[1]
}