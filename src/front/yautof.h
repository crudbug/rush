#pragma once

#include "yfind.h"

//自动增加类成员函数
struct yautof
{
	static rbool auto_add_func(const tsh& sh,tclass& tci)
	{
		if(!tconf::c_auto_addfunc)
		{
			return true;
		}
		if(ybase::is_basic_type(sh,tci.name)||
			tci.is_friend)
		{
			return true;
		}
		rbool added=false;
		tpos pos;
		pos.line=1;
		pos.file=null;
		if(!tci.vfunc.empty())
		{
			pos=tci.vfunc.begin()->pos;
		}
		if(yfind::destruct_search(tci)==null)
		{
			tfunc tfi;
			tfi.ptci=&tci;
			tfi.pos=pos;
			tfi.name=rsoptr(c_destruct)+tci.name;

			tfi.param.push(tdata(tci.name+rsoptr(c_addr),rskey(c_this)));
			tfi.retval=tdata(rskey(c_void),rstr());

			tfi.name_dec=tfi.get_dec();
			tci.vfunc.insert(tfi);
			added=true;
		}
		if(yfind::emptystruct_search(tci)==null)
		{
			tfunc tfi;
			tfi.ptci=&tci;
			tfi.pos=pos;

			tfi.name=tci.name;

			tfi.param.push(tdata(tci.name+rsoptr(c_addr),rskey(c_this)));
			tfi.retval=tdata(rskey(c_void),rstr());

			tfi.name_dec=tfi.get_dec();
			tci.vfunc.insert(tfi);
			added=true;
		}
		if(yfind::copystruct_search(tci)==null)
		{
			tfunc tfi;
			tfi.ptci=&tci;
			tfi.pos=pos;

			tfi.name=tci.name;

			tfi.param.push(tdata(tci.name+rsoptr(c_addr),rskey(c_this)));
			tfi.param.push(tdata(tci.name+rsoptr(c_addr),rskey(c_rvara)));
			tfi.retval=tdata(rskey(c_void),rstr());

			add_copy_sent(sh,tfi);
			tfi.name_dec=tfi.get_dec();
			tci.vfunc.insert(tfi);
			added=true;
		}
		if(add_copystruct_func(sh,tci))
		{
			added=true;
		}
		if(yfind::func_search(tci,rsoptr(c_equal),tci.name+rsoptr(c_addr),
			tci.name+rsoptr(c_addr))==null)
		{
			tfunc tfi;
			tfi.ptci=&tci;
			tfi.pos=pos;

			tfi.name=rsoptr(c_equal);

			tfi.param.push(tdata(tci.name+rsoptr(c_addr),rskey(c_this)));
			tfi.param.push(tdata(tci.name+rsoptr(c_addr),rskey(c_rvara)));
			tfi.retval=tdata(rskey(c_void),rstr());

			add_copy_sent(sh,tfi);
			tfi.name_dec=tfi.get_dec();
			tci.vfunc.insert(tfi);
			added=true;
		}
		if(yfind::func_search(tci,rsoptr(c_equal),
			tci.name+rsoptr(c_addr),tci.name)==null)
		{
			tfunc tfi=*yfind::func_search(
				tci,rsoptr(c_equal),tci.name+rsoptr(c_addr),
				tci.name+rsoptr(c_addr));
			tfi.ptci=&tci;
			tfi.param[1].type=tci.name;
			tfi.name_dec=tfi.get_dec();
			tci.vfunc.insert(tfi);
			added=true;
		}
		if(yfind::func_search(tci,rsoptr(c_addr))==null)
		{
			tfunc tfi;
			tfi.ptci=&tci;
			tfi.pos=pos;

			tfi.name=rsoptr(c_addr);

			tfi.param.push(tdata(tci.name+rsoptr(c_addr),rskey(c_this)));
			tfi.retval=tdata("rp<"+tci.name+">",rskey(c_s_ret));

			tword twi;
			twi.pos=tfi.pos;
			if(sh.m_mode==tsh::c_gpp)
			{
				//todo:
				tfi.is_final=true;
				ybase::push_twi(tfi.vword,twi,"return");
				ybase::push_twi(tfi.vword,twi,"&");
				ybase::push_twi(tfi.vword,twi,rskey(c_this));
				ybase::push_twi(tfi.vword,twi,";");
			}
			else
			{
				ybase::push_twi(tfi.vword,twi,rskey(c_mov));
				ybase::push_twi(tfi.vword,twi,rskey(c_s_ret));
				ybase::push_twi(tfi.vword,twi,rsoptr(c_comma));
				ybase::push_twi(tfi.vword,twi,rskey(c_this));
			}
			tfi.name_dec=tfi.get_dec();
			if(yfind::is_class(sh,tfi.retval.type))
			{
				tci.vfunc.insert(tfi);
			}
			added=true;
		}
		return true;
	}

	static rbool add_copystruct_func(const tsh& sh,tclass& tci)
	{
		rbuf<tfunc> temp;
		for(tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			if(tci.name!=p->name)
			{
				continue;
			}
			if(p->param.count()<2)
			{
				continue;
			}
			int i;
			for(i=0;i<p->param.count();i++)
			{
				ifn(ybase::is_quote(p->param[i].type))
				{
					break;
				}
			}
			if(i<p->param.count())
			{
				continue;
			}
			tfunc* q;
			for(q=tci.vfunc.begin();q!=tci.vfunc.end();q=tci.vfunc.next(q))
			{
				if(q->name!=p->name||q->param.count()!=p->param.count())
				{
					continue;
				}
				ifn(ybase::is_quote(q->param.get(0).type))
				{
					continue;
				}
				int j;
				for(j=1;j<q->param.count();j++)
				{
					if(ybase::is_quote(q->param[j].type))
					{
						break;
					}
					if(ybase::get_tname(p->param[j].type)!=q->param[j].type)
					{
						break;
					}
				}
				if(j>=q->param.count())
				{
					break;
				}
			}
			if(q!=tci.vfunc.end())
			{
				continue;
			}
			tfunc tfi=*p;
			tfi.ptci=&tci;
			for(int i=1;i<tfi.param.count();i++)
			{
				tfi.param[i].type=ybase::get_tname(tfi.param[i].type);
			}
			tfi.name_dec=tfi.get_dec();
			temp.push(tfi);
		}
		for(int k=0;k<temp.count();k++)
		{
			tci.vfunc.insert(temp[k]);
		}
		return !temp.empty();
	}

	static void add_copy_sent(const tsh& sh,tfunc& tfi)
	{
		tclass& tci=*tfi.ptci;
		tword twi;
		twi.pos=tfi.pos;
		for(int i=0;i<tci.vdata.count();i++)
		{
			if(ybase::is_basic_type(sh,tci.vdata[i].type)||
				ybase::is_quote(tci.vdata[i].type))
			{
				continue;
			}
			ybase::push_twi(tfi.vword,twi,rskey(c_this));
			ybase::push_twi(tfi.vword,twi,rsoptr(c_dot));
			ybase::push_twi(tfi.vword,twi,tci.vdata[i].name);
			ybase::push_twi(tfi.vword,twi,rsoptr(c_equal));
			ybase::push_twi(tfi.vword,twi,rskey(c_rvara));
			ybase::push_twi(tfi.vword,twi,rsoptr(c_dot));
			ybase::push_twi(tfi.vword,twi,tci.vdata[i].name);
			ybase::push_twi(tfi.vword,twi,rsoptr(c_semi));
		}
	}
};
