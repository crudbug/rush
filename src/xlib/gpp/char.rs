class char
{
	rd8 m_in

	final ~char()
	{
	}

	final char()
	{
	}

	final char(char& a)
	{
		this=a;
	}

	final char(char a)
	{
		this=a;
	}
	
	TYPE to<TYPE>()
	{
		xf.print("error\n")
	}
	
	template<>
	uint to<uint>()
	{
		return touint
	}
	
	template<>
	int to<int>()
	{
		return toint
	}
	
	template<>
	ushort to<ushort>()
	{
		return toint.toushort
	}
	
	template<>
	int64 to<int64>()
	{
		return toint.toint64
	}

	final int toint()
	{
		return (UCHAR)this;
	}

	final uint touint()
	{
		return (UCHAR)this;
	}

	/*rstr torstr()
	{
		rstr ret(this)
		return ret
	}*/

	char tochar()
	{
		mov esi,this
		mov1 s_ret,[esi]
	}

	final void operator=(char& a)
	{
		this=a;
	}

	final void operator=(char a)
	{
		this=a;
	}
	
	final void operator=(int a)
	{
		this=(char)a;
	}

	/*void operator=(uint a)
	{
		mov esi,this
		mov1 [esi],a
	}*/

	final void operator++()
	{
		this++;
	}

	friend operator<->(char& a,char& b)
	{
		c=a
		a=b
		b=c
	}

	final friend bool operator==(char a,char b)
	{
		return a==b;
	}

	final friend bool operator!=(char a,char b)
	{
		return a!=b;
	}

	final friend bool operator>(char a,char b)
	{
		return (uchar)a>(uchar)b;
	}

	final friend bool operator>=(char a,char b)
	{
		return (uchar)a>=(uchar)b;
	}

	final friend bool operator<(char a,char b)
	{
		return (uchar)a<(uchar)b;
	}

	final friend bool operator<=(char a,char b)
	{
		return (uchar)a<=(uchar)b;
	}

	/*void set(char a)
	{
		mov esi,this
		mov1 [esi],a
	}

	void set(int a)
	{
		mov esi,this
		mov1 [esi],a
	}

	void clear()
	{
		set(0)
	}

	bool empty()
	{
		return this.toint==0
	}*/

	/*void print()
	{
		rf.print this.torstr
	}

	void printl()
	{
		rf.printl this.torstr
	}*/
}
