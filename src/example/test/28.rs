/*
全局变量定义的时候不能使用类型推断
*/


int g_a=2
rbuf<char> g_b(3)

void main()
{
	printl g_a
	b=g_b
	b.count.printl
}