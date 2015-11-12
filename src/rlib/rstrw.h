#pragma once

#include "rcode.h"

struct rstrw
{
	rbuf<ushort> m_buf;

	~rstrw()
	{
	}

	rstrw()
	{
	}

	rstrw(const rstrw& s)
	{
		m_buf.alloc_not_change(s.m_buf.m_max);
		m_buf.m_count=s.count();
		xf::memcpy(begin(),s.begin(),s.size());
	}

	rstrw(const char* p)
	{
		rstr tmp=rcode::utf8_to_utf16(rstr(p));//todo:
		set_size(tmp.size());
		xf::memcpy(begin(),tmp.begin(),tmp.size());
	}

	rstrw(const rstr& s)
	{
		rstr tmp=rcode::utf8_to_utf16(s);
		set_size(tmp.size());
		xf::memcpy(begin(),tmp.begin(),tmp.size());
	}

	rstrw(rstrw&& s)
	{
		m_buf.move(s.m_buf);
	}

	rstr torstr() const
	{
		return rcode::utf16_to_utf8(rstr((char*)begin(),size()));
	}

	int toint()
	{
		return this->torstr().toint();
	}

	ushort* cstrw()
	{
		this->push((ushort)0);
		this->pop();
		return begin();
	}

	wchar* cstrw_t()
	{
		return (wchar*)(cstrw());
	}

	void operator=(const rstrw& a)
	{
		m_buf=a.m_buf;
	}

	void operator=(rstrw&& a)
	{
		m_buf.free();
		m_buf.move(a.m_buf);
	}

	ushort& operator[](int num) const 
	{
		return m_buf[num];
	}

	friend rbool operator==(const rstrw& a,const rstrw& b)
	{
		return a.m_buf==b.m_buf;
	}

	friend rbool operator!=(const rstrw& a,const rstrw& b)
	{
		return a.m_buf!=b.m_buf;
	}

	friend rstrw operator+(const rstrw& a,const rstrw& b)
	{
		rstrw ret;
		ret.m_buf=a.m_buf+b.m_buf;
		return r_move(ret);
	}

	void operator+=(const rstrw& a)
	{
		m_buf+=a.m_buf;
	}

	void operator+=(rstrw&& a)
	{
		m_buf+=r_move(a.m_buf);
	}

	void operator+=(ushort ch)
	{
		m_buf+=ch;
	}

	friend rbool operator<(const rstrw& a,const rstrw& b)
	{
		for(int i=0;i<a.count()&&i<b.count();i++)
		{
			if(a[i]!=b[i])
			{
				return a[i]<b[i];
			}
		}
		return a.count()<b.count();
	}

	friend rbool operator>(const rstrw& a,const rstrw& b)
	{
		return b<a;
	}

	friend rbool operator<=(const rstrw& a,const rstrw& b)
	{
		return !(a>b);
	}

	friend rbool operator>=(const rstrw& a,const rstrw& b)
	{
		return !(a<b);
	}

	ushort* set_count(int count)
	{
		m_buf.realloc_n(count);
		return begin();
	}

	ushort* set_size(int size)
	{
		return set_count(size/2);
	}

	void set_w(const wchar* p)
	{
		int len=xf::strlenw(p);
		m_buf.realloc_n_not_change(rbuf<ushort>::extend_num(len));
		m_buf.m_count=len;
		xf::memcpy(m_buf.begin(),p,len*2);
	}

	void push(ushort ch)
	{
		m_buf.push(ch);
	}

	void push_front(ushort ch)
	{
		m_buf.push_front(ch);
	}

	ushort pop()
	{
		return m_buf.pop();
	}

	ushort pop_front()
	{
		return m_buf.pop_front();
	}

	int count() const 
	{
		return m_buf.count();
	}

	int size() const
	{
		return count()*2;
	}

	ushort* begin() const 
	{
		return m_buf.begin();
	}

	ushort* end() const 
	{
		return m_buf.end();
	}

	rbool empty() const 
	{
		return m_buf.empty();
	}

	void clear()
	{
		m_buf.clear();
	}

	static ushort* next(void* p)
	{
		return ((ushort*)p)+1;
	}

	static ushort* prev(void* p)
	{
		return ((ushort*)p)-1;
	}

	ushort* pos(int num) const
	{
		return begin()+num;
	}

	int pos(void* p) const
	{
		return int((ushort*)p-begin());
	}

	rstrw sub(int begin) const
	{
		return sub(begin,count());
	}

	rstrw sub(int begin,int end) const
	{
		rstrw ret;
		ret.m_buf=m_buf.sub(begin,end);
		return r_move(ret);
	}

	rbool erase(int begin,int end)
	{
		return m_buf.erase(begin,end);
	}

	rbool erase(int i)
	{
		return erase(i,i+1);
	}

	rbool insert(int pos,const rstrw& a)
	{
		return m_buf.insert(pos,a.m_buf);
	}

	ushort get(int i) const
	{
		if(i>=count())
		{
			return (ushort)0;
		}
		else
		{
			return m_buf[i];
		}
	}

	ushort get_top() const
	{
		if(count()>0)
		{
			return m_buf.m_p[count()-1];
		}
		return (ushort)0;
	}

	ushort get_bottom() const
	{
		if(count()>0)
		{
			return m_buf.m_p[0];
		}
		return (ushort)0;
	}

	ushort get_left() const
	{
		return get_bottom();
	}

	ushort get_right() const
	{
		return get_top();
	}

	rbool is_number() const
	{
		if(empty())
		{
			return false;
		}
		for(int i=0;i<count();i++)
		{
			if(!is_number(m_buf[i]))
			{
				return false;
			}
		}
		return true;
	}

	static rbool is_number(ushort ch)
	{
		return ch>=r_char('0')&&ch<=r_char('9');
	}

	int find(rstrw s,int begin=0) const
	{
		return r_find<rstrw>(*this,s,begin);
	}

	int find_last(ushort ch) const
	{
		return m_buf.find_last(ch);
	}
};
