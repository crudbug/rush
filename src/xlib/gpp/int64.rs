class int64
{
	rd64 m_in

	final ~int64()
	{
	}

	final int64()
	{
	}

	final int64(int64 a)
	{
		this=a;
	}

	final int64(int64& a)
	{
		this=a;
	}

	final int64(int a)
	{
		this=a;
	}

	final int64(uint a)
	{
		this=a;
	}
	
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
		return toint.touint
	}
	
	template<>
	char to<char>()
	{
		return toint.tochar
	}
	
	template<>
	ushort to<ushort>()
	{
		return toint.toushort
	}

	final int toint()
	{
		return (int)this;
	}

	rstr torstr()
	{
		rstr ret(this)
		return ret
	}

	final void operator=(int64 a)
	{
		this=a;
	}

	final void operator=(int64& a)
	{
		this=a;
	}

	friend operator<->(int64& a,int64& b)
	{
		c=a
		a=b
		b=c
	}

	final void operator+=(int64 a)
	{
		this+=a;
	}

	final void operator-=(int64 a)
	{
		this-=a;
	}

	final void operator*=(int64 a)
	{
		this*=a;
	}
	
	final void operator/=(int64 a)
	{
		this/=a;
	}
	
	void operator%=(int64 a)
	{
		this=this%a
	}

	void operator++()
	{
		this+=1
	}

	void operator--()
	{
		this-=1
	}

	friend int64 operator+(int64 a,int64 b) 
	{
		a+=b
		return a
	}

	friend int64 operator-(int64 a,int64 b)
	{
		a-=b
		return a
	}
	
	friend int64 operator*(int64 a,int64 b)
	{
		a*=b
		return a
	}

	friend int64 operator/(int64 a,int64 b)
	{
		a/=b
		return a
	}
	
	//todo
	friend int64 operator%(int64 a,int64 b)
	{
		return a-a/b*b
	}
	
	final friend bool operator<(int64 a,int64 b)
	{
		return a<b;
	}

	friend bool operator<=(int64 a,int64 b)
	{
		return a<b||a==b;
	}

	final friend bool operator==(int64 a,int64 b)
	{
		return a==b;
	}

	friend bool operator!=(int64 a,int64 b)
	{
		return !(a==b)
	}

	friend bool operator>(int64 a,int64 b)
	{
		return b<a;
	}

	friend bool operator>=(int64 a,int64 b)
	{
		return b<=a;
	}
	
	friend int64 operator neg(int64 a)
	{
		return 0-a;
	}

	void clear()
	{
		this=0
	}

	bool empty()
	{
		return 0==this
	}

	/*print()
	{
		rf.print this
	}

	printl()
	{
		rf.printl this
	}*/
}
