#pragma once

#include "rnew.h"

template<typename T>
struct rbuf
{
	T* m_p;
	int m_count;
	int m_max;
	uchar m_block[16];
	//当分配小内存时，m_p指向m_block，无需new可提高效率

	~rbuf<T>()
	{
		free_x();
	}

	rbuf<T>()
	{
		init();
	}

	rbuf<T>(int num)
	{
		init();
		alloc(num);
	}

	rbuf<T>(int num,const T& a)
	{
		init();
		alloc(num);
		for(int i=0;i<num;i++)
		{
			m_p[i]=a;
		}
	}

	rbuf<T>(const rbuf<T>& a)
	{
		init();
		copy(a);
	}

	rbuf<T>(rbuf<T>&& a)
	{
		move(a);
	}

	void move(rbuf<T>& a)
	{
#ifndef _RS
		if(a.m_p==(T*)(a.m_block))
		{
			xf::memcpy(this,&a,r_size(a));
			m_p=(T*)m_block;
		}
		else
		{
			m_p=a.m_p;
			m_count=a.m_count;
			m_max=a.m_max;
		}
#else
		if(a.m_p==&a.m_block)
		{
			xf::memcpy(this,&a,r_size(a));
			m_p=&m_block;
		}
		else
		{
			m_p=a.m_p;
			m_count=a.m_count;
			m_max=a.m_max;
		}
#endif
		a.init();//如果move后对象不再使用可以只置空a.m_p
	}

	//如果复制指针一定要处理好等于号，不支持自己复制自己
	void operator=(const rbuf<T>& a)
	{
		if(a.m_p!=m_p)
		{
			copy(a);
		}
	}

	void operator=(rbuf<T>&& a)
	{
		free_x();
		move(a);
	}

	friend rbuf<T> operator+(const rbuf<T>& a,const rbuf<T>& b)
	{
		int total=a.count()+b.count();
		int max=r_max(a.m_max,b.m_max);
		rbuf<T> ret;
		if(total>max)
		{
			ret.alloc_not_change(extend_num(total));
		}
		else
		{
			ret.alloc_not_change(max);
		}
		int i;
		for(i=0;i<a.count();i++)
		{
			ret[i]=a[i];
		}
		for(i=0;i<b.count();i++)
		{
			ret[i+a.count()]=b[i];
		}
		ret.m_count=total;
		return r_move(ret);
	}

	void operator+=(const rbuf<T>& a)
	{
		int total=a.count()+count();
		if(total>m_max)
		{
			realloc_not_change(extend_num(total));
		}
		for(int i=count();i<total;i++)
		{
			m_p[i]=a[i-count()];
		}
		m_count=total;
	}

	void operator+=(rbuf<T>&& a)
	{
		int total=a.count()+count();
		if(total>m_max)
		{
			realloc_not_change(extend_num(total));
		}
		for(int i=count();i<total;i++)
		{
			m_p[i]=r_move(a[i-count()]);
		}
		m_count=total;
	}

	void operator+=(const T& a)
	{
		this->push(a);
	}

	void operator+=(T&& a)
	{
		this->push(a);
	}

	T& operator[](int num) const
	{
		return m_p[num];
	}

	friend rbool operator==(const rbuf<T>& a,const rbuf<T>& b)
	{
		if(a.count()!=b.count())
		{
			return false;
		}
		for(int i=0;i<a.count();++i)
		{
			if(a[i]!=b[i])
			{
				return false;
			}
		}
		return true;
	}

	friend rbool operator!=(const rbuf<T>& a,const rbuf<T>& b)
	{
		return !(a==b);
	}

	void init()
	{
		m_p=null;
		m_count=0;
		m_max=0;
	}

	static T* next(void* p)
	{
		return (T*)p+1;
	}

	static T* prev(void* p)
	{
		return (T*)p-1;
	}

	int size() const 
	{
		return m_count*r_size(T);
	}

	int count() const
	{
		return m_count;
	}

	T* begin() const 
	{
		return m_p;
	}

	T* end() const 
	{
		return m_p+count();
	}

	T* rbegin() const 
	{
		if(0==m_count)
		{
			return null;
		}
		else
		{
			return m_p+count()-1;
		}
	}

	T* rend() const 
	{
		if(0==m_count)
		{
			return null;
		}
		else
		{
			return m_p-1;
		}
	}

	rbool empty() const 
	{
		return 0==m_count;
	}

	void clear()
	{
		m_count=0;
	}

	static int extend_num(int num)
	{
		return r_cond(num<16,16,num*2);
	}
	
	//应判断m_count是否太大,否则在64位上m_count++有可能归零
	void push(const T& a)
	{
		if(m_count>=m_max)
		{
			realloc_not_change(extend_num(m_count));
		}
		m_p[count()]=a;
		m_count++;
	}

	void push(T&& a)
	{
		if(m_count>=m_max)
		{
			realloc_not_change(extend_num(m_count));
		}
		m_p[count()]=a;
		m_count++;
	}

	void push_move(const T& a)
	{
		this->push(r_move(a));
	}

	T pop()
	{
		m_count--;
		return m_p[count()];
	}

	T pop_front()
	{
		T ret=r_move(m_p[0]);
		erase(0);
		return r_move(ret);
	}

	//没必要增加push_front的move版本
	void push_front(const T& a)
	{
		insert(0,a);
	}

	T& top() const 
	{
		return m_p[count()-1];
	}

	T& bottom() const 
	{
		return m_p[0];
	}

	T get_top() const 
	{
		if(count()>0)
		{
			return m_p[count()-1];
		}
		return T();
	}

	T get_right() const 
	{
		return get_top();
	}

	T get_bottom() const 
	{
		if(count()>0)
		{
			return m_p[0];
		}
		return T();
	}

	T get_left() const 
	{
		return get_bottom();
	}

	rbool erase(int num)
	{
		if(num>=count()||num<0)
		{
			return false;
		}
		for(int i=num;i<count()-1;++i)
		{
			*(m_p+i)=r_move(*(m_p+i+1));
		}
		m_count--;
		return true;
	}

	rbool erase(int begin,int end)
	{
		if(begin<0||end>count()||begin>=end)
		{
			return false;
		}
		for(int i=0;i<count()-end;i++)//count()-(end-begin)-begin
		{
			m_p[i+begin]=r_move(m_p[end+i]);//删除的元素稍后析构
		}
		m_count-=end-begin;
		return true;
	}

	rbool insert(int pos,const T& a)
	{
		if(pos>count()||pos<0)
		{
			return false;
		}
		if(m_count>=m_max)
		{
			realloc_not_change(extend_num(m_count));
		}
		for(int i=count();i>pos;--i)
		{
			m_p[i]=r_move(m_p[i-1]);
		}
		m_p[pos]=a;
		m_count++;
		return true;
	}

	rbool insert(int pos,const rbuf<T>& a)
	{
		if(pos>count()||pos<0||0==a.count())
		{
			return false;
		}
		if(m_max<m_count+a.count())
		{
			realloc_not_change(a.count()+count());
		}
		for(int i=0;i<count()-pos;i++)
		{
			m_p[a.count()+count()-1-i]=r_move(m_p[count()-1-i]);
		}
		for(int i=0;i<a.count();i++)
		{
			m_p[pos+i]=a[i];
		}
		m_count+=a.count();
		return true;
	}

	T* v_new(int num) const
	{
#ifdef _RGPP
		return r_new<T>(num);//GPP模式不能使用此优化技巧
#endif
		if(num*r_size(T)<=16)
		{
#ifndef _RS
			return (T*)m_block;
#else
			return &m_block;
#endif
		}
		return r_new<T>(num);
	}

	void v_delete()
	{
#ifdef _RGPP
		r_delete<T>(m_p);
		return;
#endif
#ifndef _RS
		if(m_p!=(T*)m_block)
		{
			r_delete<T>(m_p);
		}
#else
		if(m_p!=&m_block)
		{
			r_delete<T>(m_p);
		}
#endif
	}

	//这里的重复代码可用宏或者模板简化
	void alloc(int num)
	{
		if(m_p!=null)
		{
			return;
		}
		if(num<=0)
		{
			init();
			return;
		}
		m_p=v_new(num);
		m_max=num;
		m_count=m_max;
	}

	void alloc_not_change(int num)
	{
		if(m_p!=null)
		{
			return;
		}
		if(num<=0)
		{
			init();
			return;
		}
		m_p=v_new(num);
		m_max=num;
	}

	void realloc(int num)
	{
		if(null==m_p)
		{
			alloc(num);
			return;
		}
		if(num<=0)
		{
			free();
			return;
		}
		realloc_not_change_in(num);
		m_count=m_max;
	}

	void realloc_not_change(int num)
	{
		if(null==m_p)
		{
			alloc_not_change(num);
			return;
		}
		if(num<=0)
		{
			free();
			return;
		}
		realloc_not_change_in(num);
	}

	void realloc_not_change_in(int num)
	{
		T* p=v_new(num);
		int copy_size=r_min(num,m_count);
		for(int i=0;i<copy_size;i++)
		{
			p[i]=r_move(m_p[i]);
		}
		v_delete();
		m_p=p;
		m_max=num;
	}
	
	//不复制原有的元素
	void realloc_n(int num)
	{
		if(null==m_p)
		{
			alloc(num);
			return;
		}
		if(num<=0)
		{
			free();
			return;
		}
		v_delete();
		m_p=v_new(num);
		m_max=num;
		m_count=m_max;
	}

	void realloc_n_not_change(int num)
	{
		if(null==m_p)
		{
			alloc_not_change(num);
			return;
		}
		if(num<=0)
		{
			free();
			return;
		}
		v_delete();
		m_p=v_new(num);
		m_max=num;
	}
	
	void free_x()
	{
		if(null!=m_p)//m_p为空时rbuf必须保证m_count和m_max也为空
		{
			v_delete();
		}
	}

	void free()
	{
		free_x();
		init();
	}

	void copy(const rbuf<T>& a)
	{
		if(m_max<a.count())
		{
			realloc_n_not_change(a.m_max);
		}
		for(int i=0;i<a.count();i++)
		{
			m_p[i]=a[i];
		}
		m_count=a.count();
	}

	T get(int i) const
	{
		if((uint)i>=(uint)(count()))//补码判断
		{
			return T();
		}
		else 
		{
			return m_p[i];
		}
	}

	rbuf<T> sub(int begin,int end) const 
	{
		if(end>count())
		{
			end=count();
		}
		if(begin<0)
		{
			begin=0;
		}
		int temp=end-begin;
		rbuf<T> ret(temp);//alloc会判断temp<0的情况，所以不需要判断begin>=end
		for(int i=0;i<temp;i++)
		{
			ret[i]=m_p[begin+i];
		}
		return r_move(ret);
	}

	rbuf<T> sub(int begin) const 
	{
		return sub(begin,count());
	}

	rbool exist(const T& a) const
	{
		for(int i=0;i<count();i++)
		{
			if(a==m_p[i])
			{
				return true;
			}
		}
		return false;
	}

	int find(const T& a,int begin=0) const
	{
		for(int i=begin;i<count();i++)
		{
			if(a==m_p[i])
			{
				return i;
			}
		}
		return count();
	}

	int find_last(const T& a) const
	{
		for(int i=count()-1;i>=0;i--)
		{
			if(a==m_p[i])
			{
				return i;
			}
		}
		return count();
	}

	void print() const 
	{
		for(int i=0;i<count();i++)
		{
			m_p[i].print();
		}
	}

	void printl() const 
	{
		for(int i=0;i<count();i++)
		{
			m_p[i].printl();
		}
	}
#ifdef _RS
	rstr join(rstr s) const
	{
		rstr ret
		for i=0;i<count;i++
			if i!=0
				ret+=s
			ret+=m_p[i].torstr
		return ret
	}
	
	rbuf<T> map(void* f) const
	{
		rbuf<T> ret
		for i=0;i<count;i++
			ret.push(T[f,m_p[i]])
		return ret
	}
	
	void parallel(int num,void* f)
	{
		rmutex* pmu=r_new<rmutex>()
		int* pcur=r_new<int>()
		rbuf<T>* pthis=this
		rbuf<int> arr_thr
		*pcur=0
		for i=0;i<num;i++
			arr_thr.push(xf.create_thr(lambda(void* param){
				for
					pmu->enter
					if *pcur>=pthis->count
						pmu->leave
						return
					index=*pcur
					(*pcur)++
					pmu->leave
					void[f,&(*pthis)[index]]
			}))
		for i in arr_thr
			xf.wait_thr(arr_thr[i])
			xf.CloseHandle(arr_thr[i])
		r_delete<int>(pcur)
		r_delete<rmutex>(pmu)
	}
#endif
};
