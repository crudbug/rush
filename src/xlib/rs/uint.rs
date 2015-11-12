class uint
{
	rd32 m_in

	enum
	{
		c_min=0,
		c_max=0xffffffff,
	}

	~uint()
	{
	}

	uint()
	{
	}

	uint(uint a)
	{
		mov esi,this
		mov [esi],a
	}

	uint(uint& a)
	{
		mov edi,this
		mov esi,a
		mov [edi],[esi]
	}

	uint(int a)
	{
		mov esi,this
		mov [esi],a
	}

	uint(char a)
	{
		mov edi,this
		mov [edi],0
		movb [edi],a
	}

	TYPE to<TYPE>()
	{
		mov esi,this
		mov s_ret,[esi]
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
	
	template<>
	int64 to<int64>()
	{
		return toint.toint64
	}

	int toint()
	{
		mov esi,this
		mov s_ret,[esi]
	}

	uint touint()
	{
		mov esi,this
		mov s_ret,[esi]
	}

	rstr torstr()
	{
		rstr ret(this)
		return ret
	}

	void operator=(uint a)
	{
		mov esi,this
		mov [esi],a
	}

	void operator=(uint& a)
	{
		mov edi,this
		mov esi,a
		mov [edi],[esi]
	}

	friend operator<->(uint& a,uint& b)
	{
		c=a
		a=b
		b=c
	}

	void operator+=(uint a)
	{
		mov esi,this
		add [esi],a
	}

	void operator-=(uint a)
	{
		mov esi,this
		sub [esi],a
	}

	void operator*=(uint a)
	{
		mov esi,this
		imul [esi],a
	}

	void operator/=(uint a)
	{
		mov esi,this
		_udiv [esi],a
	}

	void operator%=(uint a)
	{
		mov esi,this
		_umod [esi],a
	}

	void operator++()
	{
		mov esi,this
		add [esi],1
	}

	void operator--()
	{
		mov esi,this
		sub [esi],1
	}

	friend uint operator+(uint a,uint b) 
	{
		add a,b
		mov s_ret,a
	}

	friend uint operator-(uint a,uint b)
	{
		sub a,b
		mov s_ret,a
	}

	friend uint operator*(uint a,uint b)
	{
		imul a,b
		mov s_ret,a
	}

	friend uint operator/(uint a,uint b)
	{
		_udiv a,b
		mov s_ret,a
	}

	friend uint operator%(uint a,uint b)
	{
		_umod a,b
		mov s_ret,a
	}

	friend bool operator<(uint a,uint b)
	{
		_uclsb a,b
		mov s_ret,ebx
	}

	friend bool operator<=(uint a,uint b)
	{
		_uclesb a,b
		mov s_ret,ebx
	}

	friend bool operator==(uint a,uint b)
	{
		cesb a,b
		mov s_ret,ebx
	}

	friend bool operator!=(uint a,uint b)
	{
		cnesb a,b
		mov s_ret,ebx
	}

	friend bool operator>(uint a,uint b)
	{
		return b<a;
	}

	friend bool operator>=(uint a,uint b)
	{
		return b<=a;
	}

	void set(uint a)
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
	}

	/*void print()
	{
		rf.print this
	}

	void printl()
	{
		rf.printl this
	}*/
}
