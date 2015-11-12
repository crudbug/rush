#pragma once

#include "../xlib/xf.h"

template<typename T>
T* r_new(int count=1)
{
	T* p=(T*)(xf::malloc(r_size(int)+count*r_size(T)));
	if(p==null)
	{
		xf::print("memory insufficient");
		xf::exit(1);
		return null;
	}
	*(int*)p=count;
	p=(T*)((uchar*)p+r_size(int));
	T* start=p;
	T* end=start+count;
	for(;p<end;p++)
	{
#ifndef _RS
		new(p)T();//((T*)p)->T::T();
#else
		p->T();
#endif
	}
	return start;
}

template<typename T>
T* r_new_n(int count=1)
{
	T* p=(T*)(xf::malloc(r_size(int)+count*r_size(T)));
	if(p==null)
	{
		xf::print("memory insufficient");
		xf::exit(1);
		return null;
	}
	*(int*)p=count;
	return (T*)((uchar*)p+r_size(int));
}

template<typename T>
void r_delete(T* p)
{
	uchar* start=(uchar*)p-r_size(int);
	T* end=p+*(int*)start;
	for(;p<end;p++)
	{
#ifndef _RS
		p->~T();
#else
		T.~T(*p);
#endif
	}
	xf::free(start);
}

template<typename T>
void r_delete_n(T* p)//这里模板参数没用
{
	xf::free((uchar*)p-r_size(int));
}

//模板特化放在头文件里只能包含一次
//如果注释掉这4个特化int、char等内置类型会初始化为0，效率略低
template<>
uchar* r_new<uchar>(int count)
{
	return r_new_n<uchar>(count);
}

template<>
void r_delete<uchar>(uchar* p)
{
	r_delete_n<uchar>(p);
}

template<>
ushort* r_new<ushort>(int count)
{
	return r_new_n<ushort>(count);
}

template<>
void r_delete<ushort>(ushort* p)
{
	r_delete_n<ushort>(p);
}
