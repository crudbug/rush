#pragma once

#ifndef _RS
#include "../xlib/xsock.h"
#endif
#include "../front/yclass.h"
#include "../front/yformat.h"
#include "zvm.h"
#include "zjit.h"
#include "zgpp.h"

//main函数进行参数解析
struct zmain
{
	static rbool compile(uchar* cont)
	{
		tsh sh;
		sh.m_main_cont=cont;
		rbuf<rstr> vparam=rf::get_param();
		if(vparam.count()<2)
		{
			return false;
		}
		sh.m_vdefine.insert_c(tmac(rstr("_RS")));
		if(vparam[1]=="-jit")
		{
			sh.m_mode=tsh::c_jit;
			sh.m_vdefine.insert_c(tmac(rstr("_RJIT")));
		}
		elif(vparam[1]=="-gpp")
		{
			sh.m_mode=tsh::c_gpp;
			sh.m_vdefine.insert_c(tmac(rstr("_RGPP")));
		}
		elif(vparam[1]=="-gpp64")
		{
			sh.m_point_size=8;
			sh.m_mode=tsh::c_gpp;
			sh.m_vdefine.insert_c(tmac(rstr("_RGPP")));
		}
		elif(vparam[1]=="-nasm")
		{
			sh.m_mode=tsh::c_nasm;
			sh.m_vdefine.insert_c(tmac(rstr("_RNASM")));
		}
		else
		{
			sh.m_vdefine.insert_c(tmac(rstr("_RVM")));
		}
		rstr name;
		for(int i=1;i<vparam.count();i++)
		{
			if(vparam[i].get_left()!=r_char('-'))
			{
				name=vparam[i];
				break;
			}
		}
		if(name.get_left()==r_char('\"'))
		{
			name=ybase::del_quote(name);
		}
		name=rcode::gbk_to_utf8(name);
		name=rdir::dir_std(name);
		name=ypre::get_abs_path(name);
		sh.m_main_file=name;
		ybase::init_path(sh.m_path);
		ybase::init_match(sh.m_match);
		ifn(ypre::process(sh))
		{
			rserror("pre process error");
			return false;
		}
		ifn(yformat::process(sh))
		{
			rserror("format error");
			return false;
		}
		ifn(yclass::process(sh))
		{
			rserror("extract class error");
			return false;
		}
		ifn(yclasstl::process(sh))
		{
			rserror("class template replace error");
			return false;
		}
		ifn(ymemb::process(sh))
		{
			rserror("ymemb error");
			return false;
		}
		if(sh.m_mode==tsh::c_vm)
		{
			tfunc* ptfi=yfind::func_search(*sh.m_main,"main");
			if(ptfi==null)
			{
				rf::printl("main not find");
				return false;
			}
			zjitf::init_addr_list(sh);
			ifn(zbin::process(sh,*ptfi))
			{
				return false;
			}
			ifn(zvm::run(sh))
			{
				return false;
			}
		}
		elif(sh.m_mode==tsh::c_jit)
		{
			zjitf::init_addr_list(sh);
			ifn(zjit::run(sh))
			{
				return false;
			}
		}
		elif(sh.m_mode==tsh::c_nasm)
		{
			ifn(znasm::process(sh))
			{
				return false;
			}
		}
		elif(sh.m_mode==tsh::c_gpp)
		{
			ifn(zgpp::process(sh))
			{
				return false;
			}
		}
		return true;
	}
};

//todo C++不支持mixin，只能这么写
rbool r_func_to_x86(tsh& sh,tfunc& tfi,tenv env)
{
	return zjit::func_to_x86(sh,tfi,env);
}
