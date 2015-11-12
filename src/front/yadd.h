#pragma once

#include "yfind.h"

//增加局部变量和成员变量构造及析构
struct yadd
{
	static void add_local_and_memb(const tsh& sh,tfunc& tfi)
	{
		tclass& tci=*tfi.ptci;
		//增加局部变量构造和析构
		//注意增加的这些语句type为空
		tsent temp=add_local_init(sh,tfi);
		if(!temp.empty())
		{
			tfi.vsent.push_front(temp);
		}
		temp=add_local_free(sh,tfi);
		if(!temp.empty())
		{
			tfi.vsent.push(temp);
		}
		ybase::part_vsent(tfi);
		if(tconf::c_auto_addinit)
		{
			if(tci.name==rskey(c_main)&&tfi.name==rskey(c_main))
			{
				add_member_struct(sh,tfi);	
				add_member_destruct(sh,tfi);//全局变量在main中构造和析构
			}
			elif(tfi.name==tci.name&&
				tfi.retval.type==rskey(c_void)&&
				!tfi.is_friend)
			{
				add_member_struct(sh,tfi);	
			}
			elif(yfind::is_destruct(sh,tfi))
			{
				add_member_destruct(sh,tfi);
			}
		}
	}

	static tsent add_local_free(const tsh& sh,tfunc& tfi)
	{
		tsent sent;
		sent.pos=tfi.pos;
		for(int i=0;i<tfi.param.count();i++)
		{
			add_destructor_func(sh,*tfi.ptci,tfi.param[i],sent.vword,false);
		}
		for(int i=0;i<tfi.local.count();i++)
		{
			ifn(is_temp_var(sh,tfi.local[i].name))
			{
				add_destructor_func(sh,*tfi.ptci,tfi.local[i],sent.vword,false);
			}
		}
		return r_move(sent);
	}

	static tsent add_local_init(const tsh& sh,tfunc& tfi)
	{
		tsent sent;
		sent.pos=tfi.pos;
		for(int i=0;i<tfi.local.count();i++)
		{
			if(!is_temp_var(sh,tfi.local[i].name)&&tfi.local[i].off!=-1)
			{
				add_structor_func(sh,tfi.local[i],sent.vword);
			}
		}
		return r_move(sent);
	}

	static void add_structor_func(const tsh& sh,const tdata& tdi,rbuf<tword>& v)
	{
		if(ybase::is_quote(tdi.type))
		{
			return;
		}
		if(tconf::c_op_empty_func&&yfind::is_empty_struct_type(sh,tdi.type))
		{
			return;
		}
		v+=rsoptr(c_mbk_l);
		v+=tdi.type;
		v+=rsoptr(c_comma);
		v+=tdi.type;
		v+=rsoptr(c_comma);
		v+=rsoptr(c_mbk_l);
		v+=tdi.name;
		v+=rsoptr(c_mbk_r);
		v+=rsoptr(c_mbk_r);
		v+=rsoptr(c_semi);
	}

	static void add_destructor_func(const tsh& sh,const tclass& tci,const tdata& tdi,
		rbuf<tword>& v,bool is_memb)
	{
		if(ybase::is_quote(tdi.type))
		{
			return;
		}
		if(tconf::c_op_empty_func&&yfind::is_empty_struct_type(sh,tdi.type))
		{
			return;
		}
		v+=rsoptr(c_mbk_l);
		v+=tdi.type;
		v+=rsoptr(c_comma);
		v+=rsoptr(c_destruct)+tdi.type;
		v+=rsoptr(c_comma);
		v+=rsoptr(c_mbk_l);
		if(is_memb)
		{
			v+=rsoptr(c_mbk_l);
			v+=rsoptr(c_dot);
			v+=rsoptr(c_comma);
			if(tci.name==rskey(c_main))
			{
				v+=rskey(c_pmain);
			}
			else
			{
				v+=rskey(c_this);
			}
			v+=rsoptr(c_comma);
			v+=tdi.name;
			v+=rsoptr(c_mbk_r);
		}
		else
		{
			v+=tdi.name;
		}
		v+=rsoptr(c_mbk_r);
		v+=rsoptr(c_mbk_r);
		v+=rsoptr(c_semi);
	}

	static void add_structor_func_m(const tsh& sh,const tclass& tci,const tdata& tdi,rbuf<tword>& v)
	{
		if(ybase::is_quote(tdi.type))
		{
			return;
		}
		if(tdi.param.empty()&&tconf::c_op_empty_func&&
			yfind::is_empty_struct_type(sh,tdi.type))
		{
			return;
		}
		v.push(tword(tdi.type));
		v.push(tword(rsoptr(c_dot)));
		v.push(tword(tdi.type));
		v.push(tword(rsoptr(c_sbk_l)));
		if(tci.name==rskey(c_main))
		{
			v+=tword(rskey(c_pmain));
		}
		else
		{
			v+=tword(rskey(c_this));
		}
		v+=tword(rsoptr(c_dot));
		v.push(tword(tdi.name));
		if(!tdi.param.empty())
		{
			v.push(tword(rsoptr(c_comma)));
			v+=tdi.param;
		}
		v.push(tword(rsoptr(c_sbk_r)));
		v.push(tword(rsoptr(c_semi)));
	}

	static void add_member_destruct(const tsh& sh,tfunc& tfi)
	{
		tclass& tci=*tfi.ptci;
		tsent sent;
		sent.pos=tfi.pos;
		sent.type=rskey(c_void);
		for(int i=tci.vdata.count()-1;i>=0;i--)
		{
			if(ybase::is_quote(tci.vdata[i].type))
			{
				continue;
			}
			tclass* ptci=yfind::class_search(sh,tci.vdata[i].type);
			if(null==ptci)
			{
				continue;
			}
			if(yfind::destruct_search(*ptci)==null)
			{
				continue;
			}
			add_destructor_func(sh,tci,tci.vdata[i],sent.vword,true);
		}
		if(!sent.vword.empty())
		{
			tfi.vsent.push_move(sent);
			ybase::part_vsent(tfi.vsent);
		}
	}

	static void add_member_struct(const tsh& sh,tfunc& tfi)
	{
		tclass& tci=*tfi.ptci;
		tsent sent;
		sent.type=rskey(c_void);
		for(int i=0;i<tci.vdata.count();i++)
		{
			if(ybase::is_quote(tci.vdata[i].type))
			{
				continue;
			}
			tclass* ptci=yfind::class_search(sh,tci.vdata[i].type);
			if(null==ptci)
			{
				continue;
			}
			if(yfind::emptystruct_search(*ptci)==null)
			{
				continue;
			}
			add_structor_func_m(sh,tci,tci.vdata[i],sent.vword);
		}
		if(sent.vword.empty())
		{
			return;
		}
		sent.pos=tfi.pos;
		tfi.vsent.push_front(r_move(sent));
		ybase::part_vsent(tfi.vsent);
	}

	static rbool is_temp_var(const tsh& sh,const rstr& name)
	{
		return name.sub(0,rskey(c_temp).count())==rskey(c_temp);
	}
};
