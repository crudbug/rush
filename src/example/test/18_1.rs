/*
继承

*/

class A
{
	int m_a

	void fa()
	{
		m_a=1
	}
}

class B:A
{
	void fb()
	{
		m_a=2
	}
}

void main()
{
	B b
	b.fa
	printl b.m_a
	b.fb
	printl b.m_a
}