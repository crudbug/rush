class int
{
	rd32 m_in

	enum
	{
		c_min=4294967295,
		c_max=0x7fffffff
	}

	final ~int()
	{
	}

	final int()
	{
	}

	final int(int a)
	{
		this=a;
	}

	final int(int& a)
	{
		this=a;
	}

	final int(char a)
	{
		this=(UCHAR)a;
	}

	final int(ushort a)
	{
		this=a;
	}

	final TYPE to<TYPE>()
	{
		return (TYPE)this;
	}
	
	template<>
	char to<char>()
	{
		return tochar()
	}
	
	template<>
	ushort to<ushort>()
	{
		return toushort
	}
	
	template<>
	int64 to<int64>()
	{
		return toint64
	}

	final ushort toushort()
	{
		return (ushort)this;
	}

	final int toint()
	{
		return this;
	}

	final uint touint()
	{
		return this;
	}
	
	int64 toint64()
	{
		int64 ret(this)
		return ret
	}
	
	double todouble()
	{
		double ret(this)
		return ret
	}

	rstr torstr()
	{
		rstr ret(this)
		return ret
	}
	
	final char tochar()
	{
		return (char)this;
	}

	final void operator=(int a)
	{
		this=a;
	}

	final void operator=(int& a)
	{
		this=a;
	}

	final void operator=(uint a)
	{
		this=a;
	}

	friend operator<->(int& a,int& b)
	{
		c=a
		a=b
		b=c
	}

	final friend int& operator=>(int a,int& this)
	{
		this=a;
		return this;
	}

	final void operator+=(int a)
	{
		this+=a;
	}

	final void operator-=(int a)
	{
		this-=a;
	}

	final void operator*=(int a)
	{
		this*=a;
	}
	
	final void operator/=(int a)
	{
		this/=a;
	}
	
	final void operator%=(int a)
	{
		this%=a;
	}

	final friend int operator neg(int a)
	{
		return -a;
	}

	final void operator++()
	{
		this++;
	}

	final void operator--()
	{
		this--;
	}

	final friend int operator+(int a,int b) 
	{
		return a+b;
	}

	final friend int operator-(int a,int b)
	{
		return a-b;
	}

	final friend int operator*(int a,int b)
	{
		return a*b;
	}

	final friend int operator/(int a,int b)
	{
		return a/b;
	}

	final friend int operator%(int a,int b)
	{
		return a%b;
	}

	final friend bool operator<(int a,int b)
	{
		return a<b;
	}

	final friend bool operator<=(int a,int b)
	{
		return a<=b;
	}

	final friend bool operator==(int a,int b)
	{
		return a==b;
	}

	final friend bool operator!=(int a,int b)
	{
		return a!=b;
	}

	final friend bool operator>(int a,int b)
	{
		return a>b;
	}

	final friend bool operator>=(int a,int b)
	{
		return a>=b;
	}

	final friend bool operator!(int a)
	{
		return !a;
	}

	final friend bool operator&&(int a,int b)
	{
		return a&&b;
	}

	final friend bool operator||(int a,int b)
	{
		return a||b;
	}
	
	int abs()
	{
		if this<0
			return -this
		return this
	}

	void set(int a)
	{
		this=a
	}

	void clear()
	{
		this=0
	}

	bool empty()
	{
		return 0==this
	}

	void print()
	{
		rf.print(this)
	}

	void printl()
	{
		rf.printl(this)
	}
}
