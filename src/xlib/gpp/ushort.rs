class ushort
{
	rd16 m_in

	~ushort()
	{
	}

	ushort()
	{
	}

	final ushort(ushort& a)
	{
		this=a;
	}

	final ushort(ushort a)
	{
		this=a;
	}
	
	ushort(char a)
	{
		this=a.toint
	}
	
	/*short toshort()
	{
		mov esi,this
		mov1 s_ret,[esi]
		mov1 [ebp+(s_off s_ret+1)],[esi+1]
	}*/
	
	TYPE to<TYPE>()
	{
		xf.print("error\n")
	}
	
	template<>
	int to<int>()
	{
		return toint
	}
	
	template<>
	uint to<uint>()
	{
		return touint
	}

	final int toint()
	{
		return this;
	}

	final uint touint()
	{
		return this;
	}

	rstr torstr()
	{
		rstr ret(this.toint)
		return ret
	}

	final void operator=(ushort& a)
	{
		this=a;
	}

	final void operator=(ushort a)
	{
		this=a;
	}
	
	final void operator=(int a)
	{
		this=a;
	}

	final void operator=(uint a)
	{
		this=a;
	}
	
	//void operator=(uint a)
	//{

	void operator++()
	{
		this=toint+1
	}

	void operator--()
	{
		this=toint-1
	}

	friend operator<->(ushort& a,ushort& b)
	{
		c=a
		a=b
		b=c
	}

	friend bool operator==(ushort a,ushort b)
	{
		return a.toint==b.toint
	}

	friend bool operator!=(ushort a,ushort b)
	{
		return a.toint!=b.toint
	}

	friend bool operator>(ushort a,ushort b)
	{
		return a.toint>b.toint
	}

	friend bool operator>=(ushort a,ushort b)
	{
		return a.toint>=b.toint
	}

	friend bool operator<(ushort a,ushort b)
	{
		return a.toint<b.toint
	}

	friend bool operator<=(ushort a,ushort b)
	{
		return a.toint<=b.toint
	}

	void clear()
	{
		this=0
	}

	bool empty()
	{
		return this.toint==0
	}

	/*print()
	{
		rf.print this.torstr
	}

	printl()
	{
		rf.printl this.torstr
	}*/
}
