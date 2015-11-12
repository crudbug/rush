/*
多重继承
*/

class A
{
	int m_a

	void fa()
	{
		m_a=1
	}
}

class B
{
	void fb()
	{
		printl 2
	}
}

class C:A,B
{
	void fc()
	{
		printl m_a
	}
}

void main()
{
	C c
	c.fa
	c.fc
	c.fb
}