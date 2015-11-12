/*
模板函数作为类的成员
*/

void main()
{
	A a
	
	A.set<2>(a)
	a.m_a.printl
	
	A.set<5>(a)
	a.m_a.printl
	
	a.func
	a.m_a.printl
}

class A
{
	int m_a
	
	//V1.5模板函数定义的时候可以省略小括号
	void set<T>()
	{
		m_a=T
	}
	
	void func()
	{
		set<9>
	}
}