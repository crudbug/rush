#pragma once

#include "yfind.h"

struct ymatch
{
	//可构造转换传递
	static rbool is_construct_pass(const tsh& sh,const tsent& src,const rstr& dst)
	{
		tclass* ptci=yfind::class_search_t(sh,dst);
		if(ptci==null)
		{
			return false;
		}
		tfunc* ptfi=yfind::func_search(*ptci,ybase::get_tname(dst),
			ybase::get_tname(dst)+rsoptr(c_addr),
			ybase::get_tname(src.type)+rsoptr(c_addr));
		if(ptfi!=null)
		{
			return true;
		}
		return null!=yfind::func_search(*ptci,ybase::get_tname(dst),
			ybase::get_tname(dst)+rsoptr(c_addr),ybase::get_tname(src.type));
	}

	//可直接传递，返回对象可传递引用
	static rbool is_direct_pass(const tsh& sh,const tsent& src,const rstr& dst)
	{
		if(ybase::is_point(src.type)&&ybase::is_point(dst))
		{
			return true;
		}
		if(dst==rskey(c_rd32)&&yfind::get_type_size(sh,src.type)==4)
		{
			return true;
		}
		return ybase::get_tname(src.type)==ybase::get_tname(dst);
	}

	//是否直接传递或构造转换传递
	static rbool is_direct_construct_pass_m(const tsh& sh,const rbuf<tsent>& vsent,
		const rbuf<tdata>& vparam)
	{
		if(vsent.count()!=vparam.count())
		{
			return false;
		}
		for(int i=0;i<vsent.count();i++)
		{
			if(!is_direct_pass(sh,vsent[i],vparam[i].type)&&
				!is_construct_pass(sh,vsent[i],vparam[i].type))
			{
				return false;
			}
		}
		return true;
	}

	//查找直接传递或构造转换传递
	static tfunc* find_direct_construct_pass(const tsh& sh,const tclass& tci,const rstr& fname,
		const rbuf<tsent>& vsent)
	{
		for(tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			if(fname!=p->name)
			{
				continue;
			}
			if(is_direct_construct_pass_m(sh,vsent,p->param))
			{
				return p;
			}
		}
		return null;
	}

	//可完全传递，临时对象可传递引用，常量和返回对象不可传递引用
	static rbool is_full_pass(const tsh& sh,const tsent& src,const rstr& dst)
	{
		if(!is_direct_pass(sh,src,dst))
		{
			return false;
		}
		if(src.vword.count()==1&&
			(src.vword[0].is_const()||
			sh.m_key.is_asm_reg(src.vword[0].val))&&
			ybase::is_quote(dst))
		{
			return false;
		}
		if(src.vword.count()==7&&
			ybase::is_quote(dst)&&
			src.vword[1].val==rsoptr(c_addr)&&
			src.vword[0].val==rsoptr(c_mbk_l))
		{
			return false;
		}
		if(is_temp_var(sh,src))
		{
			return true;
		}
		return !ybase::is_quote(dst)||!is_ret_obj(sh,src.vword);
	}

	static rbool is_full_pass_m(const tsh& sh,const rbuf<tsent>& vsent,const rbuf<tdata>& vparam)
	{
		if(vsent.count()!=vparam.count())
		{
			return false;
		}
		for(int i=0;i<vsent.count();i++)
		{
			if(!is_full_pass(sh,vsent[i],vparam[i].type))
			{
				return false;
			}
		}
		return true;
	}

	static tfunc* find_full_pass(const tsh& sh,const tclass& tci,const rstr& fname,
		const rbuf<tsent>& vsent)
	{
		for(tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			if(fname!=p->name)
			{
				continue;
			}
			if(is_full_pass_m(sh,vsent,p->param))
			{
				return p;
			}
		}
		return null;
	}

	static rbool is_temp_var(const tsh& sh,const tsent& sent)
	{
		return (!ybase::is_quote(sent.type)&&
			sent.vword.count()>=3&&sent.type==sent.vword[2].val&&
			sent.vword[1].val==rskey(c_btemp));
	}

	//重载的友元属性和返回值类型必须相同
	static rbool is_ret_obj(const tsh& sh,const rbuf<tword>& v)
	{
		if(v.count()<3)
		{
			return false;
		}
		int start=0;
		while(start+2<v.count()&&
			v[start+1].val==rsoptr(c_dot)&&
			v[start].val==rsoptr(c_mbk_l))
		{
			start+=3;
		}
		if(v[start].val==rsoptr(c_mbk_l))
		{
			start++;
		}
		if(start+2>=v.count())
		{
			return false;
		}
		if(v[start].val==rskey(c_pcall))
		{
			return !ybase::is_quote(v[start+2].val);
		}
		tclass* ptci=yfind::class_search(sh,v[start].val);
		if(ptci==null)
		{
			return false;
		}
		tfunc* ptfi=yfind::func_search_dec(*ptci,v[start+2].val);
		if(ptfi==null)
		{
			return false;
		}
		return !ybase::is_quote(ptfi->retval.type);
	}

	static void insert_type(const tsh& sh,tsent& sent,const rstr& type)
	{
		rbuf<tword> result;
		result+=rsoptr(c_mbk_l);
		result+=rskey(c_btemp);
		result+=rsoptr(c_comma);
		result+=type;
		result+=rsoptr(c_comma);
		result+=rsoptr(c_mbk_l);
		result+=sent.vword;
		result+=rsoptr(c_mbk_r);
		result+=rsoptr(c_mbk_r);
		sent.vword=r_move(result);
		sent.type=type;
	}

	//先完全传递，找不到再尝试替换单词
	static tfunc* find_replace(const tsh& sh,const tclass& tci,const rstr& fname,
		rbuf<tsent>& vsent)
	{
		tfunc* ptfi=find_full_pass(sh,tci,fname,vsent);
		if(ptfi!=null)
		{
			return ptfi;
		}
		//可能找到多个，只取第一个
		ptfi=find_direct_construct_pass(sh,tci,fname,vsent);
		if(ptfi==null)
		{
			return null;
		}
		for(int i=0;i<vsent.count();i++)
		{
			//这里也可以用 if is_direct_construct_pass
			ifn(is_full_pass(sh,vsent[i],ptfi->param[i].type))
			{
				insert_type(sh,vsent[i],
					ybase::get_tname(ptfi->param[i].type));
			}
		}
		return ptfi;
	}
};
