class double
{
	rd64 m_in

	final ~double()
	{
	}

	final double()
	{
	}

	final double(double a)
	{
		this=a;
	}

	final double(double& a)
	{
		this=a;
	}
	
	final double(int a)
	{
		this=a;
	}

	final double(uint a)
	{
		this=a;
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
	
	friend double operator neg(double a)
	{
		return 0.0-a;
	}

	final void operator=(double a)
	{
		this=a;
	}

	final void operator=(double& a)
	{
		this=a;
	}

	friend operator<->(double& a,double& b)
	{
		c=a
		a=b
		b=c
	}

	final void operator+=(double a)
	{
		this+=a;
	}

	final void operator-=(double a)
	{
		this-=a;
	}

	final void operator*=(double a)
	{
		this*=a;
	}
	
	final void operator/=(double a)
	{
		this/=a;
	}

	friend double operator+(double a,double b) 
	{
		a+=b
		return a
	}

	friend double operator-(double a,double b)
	{
		a-=b
		return a
	}

	friend double operator*(double a,double b)
	{
		a*=b
		return a
	}

	friend double operator/(double a,double b)
	{
		a/=b
		return a
	}

	final friend bool operator<(double a,double b)
	{
		return a<b;
	}

	friend bool operator<=(double a,double b)
	{
		return a<b||a==b;
	}

	final friend bool operator==(double a,double b)
	{
		return a==b;
	}

	friend bool operator!=(double a,double b)
	{
		return !(a==b)
	}

	friend bool operator>(double a,double b)
	{
		return b<a;
	}

	friend bool operator>=(double a,double b)
	{
		return a>b||a==b;
	}
	
	double abs()
	{
		if this<0.0
			return 0.0-this
		return this
	}

	/*void clear()
	{
		mov esi,this
		mov [esi],0
		mov [esi+4],0
	}

	bool empty()
	{
		return 0==this
	}*/

	/*print()
	{
		rf.print this
	}

	printl()
	{
		rf.printl this
	}*/
}
