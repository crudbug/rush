/*
rush一般不需要使用new，
推荐使用rbuf分配内存，
因为rbuf会在析构时自动释放内存
*/

void main()
{
	p=r_new<int>(5)
	p[2]=3
	p[2].printl
	r_delete<int>(p)
	
	rbuf<int> a(5)
	a[3]=2
	a[3].printl

	printl a.begin==a.m_p
}
