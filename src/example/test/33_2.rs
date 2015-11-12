/*
模板类嵌套
*/

void main()
{
	A<A<int>> b
	b.m_b.m_c.m_b.m_c=3
	b.m_b.m_c.m_b.m_c.printl
	b.m_a.m_a=4
	b.m_a.m_a.printl
}

class B<T>
{
	T m_c
	T m_d
}

class A<T>
{
	T m_a
	B<T> m_b
}