#pragma once

#include "rbuf.h"

//binary search
template<typename T>
int r_find_b(const rbuf<T>& v,const T& a)
{
	int left=0;
	int right=v.count()-1;
	int mid;
	while(left<=right)
	{
		mid=(left+right)/2;
		if(v[mid]<a)
		{
			left=mid+1;
		}
		elif(a<v[mid])
		{
			right=mid-1;
		}
		else
		{
			return mid;
		}
	}
	return v.count();
}

template<typename T>
rbool r_not_equal(const T& a,const T& b)
{
	return a<b||b<a;
}

template<typename T>
void r_swap(T& a,T& b)
{
	T temp(r_move(a));
	a=r_move(b);
	b=r_move(temp);
}

template<typename T>
void r_reverse(rbuf<T>& v,int begin=0,int end=0)
{
	if(end<=0)
	{
		end=v.count();
	}
	if(begin<0)
	{
		begin=0;
	}
	rbuf<T> temp=v.sub(begin,end);
	int i=end-1;
	int j=0;
	while(i>=begin)
	{
		v[i]=r_move(temp[j]);
		i--;
		j++;
	}
}

template<typename T>
void r_qsort_in(rbuf<T>& v,int left,int right)
{
	if(left<right)
	{
		T pivot=v[right];
		int i=left;
		for(int j=left;j<right;j++)
		{
			if(v[j]<=pivot)
			{
				r_swap<T>(v[i],v[j]);
				i++;
			}
		}
		r_swap<T>(v[i],v[right]);
		r_qsort_in<T>(v,left,i-1);
		r_qsort_in<T>(v,i+1,right);
	}
}

template<typename T>
void r_qsort(rbuf<T>& v,int begin=0,int end=0)
{
	if(end<=0)
	{
		end=v.count();
	}
	if(begin<0)
	{
		begin=0;
	}
	if(begin+1<end)
	{
		r_qsort_in<T>(v,begin,end-1);
	}
}

template<typename T>
int r_find_a(const rbuf<T>& v,T a,int start=0)
{
	for(int i=start;i<v.count();i++)
	{
		if(a==v[i])
		{
			return i;
		}
	}
	return v.count();
}

template<typename V>
int r_find(const V& v,const V& m,int start=0)
{
	if(m.empty()||start>=v.count())
	{
		return v.count();
	}
	for(int i=start;i<v.count()-m.count()+1;i++)
	{
		if(v[i]!=m[0])
		{
			continue;
		}
		int j;
		for(j=0;j<m.count();j++)
		{
			if(v[i+j]!=m[j])
			{
				break;
			}
		}
		if(j>=m.count())
		{
			return i;
		}
	}
	return v.count();
}

template<typename V>
rbuf<V> r_split(const V& v,const V& m,int start=0)
{
	rbuf<V> result;
	V temp;
	int i;
	while(start<v.count())
	{
		int pos=r_find<V>(v,m,start);
		if(pos>=v.count())
		{
			break;
		}
		temp.clear();
		for(i=start;i<pos;i++)
		{
			temp+=v[i];
		}
		if(!temp.empty())
		{
			result.push_move(temp);
		}
		start=pos+m.count();
	}
	temp.clear();
	for(i=start;i<v.count();i++)
	{
		temp+=v[i];
	}
	if(!temp.empty())
	{
		result.push_move(temp);
	}
	return r_move(result);
}

template<typename T>
rbuf<rbuf<T> > r_split_a(const rbuf<T>& v,const T& a,int start=0)
{
	return r_split<rbuf<T> >(v,rbuf<T>(1,a),start);
}

//允许空元素
template<typename V>
rbuf<V> r_split_e(const V& v,const V& m,int start=0)
{
	rbuf<V> result;
	V temp;
	int i;
	while(start<v.count())
	{
		int pos=r_find<V>(v,m,start);
		if(pos>=v.count())
		{
			break;
		}
		temp.clear();
		for(i=start;i<pos;i++)
		{
			temp+=v[i];
		}
		result.push_move(temp);
		start=pos+m.count();
	}
	temp.clear();
	for(i=start;i<v.count();i++)
	{
		temp+=v[i];
	}
	ifn(result.empty()&&temp.empty())//todo:
	{
		result.push_move(temp);
	}
	return r_move(result);
}

template<typename T>
rbuf<rbuf<T> > r_split_e_a(const rbuf<T>& v,const T& a,int start=0)
{
	return r_split_e<rbuf<T> >(v,rbuf<T>(1,a),start);
}
