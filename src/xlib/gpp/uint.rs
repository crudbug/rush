class uint
{
	rd32 m_in

	enum
	{
		c_min=0,
		c_max=0xffffffff,
	}

	final ~uint()
	{
	}

	final uint()
	{
	}

	final uint(uint a)
	{
		this=a;
	}

	final uint(uint& a)
	{
		this=a;
	}

	final uint(int a)
	{
		this=a;
	}

	final uint(char a)
	{
		this=(uchar)a;
	}

	final TYPE to<TYPE>()
	{
		return (TYPE)this;
	}
	
	template<>
	final char to<char>()
	{
		return (char)((uchar)this);
	}
	
	template<>
	final ushort to<ushort>()
	{
		return (ushort)this;
	}
	
	template<>
	final int64 to<int64>()
	{
		return this;
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
		rstr ret(this)
		return ret
	}

	final void operator=(uint a)
	{
		this=a;
	}

	final void operator=(uint& a)
	{
		this=a;
	}

	friend operator<->(uint& a,uint& b)
	{
		c=a
		a=b
		b=c
	}

	final void operator+=(uint a)
	{
		this+=a;
	}

	final void operator-=(uint a)
	{
		this-=a;
	}

	final void operator*=(uint a)
	{
		this*=a;
	}

	final void operator/=(uint a)
	{
		this/=a;
	}

	final void operator%=(uint a)
	{
		this%=a;
	}

	final void operator++()
	{
		this++;
	}

	final void operator--()
	{
		this--;
	}

	final friend uint operator+(uint a,uint b) 
	{
		return a+b;
	}

	final friend uint operator-(uint a,uint b)
	{
		return a-b;
	}

	final friend uint operator*(uint a,uint b)
	{
		return a*b;
	}

	final friend uint operator/(uint a,uint b)
	{
		return a/b;
	}

	final friend uint operator%(uint a,uint b)
	{
		return a%b;
	}

	final friend bool operator<(uint a,uint b)
	{
		return a<b;
	}

	final friend bool operator<=(uint a,uint b)
	{
		return a<=b;
	}

	final friend bool operator==(uint a,uint b)
	{
		return a==b;
	}

	final friend bool operator!=(uint a,uint b)
	{
		return a!=b;
	}

	final friend bool operator>(uint a,uint b)
	{
		return a>b;
	}

	final friend bool operator>=(uint a,uint b)
	{
		return a>=b;
	}

	/*void set(uint a)
	{
		mov esi,this
		mov [esi],a
	}

	void clear()
	{
		mov esi,this
		mov [esi],0
	}

	bool empty()
	{
		return 0==this
	}*/

	/*void print()
	{
		rf.print this
	}

	void printl()
	{
		rf.printl this
	}*/
}
