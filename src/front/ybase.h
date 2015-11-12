#pragma once

#include "../rlib/rf.h"
#include "../rlib/rdir.h"
#include "../struct/tbase.h"

struct ybase
{
	static void print_vclass(const tsh& sh)
	{
		for(tclass* p=sh.m_class.begin();p!=sh.m_class.end();p=sh.m_class.next(p))
		{
			p->name.printl();
		}
	}

	static void print_vword(const rbuf<tword>& v)
	{
		rf::printl();
		for(int i=0;i<v.count();i++)
		{
			(v[i].val+" ").print();
		}
		rf::printl();
	}

	static void print_vstr(const rbuf<rstr>& v)
	{
		rf::printl();
		for(int i=0;i<v.count();i++)
		{
			(v[i]+" ").print();
		}
		rf::printl();
	}

	static void print_func_sent(const tfunc& tfi)
	{
		rf::printl();
		for(int i=0;i<tfi.vsent.count();i++)
		{
			//rf::print(tfi.vsent[i].type+" ");
			ifn(is_tag<tword>(tfi.vsent[i].vword))
			{
				rf::print("\t");
			}
			//rf::print(rstr(tfi.vsent[i].pos.line)+" ");
			rstr::join<rstr>(vword_to_vstr(tfi.vsent[i].vword)," ").printl();
		}
		rf::printl();
	}

	static void print_func_asm(const tfunc& tfi)
	{
		for(int i=0;i<tfi.vasm.count();i++)
		{
			rf::print(rstr(tfi.vasm[i].pos.line)+" ");
			rstr::join<rstr>(tfi.vasm[i].vstr," ").printl();
		}
		rf::printl();
	}

	static void print_func_dec(const tclass& tci)
	{
		for(tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			get_func_declare(tci,*p).printl();
		}
	}

	static void error(rstr e=rstr())
	{
		rf::printl(e);
	}

	static void error(const tword& word,rstr e=rstr())
	{
		error(rstr("file: ")+get_file_name(word.pos.file)+
			rstr("\nline: ")+rstr(word.pos.line)+
			rstr("\nword: ")+word.val+rstr("\n")+e);
		error(get_src_str(word));
	}

	static void error(const tsent& sent,rstr e=rstr())
	{
		error(rstr("file: ")+get_file_name(sent.pos.file)+
			rstr("\nline: ")+rstr(sent.pos.line)+rstr("\n")+e);
		rstr::join<rstr>(vword_to_vstr(sent.vword)," ").printl();
		error(get_src_str(sent.vword.get_bottom()));
	}

	static void error(const tasm& oasm,rstr e=rstr())
	{
		error(rstr("file: ")+get_file_name(oasm.pos.file)+
			rstr("\nline: ")+rstr(oasm.pos.line)+rstr("\n")+e);
		oasm.vstr.print();
		rf::printl();
	}

	static void error(const tfunc& tfi,rstr e=rstr())
	{
		tclass* ptci=tfi.ptci;
		if(ptci==null)
		{
			return;
		}
		rf::printl(get_func_declare(*ptci,tfi));
		rf::printl(e);
	}

	static rstr vword_to_s(rbuf<tword>& v)
	{
		rstr s;
		for(int i=0;i<v.count();i++)
		{
			if(v.get(i-1).pos!=v[i].pos)
			{
				s+="\n";
			}
			s+=v[i].val+" ";
		}
		return s;
	}

	static rstr get_func_param_declare(const tfunc& tfi)
	{
		rstr s;
		for(int j=0;j<tfi.param.count();j++)
		{
			if(j!=0)
			{
				s+=",";
			}
			s+=tfi.param[j].type+" "+tfi.param[j].name;
		}
		return r_move(s);
	}

	static rstr get_func_declare(const tclass& tci,const tfunc& tfi)
	{
		rstr s;
		s+=tci.name+"."+tfi.name+"(";
		for(int j=0;j<tfi.param.count();j++)
		{
			if(j!=0)
			{
				s+=",";
			}
			s+=tfi.param[j].type;
		}
		s+=")";
		return r_move(s);
	}

	static rbuf<rstr> get_func_declare_lisp(const tsh& sh,const tclass& tci,const tfunc& tfi)
	{
		return get_func_declare_call(sh,tci,tfi).sub(1);
	}

	static rbuf<rstr> get_func_declare_call(const tsh& sh,const tclass& tci,const tfunc& tfi)
	{
		rbuf<rstr> ret;
		ret+=rskey(c_call);
		ret+=rsoptr(c_mbk_l);
		ret+=rsoptr(c_addr);
		ret+=rsoptr(c_comma);
		ret+=tci.name;
		ret+=rsoptr(c_comma);
		ret+=tfi.name_dec;
		ret+=rsoptr(c_mbk_r);
		return r_move(ret);
	}

	static void clear_word_val(rbuf<tword>& v,int begin,int end)
	{
		if(end>v.count())
		{
			end=v.count();
		}
		if(begin<0)
		{
			begin=0;
		}
		for(int i=begin;i<end;++i)
		{
			v[i].val.clear();
		}
	}

	static rstr get_main_name(const tsh& sh)
	{
		rstr name=sh.m_main_file;
		name=rdir::get_name(name);
		name=rdir::get_prev_dir(sh.m_main_file)+name.sub(
			0,name.count()-rdir::get_suffix(name).count()-1);
		return r_move(name);
	}

	static rstr get_ctype(const tsh& sh,const tword& word)
	{
		if(word.is_cint())
		{
			return rskey(c_int);
		}
		elif(word.is_cuint())
		{
			return rskey(c_uint);
		}
		elif(word.is_cdouble())
		{
			return rskey(c_double);
		}
		elif(word.is_cpoint())
		{
			return rstr("rp<void>");
		}
		elif(word.is_cstr())
		{
			return rstr("rp<char>");
		}
		else
		{
			return rstr();
		}
	}

	//获取类型名称，忽略引用
	static rstr get_tname(const rstr& s)
	{
		if(is_quote(s))
		{
			return s.sub(0,s.count()-1);
		}
		else
		{
			return s;
		}
	}

	static rstr get_ttype(rstr s)
	{
		if(is_point_quote(s))
		{
			return s;
		}
		return s+"&";
	}

	static rbool is_optr_func(const tsh& sh,const tfunc& tfi)
	{
		if(sh.m_optr.is_optr(tfi.name))
		{
			return true;
		}
		return tfi.name=="[]";
	}

	static rbool is_basic_type(const tsh& sh,const rstr& s)
	{
		return (s==rskey(c_rd8)||s==rskey(c_rd16)||
			s==rskey(c_rd32)||s==rskey(c_rd64)||
			s==rskey(c_rdp)||s==rskey(c_void));
	}

	static rbool is_point_quote(const rstr& s)
	{
		return is_quote(s)||is_point(s);
	}

	static rbool is_quote(const rstr& s)
	{
		return s.get_top()==r_char('&');
	}

	static rbool is_point(const rstr& s)
	{
		if(s.get_top()==r_char('*'))
		{
			return true;
		}
		return (s.count()>=5&&s[0]==r_char('r')&&s[1]==r_char('p')&&
			s[2]==r_char('<')&&s.get_top()==r_char('>'));
	}

	static void split_param(const tsh& sh,rbuf<tsent>& vsent,
		const rbuf<tword>& v,const tsent& src)
	{
		vsent.clear();
		rbuf<rbuf<tword> > vparam=comma_split<tword>(sh,v);
		for(int i=0;i<vparam.count();i++)
		{
			tsent sent;
			sent.pos=src.pos;
			sent.vword=r_move(vparam[i]);
			vsent.push_move(sent);
		}
	}

	static void split_param(const tsh& sh,rbuf<tsent>& vsent,const rbuf<tword>& v)
	{
		vsent.clear();
		rbuf<rbuf<tword> > vparam=comma_split<tword>(sh,v);
		for(int i=0;i<vparam.count();i++)
		{
			tsent sent;
			sent.vword=r_move(vparam[i]);
			vsent.push_move(sent);
		}
	}

	template<typename T>
	static rbuf<rbuf<T> > comma_split(const tsh& sh,rbuf<T> v)
	{
		rbuf<rbuf<T> > result;
		v.push_move(T(rsoptr(c_comma)));
		int count1=0;
		int count2=0;
		int start=0;
		for(int i=0;i<v.count();i++)
		{
			//大括号不可能出现在参数中
			if(v[i]==rsoptr(c_sbk_l))
			{
				count1++;
			}
			elif(v[i]==rsoptr(c_sbk_r))
			{
				count1--;
			}
			elif(v[i]==rsoptr(c_mbk_l))
			{
				count2++;
			}
			elif(v[i]==rsoptr(c_mbk_r))
			{
				count2--;
			}
			elif(count1==0&&count2==0&&
				v[i]==rsoptr(c_comma))
			{
				rbuf<T> item;
				for(int j=start;j<i;j++)
				{
					item.push_move(v[j]);
				}
				ifn(item.empty())
				{
					result.push_move(item);
				}
				start=i+1;
			}
		}
		return r_move(result);
	}

	//允许空元素
	static rbuf<rbuf<tword> > comma_split_e(const tsh& sh,rbuf<tword> v)
	{
		rbuf<rbuf<tword> > result;
		v.push(tword(rsoptr(c_comma)));
		int count1=0;
		int count2=0;
		int start=0;
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val==rsoptr(c_sbk_l))
			{
				count1++;
			}
			elif(v[i].val==rsoptr(c_sbk_r))
			{
				count1--;
			}
			elif(v[i].val==rsoptr(c_mbk_l))
			{
				count2++;
			}
			elif(v[i].val==rsoptr(c_mbk_r))
			{
				count2--;
			}
			elif(count1==0&&count2==0&&
				v[i].val==rsoptr(c_comma))
			{
				rbuf<tword> item;
				for(int j=start;j<i;j++)
				{
					item.push_move(v[j]);
				}
				result.push_move(item);
				start=i+1;
			}
		}
		return r_move(result);
	}

	//带尖括号的split
	static rbuf<tsent> comma_split_t(const tsh& sh,rbuf<tword> v)
	{
		rbuf<tsent> result;
		v.push(tword(rsoptr(c_comma)));
		int count1=0;
		int count2=0;
		int count3=0;
		int start=0;
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val==rsoptr(c_sbk_l))
			{
				count1++;
			}
			elif(v[i].val==rsoptr(c_sbk_r))
			{
				count1--;
			}
			elif(v[i].val==rsoptr(c_mbk_l))
			{
				count2++;
			}
			elif(v[i].val==rsoptr(c_mbk_r))
			{
				count2--;
			}
			elif(v[i].val==rsoptr(c_tbk_l))
			{
				count3++;
			}
			elif(v[i].val==rsoptr(c_tbk_r))
			{
				count3--;
			}
			elif(count1==0&&count2==0&&count3==0&&
				v[i].val==rsoptr(c_comma))
			{
				tsent item;
				for(int j=start;j<i;j++)
				{
					item.vword.push_move(v[j]);
				}
				ifn(item.vword.empty())
				{
					result.push_move(item);
				}
				start=i+1;
			}
		}
		return r_move(result);
	}

	static int find_symm_sbk(const tsh& sh,const rbuf<rstr>& v,int begin=0)
	{
		return find_symm_word_e<rstr>(v,rsoptr(c_sbk_l),
			rsoptr(c_sbk_r),begin);
	}

	static int find_symm_sbk(const tsh& sh,const rbuf<tword>& v,int begin=0)
	{
		return find_symm_word_e<tword>(v,rsoptr(c_sbk_l),
			rsoptr(c_sbk_r),begin);
	}

	static int find_symm_mbk(const tsh& sh,const rbuf<tword>& v,int begin=0)
	{
		return find_symm_word_e<tword>(v,rsoptr(c_mbk_l),
			rsoptr(c_mbk_r),begin);
	}

	static int find_symm_bbk(const tsh& sh,const rbuf<rstr>& v,int begin=0)
	{
		return find_symm_word_e<rstr>(v,rsoptr(c_bbk_l),
			rsoptr(c_bbk_r),begin);
	}

	static int find_symm_bbk(const tsh& sh,const rbuf<tword>& v,int begin=0)
	{
		return find_symm_word_e<tword>(v,rsoptr(c_bbk_l),
			rsoptr(c_bbk_r),begin);
	}

	static int find_symm_tbk(const tsh& sh,const rbuf<tword>& v,int begin=0)
	{
		return find_symm_word_e<tword>(v,rsoptr(c_tbk_l),
			rsoptr(c_tbk_r),begin);
	}

	//未找到返回结束位置，应该判断count<0，但那样会降低效率
	template<typename T>
	static int find_symm_word_e(const rbuf<T>& v,const rstr& left,const rstr& right,
		int begin=0)
	{
		int end=v.count();
		if(begin<0)
		{
			begin=0;
		}
		int count=0;
		int i;
		for(i=begin;i<end;i++)
		{
			if(left==v[i])
			{
				++count;
			}
			if(right==v[i])
			{
				--count;
			}
			if(0==count)
			{
				return i;
			}
		}
		return v.count();
	}

	static int find_symm_word_rev(const rbuf<tword>& v,const rstr& left,const rstr& right,
		int begin,int end)
	{
		if(end>=v.count())
		{
			end=v.count();
		}
		if(begin<0)
		{
			begin=0;
		}
		int count=0;
		int i;
		for(i=end-1;i>=begin;--i)
		{
			if(left==v[i].val)
			{
				--count;
			}
			if(right==v[i].val)
			{
				++count;
			}
			if(0==count)
			{
				return i;
			}
		}
		return v.count();
	}

	static rbuf<rstr> vword_to_vstr(const rbuf<tword>& v)
	{
		rbuf<rstr> ret(v.count());
		for(int i=0;i<v.count();i++)
		{
			ret[i]=v[i].val;
		}
		return r_move(ret);
	}

	static void push_twi(rbuf<tword>& v,tword& twi,const rstr& val)
	{
		twi.val=val;
		v.push(twi);
	}

	static rstr get_file_name(const tfile* p)
	{
		if(p==null)
		{
			return rstr();
		}
		return p->name.torstr();
	}

	static rstr get_src_str(const tword& word)
	{
		tpos pos=word.pos;//pos_src;
		if(pos.file==null)
		{
			return rstr();
		}
		rstr ret;
		ret+="\nsrc_file: "+pos.file->name.torstr();
		ret+="\nsrc_line: "+rstr(pos.line);
		ret+="\nsrc_cont: "+pos.file->line_list.get(pos.line);
		ret+="\n";
		return r_move(ret);
	}

	static rbool need_arrange(const rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].empty())
			{
				return true;
			}
			elif(v[i].is_multi())
			{
				return true;
			}
		}
		return false;
	}

	//返回是否需要重排
	static rbool arrange(rbuf<tword>& src)
	{
		if(!need_arrange(src))
		{
			return false;
		}
		rbuf<tword> wlist(src.count()*2);
		wlist.m_count=0;
		for(int i=0;i<src.count();i++)
		{
			if(src[i].empty())
			{
				continue;
			}
			elif(src[i].is_multi())
			{
				for(int j=0;j<src[i].multi.count();++j)
				{
					tword word;
					word.val=r_move(src[i].multi[j]);
					word.pos=src[i].pos;
					ifn(word.val.empty())
					{
						wlist.push_move(word);
					}
				}
			}
			else
			{
				wlist.push_move(src[i]);
			}
		}
		src=r_move(wlist);
		return true;
	}

	static rbool part_vsent(tfunc& tfi)
	{
		return part_vsent(tfi.vsent);
	}

	static rbool part_vsent(rbuf<tsent>& vsent)
	{
		ifn(need_part(vsent))
		{
			return false;
		}
		rbuf<tsent> result(vsent.count()*2);
		result.m_count=0;
		for(int i=0;i<vsent.count();++i)
		{
			rbuf<rbuf<tword> > v=r_split_a<tword>(
				vsent[i].vword,tword(rstr(";")));
			if(v.empty())
			{
				continue;
			}
			tsent sent;
			sent.pos=vsent[i].pos;
			for(int j=0;j<v.count();j++)
			{
				sent.vword=r_move(v[j]);
				ifn(sent.vword.empty())
				{
					result.push_move(sent);
				}
			}
		}
		vsent=r_move(result);
		return true;
	}

	static rbool need_part(const rbuf<tsent>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].vword.count()==0)
			{
				return true;
			}
			for(int j=0;j<v[i].vword.count();j++)
			{
				if(v[i].vword[j].val==rstr(";"))
				{
					return true;
				}
			}
		}
		return false;
	}

	static rstr get_rs_dir()
	{
		return rdir::get_prev_dir(rdir::get_exe_dir());
	}

	static void init_path(rbuf<rstr>& vpath)
	{
		vpath+=get_rs_dir()+"src/rlib/";
		vpath+=get_rs_dir()+"src/";
	}

	static void init_match(rbuf<top_node>& result)
	{
		tasm src;
		tasm dst;
		top_node item;

		item.src+=tasm(rf::vstr("sub","esp",",","4"));
		item.src+=tasm(rf::vstr("push","@n"));
		item.src+=tasm(rf::vstr("push","[","@@","+","@n","]"));
		item.src+=tasm(rf::vstr("call","[","&",",","int",",","<=(int,int)","]"));
		item.src+=tasm(rf::vstr("mov","ebx",",","[","esp","]"));
		item.src+=tasm(rf::vstr("add","esp",",","4"));

		item.dst+=tasm(rf::vstr("clesb","[","@1","+","@2","]",",","@0"));
		result+=item;

		item.clear();

		item.src+=tasm(rf::vstr("push","@n"));
		item.src+=tasm(rf::vstr("lea","esi",",","[","@@","+","@n","]"));
		item.src+=tasm(rf::vstr("push","esi"));
		item.src+=tasm(rf::vstr("call","[","&",",","int",",","=(int&,int)","]"));

		item.dst+=tasm(rf::vstr("mov","[","@1","+","@2","]",",","@0"));
		result+=item;

		item.clear();

		item.src+=tasm(rf::vstr("push","[","@@","+","@n","]"));
		item.src+=tasm(rf::vstr("lea","esi",",","[","@@","+","@n","]"));
		item.src+=tasm(rf::vstr("push","esi"));
		item.src+=tasm(rf::vstr("call","[","&",",","int",",","+=(int&,int)","]"));

		item.dst+=tasm(rf::vstr("add","[","@2","+","@3","]",",","[","@0","+","@1","]"));
		result+=item;

		item.clear();

		item.src+=tasm(rf::vstr("lea","esi",",","[","@@","+","@n","]"));
		item.src+=tasm(rf::vstr("push","esi"));
		item.src+=tasm(rf::vstr("call","[","&",",","int",",","++(int&)","]"));

		item.dst+=tasm(rf::vstr("add","[","@0","+","@1","]",",","1"));
		result+=item;

		item.clear();

		item.src+=tasm(rf::vstr("push","[","@@","+","@n","]" ));
		item.src+=tasm(rf::vstr("push","[","@@","+","@n","]" ));
		item.src+=tasm(rf::vstr("call","[","&",",","int",",","+(int,int)","]"));

		item.dst+=tasm(rf::vstr("mov","[","esp","]",",","[","@2","+","@3","]"));
		item.dst+=tasm(rf::vstr("add","[","esp","]",",","[","@0","+","@1","]"));
		result+=item;
	}

	static rstr del_quote(rstr s)
	{
		if(s.get_bottom()==r_char('\"'))
		{
			return s.sub(1,s.count()-1);
		}
		return r_move(s);
	}

	static rstr add_quote(const rstr& s)
	{
		return "\""+s+"\"";
	}

	static int get_word_tab(tword& word)
	{
		tpos pos=word.pos;//pos_src;
		if(pos.file==null)
		{
			rserror("pos file null");
			return -1;
		}
		if(pos.line>=pos.file->tab_list.count())
		{
			rserror("pos file null");
			return -1;
		}
		return pos.file->tab_list[pos.line];
	}

	static rbool is_bbk_prev(const tsh& sh,const rstr& s)
	{
		return (s==rsoptr(c_comma)||s==rsoptr(c_equal)||
			s==rsoptr(c_sbk_l)||s==rsoptr(c_mbk_l)||s==rsoptr(c_bbk_l));
	}

	static rbool is_jmp_ins(int type)
	{
		return type==tkey::c_jmp||type==tkey::c_jebxz||type==tkey::c_jebxnz;
	}

	template<typename T>
	static rbool is_tag(const rbuf<T>& v)
	{
		return v.count()==2&&v[1]==rstr(":");
	}
};
