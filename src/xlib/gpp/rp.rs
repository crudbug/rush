class rp<T>
{
	rdp m_in

	final ~rp<T>()
	{
	}

	final rp<T>()
	{
	}

	final rp<T>(rp<T> a)
	{
		this=a;
	}

	final rp<T>(rp<T>& a)
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

	final int toint()
	{
		return (int)this;
	}

	final uint touint()
	{
		return (uint)this;
	}

	final void operator=(rp<T> a)
	{
		this=a;
	}

	final void operator=(rp<T>& a)
	{
		this=a;
	}
	
	/*void operator=(int a)
	{
		mov esi,this
		mov [esi],a
	}*/

	final T& operator*()
	{
		//return *(T R_P)this;
	}
	
	//指针再取地址只能返回空指针，否则会产生递归定义
	final rp<void> operator&()
	{
		return this;
	}

	final void operator++()
	{
		this=((CHAR*)this)+R_SIZE(T);
	}

	final void operator--()
	{
		this=((CHAR*)this)-R_SIZE(T);
	}

	final void operator+=(int a)
	{
		this=((CHAR*)this)+R_SIZE(T)*a;
	}

	final void operator+=(uint a)
	{
		this=((CHAR*)this)+R_SIZE(T)*a;
	}

	final void operator-=(int a)
	{
		this=((CHAR*)this)-R_SIZE(T)*a;
	}

	final void operator-=(uint a)
	{
		this=((CHAR*)this)-R_SIZE(T)*a;
	}

	final T& operator[](int a)
	{
		return *(T R_P)(((CHAR*)this)+R_SIZE(T)*a);
	}

	final T& operator[](uint a)
	{
		return ((CHAR*)this)+R_SIZE(T)*a;
	}

	final friend rp<T> operator+(rp<T> a,int b)
	{
		return ((CHAR*)a)+R_SIZE(T)*b;
	}

	final friend rp<T> operator+(rp<T> a,uint b)
	{
		return ((CHAR*)a)+R_SIZE(T)*b;
	}

	final friend rp<T> operator-(rp<T> a,int b)
	{
		return ((CHAR*)a)-R_SIZE(T)*b;
	}

	final friend rp<T> operator-(rp<T> a,uint b)
	{
		return ((CHAR*)a)-R_SIZE(T)*b;
	}

	final friend int operator-(rp<T> a,rp<T> b)
	{
		return ((CHAR*)a-(CHAR*)b)/R_SIZE(T);
	}

	final friend bool operator<(rp<T> a,rp<T> b)
	{
		return a<b;
	}

	final friend bool operator<=(rp<T> a,rp<T> b)
	{
		return a<=b;
	}

	final friend bool operator>(rp<T> a,rp<T> b)
	{
		return a>b;
	}

	final friend bool operator>=(rp<T> a,rp<T> b)
	{
		return a>=b;
	}

	final friend bool operator==(rp<T> a,rp<T> b)
	{
		return a==b;
	}

	final friend bool operator!=(rp<T> a,rp<T> b)
	{
		return a!=b;
	}

	bool empty()
	{
		mov esi,this
		cesb [esi],0
		mov s_ret,ebx
	}

	void clear()
	{
		mov esi,this
		mov [esi],0
	}

	void print()
	{
		rf.print(this.touint)
	}

	void printl()
	{
		rf.printl(this.touint)
	}
}
