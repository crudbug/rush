#pragma once

#include "ymemb.h"
#include "yclasstl.h"

//函数模板处理
struct yfunctl
{
	static rbool ftl_replace(tsh& sh,tclass& tci,rbuf<tword>& v,tclass* ptci)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rsoptr(c_tbk_l))
			{
				continue;
			}
			int left=i;
			rstr name=v.get(left-1).val;
			tclass* cur_ptci=ptci;
			if(cur_ptci==null)
			{
				if(v.get(left-2).val==rsoptr(c_dot))
				{
					cur_ptci=yfind::class_search(sh,v.get(left-3).val);
					if(cur_ptci==null)
					{
						continue;
					}	
				}
				else
				{
					cur_ptci=&tci;
				}
			}
			tfunc* ptfi=yfind::ftl_search(*cur_ptci,name);
			if(ptfi==null)
			{
				cur_ptci=sh.m_main;
				ptfi=yfind::ftl_search(*cur_ptci,name);
			}
			if(null==ptfi)
			{
				continue;
			}
			int right=ybase::find_symm_tbk(sh,v,left);
			if(right>=v.count())
			{
				rserror(v[left-1],"func template miss >");
				return false;
			}
			rbuf<rbuf<tword> > vparam=ybase::comma_split<tword>(
				sh,v.sub(left+1,right));
			if(vparam.count()!=ptfi->vtl.count())
			{
				rserror(v[left-1],"func template not match");
				return false;
			}
			for(int j=left;j<=right;j++)
			{
				v[left-1].val+=v[j].val;
				v[j].val.clear();
			}
			//模板函数不可能重载，因此只要判断一下函数名是否相同即可
			if(yfind::func_search(*cur_ptci,v[left-1].val)!=null)
			{
				continue;
			}
			tfunc item=*ptfi;
			item.vtl.free();
			yclasstl::vtl_replace(item.vword,ptfi->vtl,vparam);
			if(!f_type_replace(sh,item.vword))
			{
				return false;
			}
			if(!combine_ftl_name(sh,item.vword,ptfi->name))
			{
				return false;
			}
			if(!ymemb::func_add(sh,*cur_ptci,item.vword,false))
			{
				return false;
			}
			i=right;
		}
		ybase::arrange(v);
		return true;
	}
	
	//处理函数模板中又出现新类型的情况
	static rbool f_type_replace(tsh& sh,rbuf<tword>& v)
	{
		int cur=sh.m_class.count();
		if(!yclasstl::type_replace(sh,v))
		{
			return false;
		}
		if(sh.m_class.count()>cur)
		{
			for(tclass* p=sh.m_class.begin();
				p!=sh.m_class.end();p=sh.m_class.next(p))
			{
				if(!f_proc_class(sh,*p))
				{
					return false;
				}
			}
		}
		return true;
	}
	
	static rbool f_proc_class(tsh& sh,tclass& tci)
	{
		ifn(ymemb::a_class(sh,tci))
		{
			return false;
		}
		ifn(ymemb::recursion_get_size(sh,tci))
		{
			return false;
		}
		ymemb::obtain_size_func(sh,tci);
		return true;
	}

	static rbool combine_ftl_name(const tsh& sh,rbuf<tword>& v,const rstr& name)
	{
		for(int i=1;i<v.count();i++)
		{
			if(v[i]!=rsoptr(c_tbk_l))
			{
				continue;
			}
			if(name!=v[i-1].val)
			{
				continue;
			}
			int left=i;
			int right=ybase::find_symm_tbk(sh,v,left);
			if(right>=v.count())
			{
				rserror(v[left-1],"miss >");
				return false;
			}
			for(int j=left;j<=right;j++)
			{
				v[left-1].val+=v[j].val;
				v[j].val.clear();
			}
			i=right;
		}
		ybase::arrange(v);
		return true;
	}
};
