#pragma once

#include "../rlib/rstrw.h"

//包含rstrw的xf
struct xfs
{
	static void* fopen(rstr name,rstr mode)
	{
#ifdef _WIN32
		return xf::_wfopen(rstrw(name).cstrw_t(),rstrw(mode).cstrw_t());
#endif
#ifdef _RS
		return xf::_wfopen(rstrw(name).cstrw_t(),rstrw(mode).cstrw_t());
#endif
#ifdef __GNUC__
		return xf::fopen(name.cstr_t(),mode.cstr_t());
#endif
	}

	static int remove(rstr name)
	{
#ifdef _WIN32
		return xf::_wremove(rstrw(name).cstrw_t());
#endif
#ifdef _RS
		return xf::_wremove(rstrw(name).cstrw_t());
#endif
#ifdef __GNUC__
		return xf::remove(name.cstr_t());
#endif
	}
};
