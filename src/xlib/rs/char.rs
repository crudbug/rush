class char
{
	rd8 m_in

	~char()
	{
	}

	char()
	{
	}

	char(char& a)
	{
		mov esi,this
		mov edi,a
		movb [esi],[edi]
	}

	char(char a)
	{
		mov esi,this
		movb [esi],a
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

	int toint()
	{
		mov esi,this
		mov s_ret,0
		movb s_ret,[esi]
	}

	uint touint()
	{
		mov esi,this
		mov s_ret,0
		movb s_ret,[esi]
	}

	/*rstr torstr()
	{
		rstr ret(this)
		return ret
	}*/

	char tochar()
	{
		mov esi,this
		movb s_ret,[esi]
	}

	void operator=(char& a)
	{
		mov esi,this
		mov edi,a
		movb [esi],[edi]
	}

	void operator=(char a)
	{
		mov esi,this
		movb [esi],a
	}
	
	void operator=(int a)
	{
		mov esi,this
		movb [esi],a
	}

	void operator=(uint a)
	{
		mov esi,this
		movb [esi],a
	}

	void operator++()
	{
		this=toint+1
	}

	friend operator<->(char& a,char& b)
	{
		c=a
		a=b
		b=c
	}

	friend bool operator==(char a,char b)
	{
		return a.toint==b.toint
	}

	friend bool operator!=(char a,char b)
	{
		return a.toint!=b.toint
	}

	friend bool operator>(char a,char b)
	{
		return a.toint>b.toint
	}

	friend bool operator>=(char a,char b)
	{
		return a.toint>=b.toint
	}

	friend bool operator<(char a,char b)
	{
		return a.toint<b.toint
	}

	friend bool operator<=(char a,char b)
	{
		return a.toint<=b.toint
	}

	void set(char a)
	{
		mov esi,this
		movb [esi],a
	}

	void set(int a)
	{
		mov esi,this
		movb [esi],a
	}

	void clear()
	{
		set(0)
	}

	bool empty()
	{
		return this.toint==0
	}

	/*void print()
	{
		rf.print this.torstr
	}

	void printl()
	{
		rf.printl this.torstr
	}*/
}
