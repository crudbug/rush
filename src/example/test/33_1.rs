/*
模板类
*/

class A<T>
{
	T m_a

	A<T>()
	{
	}
}

void main()
{
	A<int> a
	A<A<int>> b
	
	a.m_a=2
	a.m_a.printl
	b.m_a.m_a=3
	b.m_a.m_a.printl
}
