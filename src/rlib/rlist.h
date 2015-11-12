#pragma once

#include "ralgo.h"

template<typename T>
struct rlist_i
{
	T val;
	rlist_i<T>* prev;
	rlist_i<T>* next;
};

template<typename T>
struct rlist
{
	rlist_i<T>* m_front;
	rlist_i<T>* m_rear;
	int m_count;

	~rlist<T>()
	{
		clear();
	}

	rlist<T>()
	{
		init();
	}

	rlist<T>(rlist<T>& a)
	{
		init();
		for(T* p=a.begin();p!=a.end();p=a.next(p))
		{
			this->push(*p);
		}
	}

	void operator=(rlist<T>& a)
	{
		clear();
		for(T* p=a.begin();p!=a.end();p=a.next(p))
		{
			this->push(*p);
		}
	}

	void init()
	{
		m_front=null;
		m_rear=null;
		m_count=0;
	}

	void clear()
	{
		while(m_front!=null)
		{
			m_rear=m_front->next;
			r_delete<rlist_i<T> >(m_front);
			m_front=m_rear;
		}
		init();
	}

	rbool empty() const
	{
		return m_count==0;
	}

	int count() const
	{
		return m_count;
	}

	T& top() const
	{
		return *(T*)m_rear;
	}

	void push(const T& a)
	{
		rlist_i<T>* p=r_new<rlist_i<T> >();
		p->val=a;
		p->next=null;
		p->prev=m_rear;
		if(empty())
		{
			m_front=p;
			m_rear=p;
		}
		else
		{
			m_rear->next=p;
			m_rear=p;
		}
		m_count++;
	}

	T pop()
	{
		m_count--;
		rlist_i<T>* p=m_rear;
		T ret=p->val;
		m_rear=p->prev;
		if(m_rear!=null)
		{
			m_rear->next=null;
		}
		if(empty())
		{
			m_front=null;
		}
		r_delete<rlist_i<T> >(p);
		return ret;
	}

	void push_front(const T& a)
	{
		rlist_i<T>* p=r_new<rlist_i<T> >();
		p->val=a;
		p->next=m_front;
		p->prev=null;
		if(empty())
		{
			m_front=p;
			m_rear=p;
		}
		else
		{
			m_front->prev=p;
			m_front=p;
		}
		m_count++;
	}

	T pop_front()
	{
		m_count--;
		rlist_i<T>* p=m_front;
		T ret=p->val;
		m_front=p->next;
		if(m_front!=null)
		{
			m_front->prev=null;
		}
		if(empty())
		{
			m_rear=null;
		}
		r_delete<rlist_i<T> >(p);
		return ret;
	}
	
	//前插
	void insert(T* pos,const T& a)
	{
		if(pos==null)
		{
			push(a);
			return;
		}
		rlist_i<T>* cur=(rlist_i<T>*)pos;
		rlist_i<T>* p=r_new<rlist_i<T> >();
		p->val=a;
		p->next=cur;
		p->prev=cur->prev;
		if(cur->prev!=null)
		{
			cur->prev->next=p;
		}
		cur->prev=p;
		if(p->prev==null)
		{
			m_front=p;
		}
		if(p->next==null)
		{
			m_rear=p;
		}
		m_count++;
	}

	void insert(int pos,const T& a)
	{
		insert(index(pos),a);
	}

	void erase(T* pos)
	{
		if(pos==null)
		{
			return;
		}
		rlist_i<T>* cur=(rlist_i<T>*)pos;
		if(cur->prev==null)
		{
			m_front=cur->next;
		}
		if(cur->next==null)
		{
			m_rear=cur->prev;
		}
		if(cur->prev!=null)
		{
			cur->prev->next=cur->next;
		}
		if(cur->next!=null)
		{
			cur->next->prev=cur->prev;
		}
		r_delete<rlist_i<T> >(cur);
		m_count--;
	}

	void erase(int pos)
	{
		erase(index(pos));
	}

	T* index(int n) const
	{
		rlist_i<T>* p=m_front;
		for(;n;n--)
		{
			if(p==null)
			{
				return null;
			}
			p=p->next;
		}
		return (T*)p;
	}

	T& operator[](int n) const
	{
		T* p=index(n);
		return *p;
	}

	T* begin() const
	{
		return (T*)m_front;
	}

	static T* end()
	{
		return null;
	}

	T* next(void* p) const
	{
		return (T*)(((rlist_i<T>*)p)->next);
	}

	T* prev(void* p) const
	{
		return (T*)(((rlist_i<T>*)p)->prev);
	}
};
