#pragma once

#include "yautof.h"
#include "ysuper.h"

//提取类内部的成员，函数默认参数处理
//zclass::vword->vdata,vfunc,vmac
struct ymemb
{
	static rbool process(tsh& sh)
	{
		for(tclass* p=sh.m_class.begin();
			p!=sh.m_class.end();p=sh.m_class.next(p))
		{
			ifn(a_class(sh,*p))
			{
				return false;
			}
		}
		for(tclass* p=sh.m_class.begin();
			p!=sh.m_class.end();p=sh.m_class.next(p))
		{
			ifn(recursion_get_size(sh,*p))
			{
				return false;
			}
			if(p->name==rskey(c_main))
			{
				sh.m_main_data.set_size(p->size);
			}
		}
		//简便起见，重复获取参数大小也没关系
		for(tclass* p=sh.m_class.begin();
			p!=sh.m_class.end();p=sh.m_class.next(p))
		{
			obtain_size_func(sh,*p);
		}
		return true;
	}

	//解析一个类
	static rbool a_class(tsh& sh,tclass& tci)
	{
		if(tci.vword.empty())
		{
			return true;
		}
		ifn(analyse(sh,tci))
		{
			return false;
		}
		ifn(yautof::auto_add_func(sh,tci))
		{
			return false;
		}
		tci.vword.free();
		return true;
	}

	static rbool recursion_get_size(tsh& sh,tclass& tci,int level=0)
	{
		if(level>c_rs_deep)
		{
			rserror(rstr("level overflow ")+tci.name);
			return false;
		}
		level++;
		if(tci.size>0||tci.size==0&&tci.vdata.empty())
		{
			return true;
		}	
		int size=0;
		for(int i=0;i<tci.vdata.count();i++)
		{
			if(ybase::is_point_quote(tci.vdata[i].type))
			{
				tci.vdata[i].off=size;
				tci.vdata[i].size=c_point_size;
				size+=tci.vdata[i].size*tci.vdata[i].count;
			}
			else
			{
				tclass* ptci=yfind::class_search(sh,tci.vdata[i].type);
				if(null==ptci)
				{
					rserror(rstr("can't find ")+tci.vdata[i].type);
					return false;
				}
				if(!recursion_get_size(sh,*ptci,level))
				{
					return false;
				}
				tci.vdata[i].off=size;
				tci.vdata[i].size=ptci->size;
				size+=tci.vdata[i].size*tci.vdata[i].count;
			}
		}
		tci.size=size;
		return true;
	}

	static rbool analyse(tsh& sh,tclass& tci)
	{
		rbuf<tword>& v=tci.vword;
		int i;
		for(i=0;i<v.count();i++)
		{
			if(v[i].val==rskey(c_enum))
			{
				int left=i+1;
				if(v.get(left).val!=rsoptr(c_bbk_l))
				{
					rserror(v.get(i),"miss {");
					return false;
				}
				int right=ybase::find_symm_bbk(sh,v,left);
				if(right>=v.count())
				{
					rserror(v.get(i),"miss }");
					return false;
				}
				ifn(enum_add(sh,tci,v.sub(left+1,right)))
				{
					return false;
				}
				i=right;
			}
			elif(v[i].val==rskey(c_mac))
			{
				int right;
				if(!mac_add(sh,tci,v,i,right))
				{
					return false;
				}
				i=right;
			}
			elif(v[i].val==rskey(c_template))
			{
				int left=v.find(rsoptr(c_bbk_l),i+1);
				if(left>=v.count())
				{
					rserror(v.get(i));
					return false;
				}
				int right=ybase::find_symm_bbk(sh,v,left);
				if(right>=v.count())
				{
					rserror(v.get(i),"miss }");
					return false;
				}
				if(!func_add(sh,tci,v.sub(i,right+1)))
				{
					return false;
				}
				i=right;
			}
			else
			{
				for(int j=i;j<v.count();j++)
				{
					if(v[j].val==rsoptr(c_semi))
					{
						if(!data_add(sh,tci,v.sub(i,j)))
						{
							return false;
						}
						i=j;
						break;
					}
					if(v[j].val==rsoptr(c_sbk_l))
					{
						j=ybase::find_symm_sbk(sh,v,j);
						if(j>=v.count())
						{
							rserror(v.get(i));
							return false;
						}
						if(v.get(j+1).val==rsoptr(c_bbk_l))
						{
							int right=ybase::find_symm_bbk(sh,v,j+1);
							if(right>=v.count())
							{
								rserror(v.get(i));
								return false;
							}
							if(!func_add(sh,tci,v.sub(i,right+1)))
							{
								return false;
							}
							i=right;
							break;
						}
					}
					if(v[j].pos!=v.get(j+1).pos)
					{
						if(!data_add(sh,tci,v.sub(i,j+1)))
						{
							return false;
						}
						i=j;
						break;
					}
				}
			}
		}
		return true;
	}

	static rbool enum_add(const tsh& sh,tclass& tci,const rbuf<tword>& v)
	{
		uint enum_val=0;
		for(int j=0;j<v.count();j++)
		{
			if(v[j].val==rsoptr(c_comma)||v[j].val==rsoptr(c_semi))
			{
				continue;
			}
			tmac eitem;
			eitem.name=v[j].val;
			if(v.get(j+1).val==rsoptr(c_equal))
			{
				if(v.get(j+2)==rsoptr(c_sbk_l))
				{
					enum_val=v.get(j+3).val.touint();
					eitem.vstr.push(rstr(enum_val));
					enum_val++;
					j+=4;
				}
				else
				{
					enum_val=v.get(j+2).val.touint();
					eitem.vstr.push(rstr(enum_val));
					enum_val++;
					j+=2;
				}
			}
			else
			{
				eitem.vstr.push(rstr(enum_val));
				enum_val++;
			}
			if(tci.vmac.exist(eitem))
			{
				rserror(v.get_bottom(),"enum redefined");
				return false;
			}
			tci.vmac.insert(eitem);
		}
		return true;
	}

	static rbool mac_add(const tsh& sh,tclass& tci,const rbuf<tword>& v,int i,int& right)
	{
		if(v.get(i+1).val=="$")
		{
			ifn(ysuper::add_super_mac(sh,v,i,tci.vmac))
			{
				return false;
			}
			right=i;
			return true;
		}
		tmac mitem;
		int left=i+2;
		mitem.name=v.get(left-1).val;
		if(mitem.name.empty())
		{
			rserror(v.get(i),"mac miss name");
			return false;
		}
		if(v.get(left).val==rsoptr(c_sbk_l))
		{
			right=ybase::find_symm_sbk(sh,v,left);
			if(right>=v.count())
			{
				rserror(v.get(i),"miss )");
				return false;
			}
			rbuf<rbuf<rstr> > temp=r_split_a<rstr>(
				ybase::vword_to_vstr(v.sub(left+1,right)),
				rsoptr(c_comma));
			for(int j=0;j<temp.count();j++)
			{
				mitem.param.push(temp[j].get(0));
			}
			right++;
			left=right;
		}
		right=left;
		if(v.get(left).val==rsoptr(c_bbk_l))
		{
			right=ybase::find_symm_bbk(sh,v,left);
			if(right>=v.count())
			{
				rserror(v.get(i),"miss }");
				return false;
			}
			for(int j=left+1;j<right;j++)
			{
				mitem.vstr.push(v[j].val);
			}
		}
		else
		{
			for(;right<v.count()&&v[right].pos==v[i].pos;right++)
			{
				;
			}
			right--;
			if(right>=v.count())
			{
				rserror(v.get(i),"miss mac");
				return false;
			}
			if(mitem.param.empty())
			{
				for(int j=left;j<=right;j++)
				{
					mitem.vstr.push(v[j].val);
				}
			}
			else
			{
				for(int j=left;j<=right;j++)
				{
					if(mitem.param.exist(v[j].val))
					{
						mitem.vstr.push(v[j].val);
					}
					else
					{
						mitem.vstr.push(v[j].val);
					}
				}
			}
		}
		if(tci.vmac.exist(mitem))
		{
			rserror(v.get_bottom(),"mac redefined");
			return false;
		}
		tci.vmac.insert(mitem);
		return true;
	}

	static rbool a_data_define(const tsh& sh,tdata& item,const rbuf<tword>& vword)
	{
		int pos=vword.find(tword(rsoptr(c_equal)));
		if(pos<vword.count())
		{
			for(int i=pos+1;i<vword.count();i++)
			{
				item.param.push(vword[i]);
			}
		}
		else
		{
			pos=vword.find(tword(rsoptr(c_sbk_l)));
			if(pos<vword.count())
			{
				for(int i=pos+1;i<vword.count()-1;i++)
				{
					item.param.push(vword[i]);
				}
			}
		}
		//process func template
		if(vword.get(1).val==rsoptr(c_tbk_l))
		{
			int end=ybase::find_symm_tbk(sh,vword,1);
			if(end>=vword.count())
			{
				rserror(vword.get(0),"miss >");
				return false;
			}
			for(int i=0;i<end;i++)
			{
				item.type+=vword[i].val;
			}
			int pointpos=end+1;
			for(;pointpos<pos;++pointpos)
			{
				if(vword[pointpos].val!=rsoptr(c_addr)&&
					vword[pointpos].val!=rsoptr(c_mul))
				{
					break;
				}
				item.type+=vword[pointpos].val;
			}
			item.name=vword.get(pointpos).val;
		}
		else
		{
			item.type=vword.get(0).val;
			if(yfind::is_class_t(sh,item.type))
			{
				item.name=vword.get(1).val;
			}
			else
			{
				//item.type="var&";
				//item.name=vword.get(0).val;
				rserror(vword.get(0),"miss data type or name");
				return false;
			}
		}
		if(item.type.empty()||item.name.empty())
		{
			rserror(vword.get(0),"miss data type or name");
			return false;
		}
		item.count=1;
		if(vword.get(2)==rsoptr(c_mbk_l))
		{
			if(vword.get(4)!=rsoptr(c_mbk_r))
			{
				rserror(vword.get(0),"data array error");
				return false;
			}
			//保存数组个数
			item.count=vword.get(3).val.toint();
		}
		return true;
	}

	static rbool lambda_a_func_define(const tsh& sh,tfunc& item,const rbuf<tword>& v)
	{
		if(v.get(1)!=rsoptr(c_sbk_l))
		{
			rserror(v.get(0),"miss (");
			return false;
		}
		item.name=v.get(0).val;
		int right=ybase::find_symm_sbk(sh,v,1);
		if(right>=v.count())
		{
			rserror(v.get(0),"miss )");
			return false;
		}
		rbuf<tword> param=v.sub(2,right);
		rbuf<rbuf<tword> > list=ybase::comma_split<tword>(sh,param);
		int i=0;
		//如果第一个参数只有类型，则表明是返回值
		if(!list.empty()&&list[0].count()==1&&
			yfind::is_class_t(sh,list[0].get_bottom().val))
		{
			item.retval.type=list[0].get_bottom().val;
			i++;
		}
		else
		{
			item.retval.type=rskey(c_void);
		}
		item.retval.name=rskey(c_s_ret);
		for(;i<list.count();i++)
		{
			if(list[i].empty())
			{
				continue;
			}
			tdata ditem;
			ifn(a_data_define(sh,ditem,list[i]))
			{
				return false;
			}
			item.param.push(ditem);
		}
		return true;
	}

	static rbool a_func_define(const tsh& sh,tfunc& item,const rbuf<tword>& v,
		rbool lambda=false)
	{
		item.name.clear();
		if(v.get_bottom().val.sub(0,7)=="_LAMBDA")
		{
			return lambda_a_func_define(sh,item,v);
		}
		int start=0;
		for(;start<v.count();start++)
		{
			if(v[start].val==rskey(c_friend))
			{
				item.is_friend=true;
			}
			elif(v[start].val==rskey(c_cfunc))
			{
				item.is_cfunc=true;
			}
			elif(v[start].val=="macro")
			{
				item.is_macro=true;
				item.is_cfunc=true;
			}
			elif(v[start].val==rskey(c_dynamic))
			{
				item.is_dynamic=true;
			}
			elif(v[start].val=="final")
			{
				item.is_final=true;
			}
			else
			{
				break;
			}
		}
		if(lambda)
		{
			if(yfind::is_class_t(sh,v.get(start).val))
			{
				item.retval.type=v.get(start).val;
				start++;
			}
			else
			{
				item.retval.type=rskey(c_void);
			}
		}
		else
		{
			if(yfind::is_class_t(sh,v.get(start).val)
				&&v.get(start+1).val!=rsoptr(c_sbk_l)
				&&!v.get(start+1).val.empty())
			{
				item.retval.type=v.get(start).val;
				start++;
			}
			else
			{
				item.retval.type=rskey(c_void);
			}
		}
		if(item.retval.type.empty())
		{
			rserror(v.get(0),"miss ret type");
			return false;
		}
		item.retval.name=rskey(c_s_ret);
		if(v.get(start).val==rsoptr(c_destruct))
		{
			item.name+=v.get(start).val;
			start++;
			item.name+=v.get(start).val;
			start++;
		}
		elif(lambda&&(
			v.get(start)==rsoptr(c_bbk_l)||
			v.get(start)==rsoptr(c_sbk_l)))
		{
			item.name+="_SELF";
		}
		elif(sh.m_optr.is_optr(v.get(start).val))
		{
			item.name=v.get(start).val;
			if(v.get(start).val==rsoptr(c_mbk_l))
			{
				start++;
				item.name+=v.get(start).val;
			}
			start++;
		}
		else
		{
			item.name+=v.get(start).val;
			start++;
		}
		if(item.name.empty())
		{
			rserror(v.get(0),"miss func name");
			return false;
		}
		rbuf<rbuf<tword> > list;
		int right;
		if(v.get(start).val==rsoptr(c_sbk_l))
		{
			right=ybase::find_symm_sbk(sh,v,start);
			if(right>=v.count())
			{
				rserror(v.get(0),"miss )");
				return false;
			}
			list=ybase::comma_split<tword>(sh,v.sub(start+1,right));
		}
		else
		{
			list=ybase::comma_split<tword>(sh,v.sub(start));
		}
		for(int i=0;i<list.count();i++)
		{
			if(list[i].empty())
			{
				continue;
			}
			tdata ditem;
			ifn(a_data_define(sh,ditem,list[i]))
			{
				return false;
			}
			item.param.push(ditem);
		}
		return true;
	}

	static rbool data_add(const tsh& sh,tclass& tci,const rbuf<tword>& v)
	{
		if(v.empty())
		{
			return true;
		}
		tdata item;
		if(!a_data_define(sh,item,v))
		{
			return false;
		}
		if(tci.vdata.exist(item))
		{
			rserror(v.get_bottom(),"data member redefined");
			return false;
		}
		tci.vdata.push(item);
		return true;
	}

	static rbool combine_tl_name(const tsh& sh,rbuf<tword>& v)
	{
		v=v.sub(2);
		int sbk_left=v.count();
		if(v.get_top()!=rsoptr(c_sbk_r))
		{
			return false;
		}
		sbk_left=ybase::find_symm_word_rev(v,
			rsoptr(c_sbk_l),rsoptr(c_sbk_r),
			0,v.count());
		if(sbk_left>=v.count())
		{
			return false;
		}
		int tbk_right=sbk_left-1;
		int tbk_left=ybase::find_symm_word_rev(v,
			rsoptr(c_tbk_l),rsoptr(c_tbk_r),
			0,sbk_left);
		if(tbk_left>=v.count())
		{
			return false;
		}
		ifn(v.get(tbk_left-1).is_name())
		{
			return false;
		}
		v[tbk_left-1].val+=rstr::join<rstr>(
			ybase::vword_to_vstr(v.sub(tbk_left,tbk_right+1)),
			rstr());
		ybase::clear_word_val(v,tbk_left,tbk_right+1);
		ybase::arrange(v);
		return true;
	}

	static rbool have_move(const tsh& sh,const rbuf<tword>& v)
	{
		int sbk_left=v.count();
		if(v.get_top()!=rsoptr(c_sbk_r))
		{
			return false;
		}
		sbk_left=ybase::find_symm_word_rev(v,
			rsoptr(c_sbk_l),rsoptr(c_sbk_r),
			0,v.count());
		if(sbk_left>=v.count())
		{
			return false;
		}
		for(int i=sbk_left;i<v.count();i++)
		{
			if(v[i]==rsoptr(c_and)&&yfind::is_class(sh,v.get(i-1).val))
			{
				return true;
			}
		}
		return false;
	}
	
	static rbool func_add(tsh& sh,tclass& tci,const rbuf<tword>& v,
		rbool check_tl=true)
	{
		tfunc item;
		int left=v.find(tword(rsoptr(c_bbk_l)));
		if(left>=v.count())
		{
			rserror(v.get_bottom(),"miss {");
			return false;
		}
		int right=v.count()-1;
		item.pos=v[left].pos;
		item.ptci=&tci;//反射
		rbuf<tword> vhead=v.sub(0,left);
		if(have_move(sh,vhead))
		{
			return true;
		}
		if(check_tl)
		{
			if(v.get_left()==rskey(c_template)&&
				v.get(1)==rsoptr(c_less_great))
			{
				ifn(combine_tl_name(sh,vhead))
				{
					rserror(v.get_bottom(),"combine_tl_name");
					return false;
				}
			}
			else
			{
				func_tl_part(sh,item,vhead);
			}
		}
		if(item.vtl.empty())
		{
			if(!a_func_define(sh,item,vhead))
			{
				return false;
			}
			if(item.name.sub(0,7)!="_LAMBDA")
			{
				add_this_func(sh,item);
			}
			obtain_size_func(sh,item);
			item.name_dec=item.get_dec();
			if(tci.vfunc.exist(item))
			{
				if(check_tl)
				{
					rserror(v.get_bottom(),"func redefined");
					return false;
				}
				return true;
			}
			item.vword=v.sub(left+1,right);
			tci.vfunc.insert(item);
			if(item.is_macro)
			{
				sh.m_macro[item.name]=tci.vfunc.find(item);
			}
			if(!default_param_proc(sh,*tci.vfunc.find(item)))
			{
				return false;
			}
		}
		else
		{
			item.name_dec=item.get_dec();
			if(tci.vfunctl.exist(item))
			{
				rserror(v.get_bottom(),"functl redefined");
				return false;
			}
			item.vword=vhead+v.sub(left,right+1);
			tci.vfunctl.insert(item);
		}
		return true;
	}

	static void obtain_size_func(const tsh& sh,tclass& tci)
	{
		for(tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			obtain_size_func(sh,*p);
		}
	}

	static void obtain_size_func(const tsh& sh,tfunc& tfi)
	{
		tfi.retval.size=yfind::get_type_size(sh,tfi.retval.type);
		for(int i=0;i<tfi.param.count();i++)
		{
			tfi.param[i].size=yfind::get_type_size(sh,tfi.param[i].type);
		}
	}

	static void cpp_tl_replace(const tsh& sh,rbuf<tword>& v)
	{
		int sbk_left=v.count();
		if(v.get_top()!=rsoptr(c_tbk_r))
		{
			if(v.get_top()!=rsoptr(c_sbk_r))
			{
				return;
			}
			sbk_left=ybase::find_symm_word_rev(v,
				rsoptr(c_sbk_l),rsoptr(c_sbk_r),
				0,v.count());
			if(sbk_left>=v.count())
			{
				return;
			}
		}
		int tbk_left=1;
		int tbk_right=ybase::find_symm_tbk(sh,v,tbk_left);
		if(tbk_right>=v.count())
		{
			return;
		}
		v[sbk_left].multi+=ybase::vword_to_vstr(v.sub(tbk_left,tbk_right+1));
		v[sbk_left].multi+=rsoptr(c_sbk_l);
		v[sbk_left].val.clear();
		ybase::clear_word_val(v,0,tbk_right+1);
		ybase::arrange(v);
	}

	static void func_tl_part(const tsh& sh,tfunc& item,rbuf<tword>& v)
	{
		if(v.get_left()==rskey(c_template))
		{
			cpp_tl_replace(sh,v);
		}
		int left=v.count();
		if(v.get_top()!=rsoptr(c_tbk_r))
		{
			if(v.get_top()!=rsoptr(c_sbk_r))
			{
				return;
			}
			left=ybase::find_symm_word_rev(v,
				rsoptr(c_sbk_l),rsoptr(c_sbk_r),
				0,v.count());
			if(left>=v.count())
			{
				return;
			}
		}
		if(v.get(left-1)!=rsoptr(c_tbk_r))
		{
			return;
		}
		rbuf<tword> temp=v.sub(0,left);
		left=ybase::find_symm_word_rev(temp,
			rsoptr(c_tbk_l),rsoptr(c_tbk_r),
			0,temp.count());
		if(left>=temp.count())
		{
			return;
		}
		item.name=temp.get(left-1).val;
		if(item.name.empty())
		{
			return;
		}
		rbuf<rstr> vsrc=ybase::vword_to_vstr(
			temp.sub(left+1,temp.count()-1));
		rbuf<rbuf<rstr> > vdst=r_split_a<rstr>(vsrc,rsoptr(c_comma));
		if(vdst.empty())
		{
			return;
		}
		for(int i=0;i<vdst.count();i++)
		{
			ttl tli;
			tli.name=vdst[i].get_right();
			item.vtl.push(tli);
		}
	}

	static void add_this_func(const tsh& sh,tfunc& tfi)
	{
		if(tfi.ptci->is_friend)
		{
			tfi.is_friend=true;
		}
		if(!tfi.is_friend)
		{
			tdata tdi;
			tdi.name=rskey(c_this);
			tdi.type=tfi.ptci->name+rsoptr(c_addr);
			tfi.param.push_front(tdi);
		}
	}

	static rbool default_param_proc(const tsh& sh,tfunc& tfi)
	{
		int i;
		for(i=0;i<tfi.param.count();i++)
		{
			if(!tfi.param[i].param.empty())
			{
				break;
			}
		}
		if(i>=tfi.param.count())
		{
			return true;
		}
		for(int j=i;j<tfi.param.count();j++)
		{
			if(tfi.param[j].param.empty())
			{
				rserror(tfi.vword.get_bottom(),"default param error");
				return false;
			}
		}
		for(;i<tfi.param.count();i++)
		{
			tfunc ftemp=tfi;
			ftemp.param.erase(i,ftemp.param.count());
			rbuf<tword> vtemp;
			tword twi;
			twi.pos=tfi.pos;
			for(int j=i;j<tfi.param.count();j++)
			{
				twi.val=tfi.param[j].type;
				vtemp.push(twi);
				twi.val=tfi.param[j].name;
				vtemp.push(twi);
				twi.val=rsoptr(c_sbk_l);
				vtemp.push(twi);
				for(int k=0;k<tfi.param[j].param.count();k++)
				{
					twi.val=tfi.param[j].param[k].val;
					vtemp.push(twi);
				}
				twi.val=rsoptr(c_sbk_r);
				vtemp.push(twi);
				twi.val=rsoptr(c_semi);
				vtemp.push(twi);
			}
			vtemp+=r_move(ftemp.vword);
			ftemp.vword=r_move(vtemp);
			for(int j=0;j<ftemp.param.count();j++)
			{
				ftemp.param[j].param.free();
			}
			ftemp.name_dec=ftemp.get_dec();
			if(tfi.ptci->vfunc.exist(ftemp))
			{
				rserror(tfi.vword.get_bottom(),"func redefined");
				return false;
			}
			tfi.ptci->vfunc.insert(ftemp);
		}
		for(i=0;i<tfi.param.count();i++)
		{
			tfi.param[i].param.free();
		}
		return true;
	}
};
