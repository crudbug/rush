class rp<T>
{
	rdp m_in

	~rp<T>()
	{
	}

	rp<T>()
	{
	}

	rp<T>(rp<T> a)
	{
		mov esi,this
		mov [esi],a
	}

	rp<T>(rp<T>& a)
	{
		mov esi,this
		mov edi,a
		mov [esi],[edi]
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

	void operator=(rp<T> a)
	{
		mov esi,this
		mov [esi],a
	}

	void operator=(rp<T>& a)
	{
		mov esi,this
		mov edi,a
		mov [esi],[edi]
	}
	
	void operator=(int a)
	{
		mov esi,this
		mov [esi],a
	}

	T& operator*()
	{
		mov esi,this
		mov s_ret,[esi]
	}
	
	//指针再取地址只能返回空指针，否则会产生递归定义
	rp<void> operator&()
	{
		mov s_ret,this
	}

	void operator++()
	{
		mov esi,this
		add [esi],sizeof(T)
	}

	void operator--()
	{
		mov esi,this
		sub [esi],sizeof(T)
	}

	void operator+=(int a)
	{
		a*=sizeof(T)
		mov esi,this
		add [esi],a
	}

	void operator+=(uint a)
	{
		a*=sizeof(T)
		mov esi,this
		add [esi],a
	}

	void operator-=(int a)
	{
		a*=sizeof(T)
		mov esi,this
		sub [esi],a
	}

	void operator-=(uint a)
	{
		a*=sizeof(T)
		mov esi,this
		sub [esi],a
	}

	T& operator[](int a)
	{
		a*=sizeof(T)
		mov esi,this
		mov esi,[esi]
		add esi,a
		mov s_ret,esi
	}

	T& operator[](uint a)
	{
		a*=sizeof(T)
		mov esi,this
		mov esi,[esi]
		add esi,a
		mov s_ret,esi
	}

	friend rp<T> operator+(rp<T> a,int b)
	{
		b*=sizeof(T)
		add a,b
		mov s_ret,a
	}

	friend rp<T> operator+(rp<T> a,uint b)
	{
		b*=sizeof(T)
		add a,b
		mov s_ret,a
	}

	friend rp<T> operator-(rp<T> a,int b)
	{
		b*=sizeof(T)
		sub a,b
		mov s_ret,a
	}

	friend rp<T> operator-(rp<T> a,uint b)
	{
		b*=sizeof(T)
		sub a,b
		mov s_ret,a
	}

	friend int operator-(rp<T> a,rp<T> b)
	{
		sub a,b
		idiv a,sizeof T
		mov s_ret,a
	}

	friend bool operator<(rp<T> a,rp<T> b)
	{
		_uclsb a,b
		mov s_ret,ebx
	}

	friend bool operator<=(rp<T> a,rp<T> b)
	{
		_uclesb a,b
		mov s_ret,ebx
	}

	friend bool operator>(rp<T> a,rp<T> b)
	{
		_ucgsb a,b
		mov s_ret,ebx
	}

	friend bool operator>=(rp<T> a,rp<T> b)
	{
		_ucgesb a,b
		mov s_ret,ebx
	}

	friend bool operator==(rp<T> a,rp<T> b)
	{
		cesb a,b
		mov s_ret,ebx
	}

	friend bool operator!=(rp<T> a,rp<T> b)
	{
		cnesb a,b
		mov s_ret,ebx
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
