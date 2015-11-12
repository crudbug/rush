
class A
{
	int* m_p
	
	A()
	{
		m_p=r_new<int>5	
	}
	
	~A()
	{
		if(m_p!=null)
			r_delete<int>m_p
			m_p=null
	}
}

void main()
{
	A a
	a.m_p[1]=3
	a.m_p[1].print
}
