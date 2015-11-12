#pragma once

#include "yfind.h"

//提取所有类
//m_file.vword->m_class.vword
struct yclass
{	
	static rbool process(tsh& sh)
	{
		basic_type_add(sh);
		for(tfile* p=sh.m_file.begin();p!=sh.m_file.end();p=sh.m_file.next(p))
		{
			if(!extract_class(sh,p->vword))
			{
				return false;
			}
		}
		main_add(sh);
		for(tfile* p=sh.m_file.begin();p!=sh.m_file.end();p=sh.m_file.next(p))
		{
			if(!proc_class_again(sh,p->vword))
			{
				return false;
			}
		}
		if(!inherit_proc_all(sh))
		{
			return false;
		}
		for(tfile* p=sh.m_file.begin();p!=sh.m_file.end();p=sh.m_file.next(p))
		{
			p->cont.m_buf.free();
			p->vword.free();
		}
		return true;
	}

	static void main_add(tsh& sh)
	{
		sh.m_main=yfind::class_search(sh,rskey(c_main));
		if(sh.m_main==null)
		{
			tclass item;
			item.name=rskey(c_main);
			sh.m_class.insert(item);
			sh.m_main=yfind::class_search(sh,rskey(c_main));	
		}
		sh.m_main->is_friend=true;
	}

	static rbool proc_class_again(tsh& sh,const rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			sh.m_main->vword.push(v[i]);
		}
		return true;
	}

	static rbool name_part(const tsh& sh,tclass& tci,rbuf<tword>& name_w)
	{
		rbuf<tword> father;
		int colonpos=r_find_a<tword>(name_w,tword(rsoptr(c_colon)));
		if(colonpos<name_w.count())
		{
			for(int i=colonpos+1;i<name_w.count();i++)
			{
				father.push(name_w[i]);
			}
			if(father.empty())
			{
				rserror(tci.vword.get(0),"empty father");
				return false;
			}
			name_w.erase(colonpos,name_w.count());
			tci.vfather=ybase::comma_split_t(sh,father);
		}
		int pos=r_find_a<tword>(name_w,tword(rsoptr(c_tbk_l)));
		if(pos<name_w.count())
		{
			rstr temp;
			for(int i=pos+1;i<name_w.count()-1;i++)
			{
				temp+=name_w[i].val;
			}
			rbuf<rstr> result=r_split<rstr>(temp,rsoptr(c_comma));
			if(result.empty())
			{
				rserror(tci.vword.get(0),"template >error");
				return false;
			}
			for(int i=0;i<result.count();i++)
			{
				ttl item;
				int epos=r_find<rstr>(result[i],rsoptr(c_equal));
				for(int j=0;j<epos;j++)
				{
					item.name+=result[i][j];
				}
				if(item.name.empty())
				{
					rserror(tci.vword.get(0),"empty template");
					return false;
				}
				for(int j=epos+1;j<result[i].count();j++)
				{
					item.val+=result[i][j];
				}
				tci.vtl.push(item);
			}
		}
		for(int i=0;i<pos;i++)
		{
			tci.name+=name_w[i].val;
		}
		if(tci.name.empty())
		{
			rserror(tci.vword.get(0),"empty class name");
			return false;
		}
		return true;
	}

	static rbool proc_cpp_tl(const tsh& sh,rbuf<tword>& v,int i,rbuf<tword>& name_w)
	{
		int right=i-1;
		if(v.get(right)!=rsoptr(c_tbk_r))
		{
			return true;
		}
		int left=ybase::find_symm_word_rev(v,rsoptr(c_tbk_l),rsoptr(c_tbk_r),0,i);
		if(left>=v.count())
		{
			return true;
		}
		if(v.get(left-1)!=rskey(c_template))
		{
			return true;
		}
		rbuf<rbuf<tword> > vparam;
		vparam=ybase::comma_split<tword>(sh,v.sub(left+1,right));
		if(vparam.empty())
		{
			return true;
		}
		
		for(int j=0;j<vparam.count();j++)
		{
			if(vparam[j].get(0)==rskey(c_typename))
			{
				vparam[j].pop_front();
			}
		}
		rbuf<tword> vtl;
		vtl+=rsoptr(c_tbk_l);
		for(int j=0;j<vparam.count();j++)
		{
			if(j!=0)
			{
				vtl+=rsoptr(c_comma);
			}
			vtl+=vparam[j];
		}
		vtl+=rsoptr(c_tbk_r);
		name_w.insert(1,vtl);
		for(int j=left-1;j<i;j++)
		{
			v[j].clear();
		}
		return true;
	}

	static rbool extract_class(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rskey(c_class))
			{
				continue;
			}
			if(v.get(i+2)==rsoptr(c_semi)&&v.get(i+1).is_name())
			{
				ybase::clear_word_val(v,i,i+3);
				continue;
			}
			int left=r_find_a<tword>(v,tword(rsoptr(c_bbk_l)),i+1);
			if(left>=v.count())
			{
				ybase::error(v.get(i),"miss {");
				return false;
			}
			tclass item;
			rbuf<tword> name_w;
			for(int j=i+1;j<left;j++)
			{
				name_w.push(v[j]);
			}
			if(name_w.empty())
			{
				rserror(v.get(i),"miss class name");
				return false;
			}
			proc_cpp_tl(sh,v,i,name_w);
			int right=ybase::find_symm_bbk(sh,v,left);
			if(right>=v.count())
			{
				rserror(v.get(i),"miss {");
				return false;
			}
			if(v.get(i-1).val==rskey(c_friend))
			{
				v[i-1].clear();
				item.is_friend=true;
			}
			for(int j=left+1;j<right;j++)
			{
				item.vword.push(v[j]);
			}
			for(int j=i;j<=right;j++)
			{
				v[j].clear();
			}
			ifn(class_add(sh,item,name_w))
			{
				return false;
			}
			i=right;
		}
		ybase::arrange(v);
		return true;
	}

	static rbool class_add(tsh& sh,tclass& tci,rbuf<tword>& name_w)
	{
		ifn(name_part(sh,tci,name_w))
		{
			return false;
		}
		if(yfind::is_class(sh,tci.name)||yfind::is_classtl(sh,tci.name))
		{
			rserror(name_w.get_bottom(),"type redefined");
			return false;
		}
		if(tci.vtl.empty())
		{
			sh.m_class.insert(tci);
		}
		else
		{
			sh.m_classtl.insert(tci);
		}
		return true;
	}

	static void insert_type(tsh& sh,const rstr& name,int size)
	{
		tclass item;
		item.name=name;
		item.size=size;
		if(!yfind::is_class(sh,item.name))
		{
			sh.m_class.insert(item);
		}
	}

	static void basic_type_add(tsh& sh)
	{
		insert_type(sh,rskey(c_void),0);
		insert_type(sh,rskey(c_rd8),1);
		insert_type(sh,rskey(c_rd16),2);
		insert_type(sh,rskey(c_rd32),4);
		insert_type(sh,rskey(c_rd64),8);
		insert_type(sh,rskey(c_rdp),sh.m_point_size);
	}

	static rbool inherit_proc_all(tsh& sh)
	{
		for(tclass* p=sh.m_class.begin();
			p!=sh.m_class.end();p=sh.m_class.next(p))
		{
			if(!inherit_proc(sh,*p))
			{
				return false;
			}
		}
		return true;
	}
	
	//模板继承模板如 A<T>:B<T>
	//模板继承非模板 A<T>:C,D
	//非模板继承非模板 E:C
	//模板实例暂时不能继承如 C:A<int>
	//如果定义同名函数会调用子类的函数，所以不要用覆盖和隐藏
	//后面自动生成构造、析构、拷贝构造、operator=不会继承，但用户定义的会继承
	static rbool inherit_proc(tsh& sh,tclass& tci,int level=0)
	{
		if(level>c_rs_deep)
		{
			rserror("inherit too deep");
			return false;
		}
		level++;
		if(tci.vfather.empty())
		{
			return true;
		}
		rbuf<tword> v;
		for(int i=0;i<tci.vfather.count();i++)
		{
			rstr cname=tci.vfather[i].vword.get(0).val;
			tclass* ptci=yfind::class_search(sh,cname);
			if(ptci==null)
			{
				ptci=yfind::classtl_search(sh,cname);
				if(ptci==null)
				{
					rserror("inherit can't find "+cname);
					return false;
				}
			}
			if(!inherit_proc(sh,*ptci,level))
			{
				return false;
			}
			v+=ptci->vword;
		}
		v+=tci.vword;
		tci.vword=r_move(v);
		return true;
	}
};
