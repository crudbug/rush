#pragma once

#include "ymatch.h"
#include "yfunctl.h"

//表达式标准化类，返回表达式的类型
struct yexp
{
	//处理函数名调用
	static rbool p_func_call(tsh& sh,const tsent& src,tsent& outopnd,
		tfunc& tfi,int level,int& i,tenv env)
	{
		tclass* ptci=tfi.ptci;
		rstr fname=src.vword[i].val;
		tfunc* ptfi=yfind::func_search(*ptci,fname);
		if(ptfi==null)
		{
			ptci=sh.m_main;
			if(!p_call(sh,src,null,outopnd,tfi,level,i,ptci,env))
			{
				return false;
			}
			return true;
		}
		//函数重载不能友元和非友元共存，友元标志很重要。
		if(ptfi->is_friend)
		{
			if(!p_call(sh,src,null,outopnd,tfi,level,i,ptci,env))
			{
				return false;
			}
		}
		else
		{
			tsent first;
			first.type=ptci->name+rsoptr(c_addr);
			first.vword+=rskey(c_this);
			if(!p_call(sh,src,&first,outopnd,tfi,level,i,ptci,env))
			{
				return false;
			}
		}
		return true;
	}

	//处理[]函数指针直接调用
	static rbool p_point_call(tsh& sh,const tsent& src,tsent& outopnd,
		tfunc& tfi,int level,int& i,tenv env)
	{
		int left=i+1;
		int right=ybase::find_symm_mbk(sh,src.vword,left);
		if(right>=src.vword.count())
		{
			rserror(src,"call miss ]");
			return false;
		}
		outopnd.type=src.vword[i].val;
		outopnd.vword+=rsoptr(c_mbk_l);
		outopnd.vword+=rskey(c_pcall);
		outopnd.vword+=rsoptr(c_comma);
		outopnd.vword+=outopnd.type;
		outopnd.vword+=rsoptr(c_comma);
		outopnd.vword+=rsoptr(c_mbk_l);
		
		rbuf<tsent> vsent;
		ybase::split_param(sh,vsent,src.vword.sub(left+1,right),src);
		if(vsent.empty())
		{
			rserror(src,"call miss func");
			return false;
		}
		for(int j=0;j<vsent.count();j++)
		{
			if(j>=2)
			{
				outopnd.vword+=rsoptr(c_comma);
			}
			if(!p_exp(sh,vsent[j],tfi,level,env))
			{
				return false;
			}
			if(j>=1)
			{
				outopnd.vword+=vsent[j].type;
			}
		}
		outopnd.vword+=rsoptr(c_mbk_r);
		outopnd.vword+=rsoptr(c_comma);
		outopnd.vword+=vsent[0].vword;
		outopnd.vword+=rsoptr(c_comma);

		outopnd.vword+=rsoptr(c_mbk_l);
		for(int j=1;j<vsent.count();j++)
		{
			if(j!=1)
			{
				outopnd.vword+=rsoptr(c_comma);
			}
			outopnd.vword+=r_move(vsent[j].vword);
		}
		outopnd.vword+=rsoptr(c_mbk_r);

		outopnd.vword+=rsoptr(c_mbk_r);
		i=right;
		return true;
	}

	static rbool p_temp_var(tsh& sh,const tsent& src,tsent& outopnd,
		tfunc& tfi,int level,int& i,tenv env)
	{
		int left=i+1;
		int right=ybase::find_symm_sbk(sh,src.vword,left);
		if(right>=src.vword.count())
		{
			rserror(src,"class name miss (");
			return false;
		}
		rbuf<tsent> vsent;
		ybase::split_param(sh,vsent,src.vword.sub(left+1,right),src);
		if(!vsent.empty())
		{
			for(int j=0;j<vsent.count();j++)
			{
				if(!p_exp(sh,vsent[j],tfi,level,env))
				{
					return false;
				}
			}
		}
		outopnd.type=src.vword.get(i).val;
		outopnd.vword+=rsoptr(c_mbk_l);
		outopnd.vword+=rskey(c_btemp);
		outopnd.vword+=rsoptr(c_comma);
		outopnd.vword+=outopnd.type;
		outopnd.vword+=rsoptr(c_comma);
		outopnd.vword+=rsoptr(c_mbk_l);
		for(int j=0;j<vsent.count();j++)
		{
			if(j!=0)
			{
				outopnd.vword+=rsoptr(c_comma);
			}
			outopnd.vword+=r_move(vsent[j].vword);
		}
		outopnd.vword+=rsoptr(c_mbk_r);
		outopnd.vword+=rsoptr(c_mbk_r);
		i=right;
		return true;
	}
	
	//处理类名直接调用
	static rbool p_class_call(tsh& sh,const tsent& src,tsent& outopnd,
		tfunc& tfi,int level,int& i,tenv env)
	{
		if(src.vword.get(i+1).val!=rsoptr(c_dot))
		{
			rserror(src,"class name miss .");
			return false;
		}
		rstr cname=src.vword[i].val;
		tclass* ptci=yfind::class_search(sh,cname);
		if(ptci==null)
		{
			rserror(src,"can't find class name");
			return false;
		}
		i+=2;
		return p_call(sh,src,null,outopnd,tfi,level,i,ptci,env);
	}

	static rbool p_mbk(tsh& sh,const tsent& first,const tsent& second,
		tsent& outopnd,tfunc& tfi,int level,tenv env)
	{
		rbuf<tsent> vsent;
		ybase::split_param(sh,vsent,second.vword,first);
		for(int j=0;j<vsent.count();j++)
		{
			if(!p_exp(sh,vsent[j],tfi,level,env))
			{
				return false;
			}
		}
		vsent.push_front(first);
		tclass* ptci=yfind::class_search_t(sh,first.type);
		if(ptci==null)
		{
			rserror(first,"can't find func mbk");
			return false;
		}
		tfunc* ptfi=ymatch::find_replace(sh,*ptci,"[]",vsent);
		if(null==ptfi)
		{
			rserror(first,"can't find func mbk");
			return false;
		}
		set_func(sh,outopnd,vsent,ptfi);
		return true;
	}

	static rbool p_lisp(tsh& sh,const tsent& src,tsent& outopnd,
		tfunc& tfi,int level,tenv env)
	{
		if(src.vword.get_bottom().val!=rsoptr(c_mbk_l)&&
			src.vword.get_top().val!=rsoptr(c_mbk_r))
		{
			rserror(src,"lisp exp miss");
			return false;
		}
		outopnd.vword.clear();
		outopnd.pos=src.pos;
		tclass* ptci=null;
		tfunc* ptfi=null;
		rstr cname=src.vword.get(1).val;
		rbuf<rbuf<tword> > vlisp;
		get_vlisp(sh,src.vword,vlisp);
		rstr fname=rstr::join<rstr>(ybase::vword_to_vstr(vlisp.get(1)),"");
		ptci=yfind::class_search(sh,cname);
		if(ptci!=null)
		{
			if(vlisp.count()!=3)
			{
				rserror(src,"lisp exp miss");
				return false;
			}
			ptfi=yfind::func_search_dec(*ptci,fname);
			rbuf<tsent> vsent;
			tsent sent;
			sent.pos=src.pos;

			rbuf<rbuf<tword> > vparam;
			get_vlisp(sh,vlisp[2],vparam);
			for(int j=0;j<vparam.count();j++)
			{
				sent.vword=vparam[j];
				if(!p_exp(sh,sent,tfi,level,env))
				{
					return false;
				}
				vsent.push(sent);
			}

			if(ptfi==null)
			{
				ptfi=ymatch::find_replace(sh,*ptci,fname,vsent);
				if(ptfi==null)
				{
					rserror(src,"lisp exp miss");
					return false;
				}
			}
			set_func(sh,outopnd,vsent,ptfi);
			return true;
		}
		if(cname==rskey(c_pcall)||cname==rskey(c_btemp))
		{
			outopnd.vword=src.vword;
			outopnd.type=fname;
			return true;
		}
		if(cname==rsoptr(c_dot))
		{
			tsent sent;
			if(vlisp.count()!=3)
			{
				rserror(src,"lisp exp miss");
				return false;
			}
			sent.vword=vlisp[1];
			sent.pos=src.pos;
			ifn(p_exp(sh,sent,tfi,level,env))
			{
				return false;
			}
			ptci=yfind::class_search_t(sh,sent.type);
			if(ptci==null)
			{
				rserror(src,"lisp exp miss");
				return false;
			}
			tdata* ptdi=yfind::data_member_search(
				*ptci,vlisp[2].get(0).val);
			if(ptdi==null)
			{
				rserror(src,"lisp exp miss");
				return false;
			}
			outopnd.vword=src.vword;
			outopnd.type=ptdi->type;
			return true;
		}
		if(cname==rsoptr(c_addr))
		{
			outopnd.vword=src.vword;
			outopnd.type="rp<void>";
			return true;
		}
		rserror(src,"lisp exp miss");
		return false;
	}

	static void get_vsent(const rbuf<rbuf<tword> >& vlisp,rbuf<tsent>& vsent,
		const tsent& src)
	{
		vsent.clear();
		for(int i=0;i<vlisp.count();i++)
		{
			tsent sent;
			sent.pos=src.pos;
			sent.vword=vlisp[i];
			vsent.push_move(sent);
		}
	}

	static void get_vlisp(const tsh& sh,rbuf<tword> v,rbuf<rbuf<tword> >& vlisp)
	{
		vlisp=ybase::comma_split<tword>(sh,v.sub(1,v.count()-1));
	}
	
	//处理变参中括号调用,变参不能重载
	static rbool p_mbk_param(tsh& sh,const tsent& src,const tsent* pfirst,tsent& outopnd,
		tfunc& tfi,int level,int& i,const tclass* ptci,tenv env)
	{
		rstr cname=ptci->name;
		rstr fname=src.vword[i].val;
		int left=i+1;
		int right;
		right=ybase::find_symm_mbk(sh,src.vword,left);
		if(right>=src.vword.count())
		{
			rserror(src,"call miss )");
			return false;
		}
		tfunc* ptfi=yfind::func_search(*ptci,fname);
		if(null==ptfi)
		{
			rserror(src,"p_mbk_param can't find call");
			return false;
		}
		rbuf<tsent> vsent;
		ybase::split_param(sh,vsent,src.vword.sub(left+1,right),src);
		for(int j=0;j<vsent.count();j++)
		{
			if(!p_exp(sh,vsent[j],tfi,level,env))
			{
				return false;
			}
		}
		tsent tmp;
		tmp.pos=src.pos;
		tmp.vword.push(tword(rstr(vsent.count())));
		tmp.type=rskey(c_int);
		vsent.push_front(tmp);
		//插入first还未处理引用的情况
		if(pfirst!=null)
		{
			vsent.push_front(*pfirst);//插入DOT前的对象
			if(!ybase::is_point(pfirst->type))
			{
				vsent[0].vword.push_front(tword(rsoptr(c_addr)));
				if(!p_exp(sh,vsent[0],tfi,level,env))
				{
					return false;
				}
			}
		}
		tmp.vword.clear();
		tmp.vword+=tword(rsoptr(c_mbk_l));
		tmp.vword+=tword(rsoptr(c_addr));
		tmp.vword+=rsoptr(c_comma);
		tmp.vword+=tword(ptci->name);
		tmp.vword+=rsoptr(c_comma);
		tmp.vword+=tword(ptfi->name_dec);
		tmp.vword+=tword(rsoptr(c_mbk_r));

		outopnd.type=ptfi->retval.type;
		outopnd.vword+=tword(rsoptr(c_mbk_l));
		outopnd.vword+=tword(rskey(c_pcall));
		outopnd.vword+=rsoptr(c_comma);
		outopnd.vword+=tword(outopnd.type);
		outopnd.vword+=rsoptr(c_comma);
		outopnd.vword+=tword(rsoptr(c_mbk_l));
		for(int j=0;j<vsent.count();j++)
		{
			if(j!=0)
			{
				outopnd.vword+=rsoptr(c_comma);
			}
			outopnd.vword+=vsent[j].type;
		}
		outopnd.vword+=tword(rsoptr(c_mbk_r));
		outopnd.vword+=rsoptr(c_comma);
		outopnd.vword+=tmp.vword;
		outopnd.vword+=rsoptr(c_comma);

		outopnd.vword+=tword(rsoptr(c_mbk_l));
		for(int j=0;j<vsent.count();j++)
		{
			if(j!=0)
			{
				outopnd.vword+=rsoptr(c_comma);
			}
			outopnd.vword+=r_move(vsent[j].vword);
		}
		outopnd.vword+=tword(rsoptr(c_mbk_r));

		outopnd.vword+=tword(rsoptr(c_mbk_r));
		i=right;
		return true;
	}
	
	//无括号call
	static rbool p_call_n(tsh& sh,const tsent& src,const tsent* pfirst,tsent& outopnd,
		tfunc& tfi,int level,int& i,const tclass* ptci,tenv env)
	{
		rstr cname=ptci->name;
		rstr fname=src.vword[i].val;
		tfunc* ptfi;
		int left=i;
		int right=left+1;
		if(i+1==src.vword.count()-1||
			yfind::is_end_part_optr(sh,src.vword.get(i+1).val))
		{
			ptfi=yfind::func_search_empty_param_m(
				*ptci,fname,pfirst==null);
			if(ptfi==null)
			{
				rserror(src,"p_call_n can't find call");
				return false;
			}
		}
		else
		{
			ptfi=yfind::func_search_multi_param_m(
				*ptci,fname,pfirst==null);
			if(ptfi==null)
			{
				ptfi=yfind::func_search_empty_param_m(
					*ptci,fname,pfirst==null);
			}
			else
			{
				right=src.vword.count()-1;
			}
			if(ptfi==null)
			{
				rserror(src,"p_call_n2 can't find call");
				return false;
			}
		}
		tsent param=src.sub(left+1,right);
		outopnd.vword.push(tword(cname));
		outopnd.vword.push(tword(rsoptr(c_dot)));
		outopnd.vword.push(tword(fname));
		outopnd.vword.push(tword(rsoptr(c_sbk_l)));
		if(pfirst!=null&&!ptfi->is_friend)
		{
			outopnd.vword+=pfirst->vword;
			if(!param.empty())
			{
				outopnd.vword.push(tword(rsoptr(c_comma)));
				outopnd.vword+=r_move(param.vword);
			}
		}
		else
		{
			if(!param.empty())
			{
				outopnd.vword+=r_move(param.vword);
			}
		}
		outopnd.vword.push(tword(rsoptr(c_sbk_r)));
		//无括号调用添加括号递归处理
		if(!p_exp(sh,outopnd,tfi,level,env))
		{
			return false;
		}
		i=right-1;
		return true;
	}

	static rbool p_ftl(tsh& sh,tsent src,const tsent* pfirst,tsent& outopnd,
		tfunc& tfi,int level,int& i,tclass* ptci,tenv env)
	{
		int left=i+1;
		if(src.vword.get(left)!=rsoptr(c_tbk_l))
		{
			rserror(src,"call miss <");
			return false;
		}
		int right=ybase::find_symm_tbk(sh,src.vword,left);
		if(right>=src.vword.count())
		{
			rserror(src,"call miss >");
			return false;
		}
		rbuf<tword> temp=src.vword.sub(i,right+1);
		if(!yfunctl::ftl_replace(sh,*tfi.ptci,temp,ptci))
		{
			return false;
		}
		src.vword[right]=temp.get_bottom();
		i=right;
		return p_call(sh,src,pfirst,outopnd,tfi,level,i,ptci,env);
	}

	static rbool set_vsent(const tsh& sh,tpos pos,rbuf<tsent>& vsent,
		tfunc& tfi,int level,tenv env)
	{
		tsent sent;
		tword word;
		word.pos=pos;
		sent.pos=pos;
		tdynamic item;
		item.key=tfi.sdynamic.count();
		for(int k=0;k<vsent.count();k++)
		{
			item.vparam+=ybase::vword_to_vstr(vsent[k].vword);
		}
		tfi.sdynamic.insert(item);

		vsent.clear();

		ybase::push_twi(sent.vword,word,rstr((uint)(&tfi.sdynamic.find(item)->vparam)));
		vsent.push(sent);

		sent.vword.clear();
		ybase::push_twi(sent.vword,word,rstr((uint)(&tfi)));
		vsent.push(sent);

		sent.vword.clear();
		ybase::push_twi(sent.vword,word,rstr("_EBP"));
		vsent.push(sent);

		return true;
	}

	static rbool p_call(tsh& sh,const tsent& src,const tsent* pfirst,tsent& outopnd,
		tfunc& tfi,int level,int& i,const tclass* ptci,tenv env)
	{
		rstr cname=ptci->name;
		rstr fname=src.vword[i].val;
		if(src.vword.get(i+1).val==rsoptr(c_mbk_l))
		{
			return p_mbk_param(sh,src,pfirst,outopnd,tfi,level,i,ptci,env);
		}
		if(pfirst!=null&&ybase::get_tname(pfirst->type)==rskey(c_var)&&
			null==yfind::func_search(*ptci,fname))
		{
			return false;
			//a.test  ->   a["test"]
		}
		if(src.vword.get(i+1).val!=rsoptr(c_sbk_l))
		{
			return p_call_n(sh,src,pfirst,outopnd,tfi,level,i,ptci,env);
		}
		int left=i+1;
		int right;
		right=ybase::find_symm_sbk(sh,src.vword,left);
		if(right>=src.vword.count())
		{
			rserror(src,"call miss )");
			return false;
		}
		rbuf<tsent> vsent;
		ybase::split_param(sh,vsent,src.vword.sub(left+1,right),src);
		for(int j=0;j<vsent.count();j++)
		{
			if(!p_exp(sh,vsent[j],tfi,level,env))
			{
				return false;
			}
		}
		tfunc* ptfi=yfind::func_search(*ptci,fname);
		if(ptfi!=null&&ptfi->is_dynamic)
		{
			ifn(set_vsent(sh,src.vword[i].pos,vsent,tfi,level,env))
			{
				return false;
			}
			if(pfirst!=null)
			{
				vsent.push_front(*pfirst);
			}
			set_func(sh,outopnd,vsent,ptfi);
			i=right;
			return true;
		}
		if(pfirst!=null)
		{
			vsent.push_front(*pfirst);//插入DOT前的对象
		}
		ptfi=ymatch::find_replace(sh,*ptci,fname,vsent);
		if(null==ptfi)
		{
			if(pfirst==null)
			{
				rserror(src,"p_call can't find call");
				return false;
			}
			vsent.pop_front();
			ptfi=ymatch::find_replace(sh,*ptci,fname,vsent);
			if(ptfi==null||!ptfi->is_friend)
			{
				rserror(src,"p_call can't find call");
				return false;
			}
		}
		set_func(sh,outopnd,vsent,ptfi);
		i=right;
		return true;
	}

	static rbool p_exp_all(tsh& sh,tfunc& tfi,tenv env)
	{
		for(int i=0;i<tfi.vsent.count();++i)
		{
			if(sh.m_key.is_asm_ins(tfi.vsent[i].vword.get_bottom().val))
			{
				continue;
			}
			if(ybase::is_tag<tword>(tfi.vsent[i].vword))
			{
				continue;
			}
			if(!yexp::p_exp(sh,tfi.vsent[i],tfi,0,env))
			{
				return false;
			}
		}
		return true;
	}

	static rbool p_exp(tsh& sh,rbuf<tword>& v,tfunc& tfi,
		int level,tenv env)
	{
		tsent sent;
		sent.vword=v;
		sent.pos.line=1;
		ifn(p_exp(sh,sent,tfi,level,env))
		{
			return false;
		}
		v=sent.vword;
		return true;
	}

	static rbool p_exp(tsh& sh,tsent& src,tfunc& tfi,
		int level,tenv env)
	{
		return p_exp(sh,src,src,tfi,level,env);
	}
	
	//表达式标准化，并设置dst.type
	static rbool p_exp(tsh& sh,tsent src,tsent& dst,tfunc& tfi,
		int level,tenv env)
	{
		tclass& tci=*tfi.ptci;
		if(level>c_rs_deep)
		{
			rserror(src,"expression too deep");
			return false;
		}
		level++;
		dst.clear();
		rbuf<rstr> soptr;
		rbuf<tsent> sopnd;
		soptr.push(rsoptr(c_sharp_sharp));
		src.vword.push(tword(rsoptr(c_sharp_sharp)));
		for(int i=0;i<src.vword.count();++i)
		{
			if(src.vword[i].val==rsoptr(c_sharp_sharp)&&
				soptr.get_top()==rsoptr(c_sharp_sharp))
			{
				break;
			}
			if(src.vword[i].is_const())
			{
				tsent outopnd;
				outopnd.pos=src.pos;
				outopnd.vword.push(src.vword[i]);
				outopnd.type=ybase::get_ctype(sh,src.vword[i]);
				sopnd.push_move(outopnd);
			}
			elif(sh.m_key.is_asm_reg(src.vword[i].val))
			{
				tsent outopnd;
				outopnd.pos=src.pos;
				outopnd.vword.push(src.vword[i]);
				outopnd.type=rskey(c_int);
				sopnd.push_move(outopnd);
			}
			elif(src.vword[i].val==rsoptr(c_sbk_l))
			{
				//todo 小括号重载有点复杂 
				int right=ybase::find_symm_sbk(sh,src.vword,i);
				if(right>=src.vword.count())
				{
					rserror(src,"miss )");
					return false;
				}
				tsent outopnd;
				outopnd.pos=src.pos;
				if(!p_exp(sh,src.sub(i+1,right),outopnd,tfi,level,env))
				{
					return false;
				}
				sopnd.push_move(outopnd);
				i=right;
			}
			elif(src.vword.get(i+1)==rsoptr(c_mbk_l)&&
				yfind::is_class_t(sh,src.vword[i].val))
			{
				//[]函数指针调用
				tsent outopnd;
				outopnd.pos=src.pos;
				if(!p_point_call(sh,src,outopnd,tfi,level,i,env))
				{
					return false;
				}
				sopnd.push_move(outopnd);
			}
			elif(src.vword[i].val==rsoptr(c_mbk_l))
			{
				int right=ybase::find_symm_mbk(sh,src.vword,i);
				if(right>=src.vword.count())
				{
					rserror(src,"miss ]");
					return false;
				}
				tsent outopnd;
				outopnd.pos=src.pos;
				if(sopnd.empty()||
					src.vword.get(i+1).val==rsoptr(c_addr)&&
					yfind::is_class(sh,src.vword.get(i+3).val))
				{
					if(!p_lisp(sh,src.sub(
						i,right+1),outopnd,tfi,level,env))
					{
						return false;
					}
				}
				else
				{
					if(!p_mbk(sh,sopnd.pop(),src.sub(i+1,right),
						outopnd,tfi,level,env))
					{
						return false;
					}
				}
				sopnd.push_move(outopnd);
				i=right;
			}
			elif(src.vword[i].val==rsoptr(c_dot))
			{
				if(sopnd.empty())
				{
					rserror(src,". miss opnd");
					return false;
				}
				i++;
				rstr name=src.vword.get(i).val;
				tsent first=sopnd.pop();
				tclass* ptci=yfind::class_search_t(sh,first.type);
				if(null==ptci)
				{
					rserror(src,". can't find class name");
					return false;
				}
				tsent outopnd;
				outopnd.pos=src.pos;
				//数据成员和函数成员不可以同名
				tdata* ptdi=yfind::data_member_search(*ptci,name);
				if(ptdi!=null)
				{
					outopnd.type=ptdi->type;
					outopnd.vword+=tword(rsoptr(c_mbk_l));
					outopnd.vword+=tword(rsoptr(c_dot));
					outopnd.vword+=rsoptr(c_comma);
					outopnd.vword+=first.vword;
					outopnd.vword+=rsoptr(c_comma);
					outopnd.vword+=tword(name);
					outopnd.vword+=tword(rsoptr(c_mbk_r));
					sopnd.push(outopnd);
					continue;
				}
				if(src.vword.get(i+1)==rsoptr(c_tbk_l)&&
					yfind::ftl_search(*ptci,name)!=null)
				{
					if(!p_ftl(sh,src,&first,
						outopnd,tfi,level,i,ptci,env))
					{
						return false;
					}
				}
				else
				{
					if(!p_call(sh,src,&first,
						outopnd,tfi,level,i,ptci,env))
					{
						return false;
					}
				}
				sopnd.push_move(outopnd);
			}
			elif(src.vword[i].val==rsoptr(c_arrow_r))
			{
				if(sopnd.empty())
				{
					rserror(src,". miss opnd");
					return false;
				}
				tsent first=sopnd.pop();
				first.vword.push_front(tword(rsoptr(c_mul)));
				if(!p_exp(sh,first,tfi,level,env))
				{
					return false;
				}
				sopnd.push_move(first);
				src.vword[i].val=rsoptr(c_dot);
				i--;
			}
			elif(sh.m_optr.is_optr(src.vword[i].val))
			{
				if(soptr.empty())
				{
					rserror(src,"exp miss optr");
					return false;
				}
				rstr cur=src.vword[i].val;
				if(!sh.m_optr.precede(soptr.top(),cur))
				{
					soptr.push(cur);
					continue;
				}
				rstr theta=soptr.pop();
				if(sopnd.empty())
				{
					rserror(src,"exp miss opnd");
					return false;
				}
				rbuf<tsent> vsent;
				vsent.push(sopnd.pop());
				tclass* ptci=yfind::class_search_t(sh,vsent[0].type);
				tfunc* ptfi;
				tsent outopnd;
				outopnd.pos=src.pos;
				if(ptci!=null)
				{
					ptfi=ymatch::find_replace(sh,
						*ptci,theta,vsent);
					if(ptfi!=null)
					{
						set_func(sh,outopnd,
							vsent,ptfi);
						sopnd.push_move(outopnd);
						i--;
						continue;
					}
				}
				if(sopnd.empty())
				{
					rserror(src,"exp miss opnd");
					return false;
				}
				vsent.push_front(sopnd.pop());
				ptci=yfind::class_search_t(sh,vsent[0].type);
				if(ptci!=null)
				{
					ptfi=ymatch::find_replace(sh,
						*ptci,theta,vsent);
					if(ptfi!=null)
					{
						set_func(sh,outopnd,
							vsent,ptfi);
						sopnd.push_move(outopnd);
						i--;
						continue;
					}
				}
				ptci=yfind::class_search_t(sh,vsent[1].type);
				if(ptci!=null)
				{
					ptfi=ymatch::find_replace(sh,
						*ptci,theta,vsent);
					if(ptfi!=null)
					{
						set_func(sh,outopnd,
							vsent,ptfi);
						sopnd.push_move(outopnd);
						i--;
						continue;
					}
				}
				rserror(src,"exp miss opnd 2");
				return false;
			}
			elif(yfind::is_class(sh,src.vword[i].val))
			{
				tsent outopnd;
				outopnd.pos=src.pos;
				if(src.vword.get(i+1).val==rsoptr(c_sbk_l))
				{
					//临时变量
					if(!p_temp_var(sh,src,outopnd,tfi,level,i,env))
					{
						return false;
					}
				}
				else
				{
					//类名直接调用
					if(!p_class_call(sh,src,outopnd,tfi,level,i,env))
					{
						return false;
					}
				}
				sopnd.push_move(outopnd);
			}
			else
			{
				rstr name=src.vword[i].val;
				tsent outopnd;
				outopnd.pos=src.pos;
				tdata* ptdi=yfind::local_search(tfi,name);
				if(ptdi!=null)
				{
					outopnd.vword+=src.vword[i];
					outopnd.type=ptdi->type;
					sopnd.push_move(outopnd);
					continue;
				}
				if(!tci.is_friend)
				{
					ptdi=yfind::data_member_search(tci,name);
					if(ptdi!=null)
					{
						//类成员变量增加this
						//如 m_a -> this.m_a 
						outopnd.vword+=tword(rsoptr(c_mbk_l));
						outopnd.vword+=tword(rsoptr(c_dot));
						outopnd.vword+=rsoptr(c_comma);
						outopnd.vword+=tword(rskey(c_this));
						outopnd.vword+=rsoptr(c_comma);
						outopnd.vword+=src.vword[i];
						outopnd.vword+=tword(rsoptr(c_mbk_r));
						outopnd.type=ptdi->type;
						sopnd.push_move(outopnd);
						continue;
					}
				}
				ptdi=yfind::data_member_search(*sh.m_main,name);
				if(ptdi!=null)
				{
					//全局变量
					outopnd.vword+=tword(rsoptr(c_mbk_l));
					outopnd.vword+=tword(rsoptr(c_dot));
					outopnd.vword+=rsoptr(c_comma);
					outopnd.vword+=tword(rskey(c_pmain));
					outopnd.vword+=rsoptr(c_comma);
					outopnd.vword+=src.vword[i];
					outopnd.vword+=tword(rsoptr(c_mbk_r));
					outopnd.type=ptdi->type;
					sopnd.push_move(outopnd);
					continue;
				}
				if(env.ptfi!=null)
				{
					ptdi=yfind::local_search(*env.ptfi,name);
					if(ptdi!=null)
					{
						//环境变量
						outopnd.vword+=tword(rsoptr(c_mbk_l));
						outopnd.vword+=tword(rsoptr(c_dot));
						outopnd.vword+=rsoptr(c_comma);
						outopnd.vword+=tword(rskey(c_penv));
						outopnd.vword+=rsoptr(c_comma);
						outopnd.vword+=src.vword[i];
						outopnd.vword+=tword(rsoptr(c_mbk_r));
						outopnd.type=ptdi->type;
						sopnd.push_move(outopnd);
						continue;
					}
				}
				if(!p_func_call(sh,src,outopnd,tfi,level,i,env))
				{
					return false;
				}
				sopnd.push_move(outopnd);
			}
		}
		if(sopnd.count()!=1)
		{
			ybase::print_vword(src.vword);
			rserror(src,"expression error");
			return false;
		}
		dst=r_move(sopnd[0]);
		if(dst.vword.empty()||dst.type.empty())
		{
			rserror(src,"expression error");
			return false;
		}
		return true;
	}

	static void set_func(const tsh& sh,tsent& outopnd,const rbuf<tsent>& vsent,const tfunc* ptfi)
	{
		const tclass* ptci=ptfi->ptci;
		outopnd.type=ptfi->retval.type;
		outopnd.vword+=tword(rsoptr(c_mbk_l));
		outopnd.vword+=tword(ptci->name);
		outopnd.vword+=rsoptr(c_comma);
		outopnd.vword+=tword(ptfi->name_dec);
		outopnd.vword+=rsoptr(c_comma);
		outopnd.vword+=tword(rsoptr(c_mbk_l));
		for(int i=0;i<vsent.count();i++)
		{
			if(i!=0)
			{
				outopnd.vword+=rsoptr(c_comma);
			}
			outopnd.vword+=r_move(vsent[i].vword);
		}
		outopnd.vword+=tword(rsoptr(c_mbk_r));
		outopnd.vword+=tword(rsoptr(c_mbk_r));
	}
};
