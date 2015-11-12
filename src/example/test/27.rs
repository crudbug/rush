/*
指针和引用
*/

void main()
{
	a=2
	
#ifndef _RGPP
	int& b
	lea b,[ebp+s_off a]
#else
	int b=a
#endif

	a.printl
	b.printl
	
	func(a)
	a.printl

	a=3
	p=&a
	a.printl
	p->printl

	a=4
	p=&a
	int** pp=&p
	printl *(*pp)

	rp<rp<rp<int>>> ppp=&pp
	printl *(*(*ppp))
}

void func(int& n)
{
	n=5
}