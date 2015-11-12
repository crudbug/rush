#pragma once

#include "../xlib/xf.h"

struct rmutex
{
	R_CRITICAL_SECTION m_mutex;

	rmutex()
	{
		xf::InitializeCriticalSection(&m_mutex);
	}

	~rmutex()
	{
		xf::DeleteCriticalSection(&m_mutex);
	}

	void enter()
	{
		xf::EnterCriticalSection(&m_mutex);
	}

	void leave()
	{
		xf::LeaveCriticalSection(&m_mutex);
	}
};

//用于整个函数的局部临时mutex
struct rmutex_t
{
	rmutex* m_p;

	rmutex_t()
	{
		m_p=null;
	}

	rmutex_t(rmutex& m)
	{
		m_p=&m;
		m_p->enter();
	}

	~rmutex_t()
	{
		if(m_p!=null)
		{
			m_p->leave();
		}
	}
};
