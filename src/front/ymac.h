#pragma once

#include "yfind.h"
#include "ysuper.h"

//宏替换类
struct ymac
{
	static rbool func_mac_replace(const tsh& sh,rbuf<tsent>& vsent)
	{
		for(int i=0;i<vsent.count();++i)
		{
			ifn(func_mac_replace(sh,vsent[i].vword))
			{
				rserror(vsent[i],"func mac error");
				return false;
			}
		}
		ybase::part_vsent(vsent);
		return true;
	}

	static rbool func_mac_replace(const tsh& sh,rbuf<tword>& v)
	{
		if(v.get(0)!=rsoptr(c_sharp))
		{
			return true;
		}
		int right=v.find_last(tword(rsoptr(c_sbk_r)));
		if(right>=v.count())
		{
			return false;
		}
		int left=ybase::find_symm_word_rev(v,rsoptr(c_sbk_l),rsoptr(c_sbk_r),
			0,right+1);
		if(left>=v.count())
		{
			return false;
		}
		rbuf<rbuf<tword> > temp;
		temp=ybase::comma_split_e(sh,v.sub(left+1,right));
		rbuf<tword> head=v.sub(1,left+1);
		rbuf<tword> result;
		if(temp.count()==1&&yfind::is_class(sh,temp[0].get_top().val))
		{
			tclass* ptci=yfind::class_search(sh,temp[0].get_top().val);
			if(ptci==null)
			{
				return false;
			}
			if(temp[0].count()==1)
			{
				for(int i=0;i<ptci->vdata.count();i++)
				{
					result+=head;
					result+=ptci->vdata[i].name;
					result+=tword(rsoptr(c_sbk_r));
					result+=tword(rsoptr(c_semi));
					//todo 单词没有行号
				}
			}
			else
			{
				for(tfunc* p=ptci->vfunc.begin();p!=ptci->vfunc.end();
					p=ptci->vfunc.next(p))
				{
					result+=head;
					result+=tword(rsoptr(c_addr));
					result+=ptci->name;
					result+=tword(rsoptr(c_dot));
					result+=p->name;
					result+=tword(rsoptr(c_sbk_l));
					for(int j=0;j<p->param.count();j++)
					{
						if(j!=0)
						{
							result+=tword(rsoptr(c_comma));
						}
						result+=p->param[j].type;
					}
					result+=tword(rsoptr(c_sbk_r));
					result+=tword(rsoptr(c_sbk_r));
					result+=tword(rsoptr(c_semi));
				}
			}
		}
		else
		{
			for(int i=0;i<temp.count();i++)
			{
				result+=head;
				result+=temp[i];
				result+=tword(rsoptr(c_sbk_r));
				result+=tword(rsoptr(c_semi));
			}
		}
		if(result.empty())
		{
			return false;
		}
		v=r_move(result);
		return true;
	}

	static rbool replace(const tsh& sh,const tclass& tci,rbuf<tword>& v)
	{
		for(int i=0;i<c_rs_deep;i++)
		{
			ifn(replace_v(sh,tci,v))
			{
				return false;
			}
			ifn(ybase::arrange(v))
			{
				return true;
			}
		}
		return false;
	}

	static rbool replace_v(const tsh& sh,const tclass& tci,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			const tclass* ptci=&tci;
			int right=i;
			tmac item;
			item.name=v[i].val;
			if(ptci->vmac.exist(item))
			{
				ifn(i>1&&v.get(i-1).val==rsoptr(c_dot))
				{
					if(!replace_param(sh,v,i,
						*ptci->vmac.find(item),right))
					{
						return false;
					}
					i=right;
					continue;
				}
			}
			ptci=sh.m_main;
			if(ptci->vmac.exist(item))
			{
				ifn(i>1&&v.get(i-1).val==rsoptr(c_dot))
				{
					if(!replace_param(sh,v,i,
						*ptci->vmac.find(item),right))
					{
						return false;
					}
					i=right;
					continue;
				}
			}
			if(i==0||v[i].val!=rsoptr(c_dot)||i==v.count()-1)
			{
				continue;
			}
			if(i>=2&&v[i-2].val==rsoptr(c_dot))
			{
				continue;
			}
			ptci=yfind::class_search(sh,v[i-1].val);
			if(null==ptci)
			{
				continue;
			}
			i++;
			item.name=v.get(i).val;
			if(ptci->vmac.exist(item))
			{
				if(!replace_param(sh,v,i,*ptci->vmac.find(item),right))
				{
					return false;
				}
				v[i-1].clear();
				v[i-2].clear();
				i=right;
			}
		}
		return true;
	}

	static rbool replace_param(const tsh& sh,rbuf<tword>& v,int& i,
		tmac& mitem,int& right)
	{
		if(mitem.is_super)
		{
			return ysuper::replace_item(sh,v,right,mitem);
		}
		else
		{
			return replace_param_common(sh,v,i,mitem,right);
		}
	}

	static rbool replace_param_common(const tsh& sh,rbuf<tword>& v,int& i,
		tmac& mitem,int& right)
	{
		int left=i+1;
		rbuf<rbuf<rstr> > param;
		if(v.get(left).val==rsoptr(c_sbk_l))
		{
			right=ybase::find_symm_sbk(sh,v,left);
			if(right>=v.count())
			{
				rserror(v.get(i),"miss )");
				return false;
			}
			rbuf<rbuf<tword> > temp;
			temp=ybase::comma_split<tword>(sh,v.sub(left+1,right));
			for(int j=0;j<temp.count();j++)
			{
				param.push(ybase::vword_to_vstr(temp[j]));
			}
			if(!replace_w(v[i],param,mitem))
			{
				rserror(v.get(i));
				return false;
			}
			ybase::clear_word_val(v,left,right+1);
		}
		else
		{
			if(!replace_w(v[i],param,mitem))
			{
				rserror(v.get(i));
				return false;
			}
		}
		return true;
	}

	static rbool replace_w(tword& word,rbuf<rbuf<rstr> >& param,tmac& mitem)
	{
		if(param.count()!=mitem.param.count())
		{
			return false;
		}
		word.val.clear();
		if(param.empty())
		{
			word.multi=mitem.vstr;
			return true;
		}
		for(int i=0;i<mitem.vstr.count();i++)
		{
			int j;
			for(j=0;j<mitem.param.count();j++)
			{
				if(mitem.param[j]==mitem.vstr[i])
				{
					word.multi+=param[j];
					break;
				}
			}
			if(j>=mitem.param.count())
			{
				word.multi.push(mitem.vstr[i]);
			}
		}
		return true;
	}
};
