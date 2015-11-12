class float
{
	rd32 m_in
	
	~float()
	{
	}

	float()
	{
	}

	float(float a)
	{
		mov esi,[esp+4]
		mov [esi],[esp+8]
	}

	float(float& a)
	{
		mov edi,[esp+4]
		mov esi,[esp+8]
		mov [edi],[esi]
	}
	
	double todouble()
	{
		return double(this)
	}
	
	int toint()
	{
		return todouble.toint
	}

	rstr torstr()
	{
		rstr ret(this)
		return ret
	}
	
	friend float operator neg(float a)
	{
		return (0.0-a).tofloat
	}
	
	void operator=(float a)
	{
		mov esi,[esp+4]
		mov [esi],[esp+8]
	}

	void operator=(float& a)
	{
		mov edi,[esp+4]
		mov esi,[esp+8]
		mov [edi],[esi]
	}
	
	void operator=(double a)
	{
		this=a.tofloat
	}
	
	void operator+=(float a)
	{
		this=this+a
	}

	void operator-=(float a)
	{
		this=this-a
	}

	void operator*=(float a)
	{
		this=this*a
	}
	
	void operator/=(float a)
	{
		this=this/a
	}

	friend float operator+(float a,float b) 
	{
		return (double(a)+double(b)).tofloat
	}

	friend float operator-(float a,float b)
	{
		return (double(a)-double(b)).tofloat
	}

	friend float operator*(float a,float b)
	{
		return (double(a)*double(b)).tofloat
	}

	friend float operator/(float a,float b)
	{
		return (double(a)/double(b)).tofloat
	}

	friend bool operator<(float a,float b)
	{
		return double(a)<double(b)
	}

	friend bool operator<=(float a,float b)
	{
		return a<b||a==b;
	}

	friend bool operator==(float a,float b)
	{
		cesb a,b
		mov s_ret,ebx
	}

	friend bool operator!=(float a,float b)
	{
		return !(a==b)
	}

	friend bool operator>(float a,float b)
	{
		return double(a)>double(b)
	}

	friend bool operator>=(float a,float b)
	{
		return a>b||a==b;
	}
	
	float abs()
	{
		if this<0.0
			return neg this
		return this
	}

	void clear()
	{
		mov esi,this
		mov [esi],0
	}

	bool empty()
	{
		return 0==*(&this).to<int*>
	}

	/*print()
	{
		rf.print rstr(this)
	}

	printl()
	{
		rf.printl rstr(this)
	}*/
}