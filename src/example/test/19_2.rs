/*
类成员函数使用可变参数
*/

void main()
{
	A a
	a.push[1,99,3]
	a.m_arr.count.printl
	a.push[12,98]
	a.m_arr.count.printl
}

class A
{
	rbuf<int> m_arr

#ifndef _RGPP
	void push(int count)
	{
		//sub esp,sizeof(s_local)
		//push ebp
		//mov ebp,esp
		//变参函数的参数和局部变量需要手动析构，
		//这个例子使用的局部变量都是int，因此不需要析构

		int* p=&count+1
		for(i=0;i<count;i++)
		{
			m_arr.push(*p)
			p++
		}

		mov ecx,4
		imul ecx,count
		add ecx,8//注意成员函数多一个this引用
		pop ebp
		add esp,sizeof(s_local)
		mov eax,[esp]
		_reti(eax,ecx)

		//这里有编译器自动增加的语句
	}
#else
	void push(int count)
	{
		int* p=&count+1
		for(i=0;i<count;i++)
		{
			m_arr.push(*p)
			p++
		}
	}
#endif
}