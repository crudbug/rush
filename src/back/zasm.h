#pragma once

#include "../front/yexp.h"
#include "zopt.h"

//生成汇编代码
//tfunc.vsent->vasm
//返回指针或引用或int的表达式需要放入ebx中
//指针或int的引用是对本身的值进行判断
//其它类型的引用是对引用进行判断
struct zasm
{
	static rbool process(tsh& sh,tfunc& tfi)
	{
		for(int i=0;i<tfi.vsent.count();i++)
		{
			rbuf<tasm> vasm;
			ifn(proc_ret(sh,tfi.vsent[i],vasm,tfi))
			{
				rserror(tfi.vsent[i],"build asm error");
				return false;
			}
			if(tconf::c_op_match)
			{
				yopt::op_match(sh,vasm);
			}
			if(tconf::c_op_merge)
			{
				yopt::op_add_sub(sh,vasm);
			}
			for(int j=0;j<vasm.count();j++)
			{
				vasm[j].pos=tfi.vsent[i].pos;
			}
			tfi.vasm+=vasm;
		}
		add_asm_ebp(sh,tfi);
		return true;
	}

	static void push_double(const tsh& sh,rbuf<tasm>& vasm,double dval)
	{
		vasm+=rf::vstr(rskey(c_push),rstr(*((uint*)(&dval)+1)));
		vasm+=rf::vstr(rskey(c_push),rstr(*((uint*)(&dval))));
	}

	static rbool add_asm_ebp(const tsh& sh,tfunc& tfi)
	{
		rbuf<tasm> vasm;
		tasm item;
		item.pos=tfi.pos;
		int size=yfind::get_func_local_size(tfi);
		ifn(tconf::c_op_zero&&size==0)
		{
			item.vstr=rf::vstr(rskey(c_sub),rskey(c_esp),
				rsoptr(c_comma),rstr(size));
			vasm+=item;
		}
		rbool b_use_ebp=use_ebp(sh,tfi.vasm);
		rbool protect_ebx=(tfi.name.get_left()==r_char('~')&&
			!yfind::is_empty_struct_type(sh,tfi.ptci->name)&&
			!tfi.vsent.empty()||tfi.is_cfunc);
		if(b_use_ebp)
		{
			item.vstr=rf::vstr(rskey(c_push),rskey(c_ebp));
			vasm+=item;
			item.vstr=rf::vstr(rskey(c_mov),rskey(c_ebp),
				rsoptr(c_comma),rskey(c_esp));
			vasm+=item;
		}
		if(protect_ebx)
		{
			item.vstr=rf::vstr(rskey(c_push),rskey(c_ebx));
			vasm+=item;
		}
		if(tfi.is_cfunc)
		{
			//todo 如果ebx、esi、edi未修改则不用保护
			item.vstr=rf::vstr(rskey(c_push),rskey(c_esi));
			vasm+=item;
			item.vstr=rf::vstr(rskey(c_push),rskey(c_edi));
			vasm+=item;
		}
		vasm+=tfi.vasm;
		item.pos=tfi.pos;
		if(tfi.is_cfunc)
		{
			//todo: change to one line
			item.vstr.clear();
			item.vstr.push(rskey(c_pop));
			item.vstr.push(rskey(c_edi));
			vasm.push(item);

			item.vstr.clear();
			item.vstr.push(rskey(c_pop));
			item.vstr.push(rskey(c_esi));
			vasm.push(item);
		}
		if(protect_ebx)
		{
			item.vstr.clear();
			item.vstr.push(rskey(c_pop));
			item.vstr.push(rskey(c_ebx));
			vasm.push(item);
		}
		if(b_use_ebp)
		{
			item.vstr.clear();
			item.vstr.push(rskey(c_pop));
			item.vstr.push(rskey(c_ebp));
			vasm.push(item);
		}
		ifn(tconf::c_op_zero&&size==0)
		{
			item.vstr.clear();
			item.vstr.push(rskey(c_add));
			item.vstr.push(rskey(c_esp));
			item.vstr.push(rsoptr(c_comma));
			item.vstr.push(rstr(size));
			vasm.push(item);
		}
		size=yfind::get_func_param_size(tfi);
		if(size>0)
		{
			item.vstr.clear();
			item.vstr.push(rskey(c_reti));
			item.vstr.push(rstr(size));
			vasm.push(item);
		}
		else
		{
			item.vstr.clear();
			item.vstr.push(rskey(c_retn));
			vasm.push(item);
		}
		tfi.vasm=r_move(vasm);
		return true;
	}

	static rbool use_ebp(const tsh& sh,const rbuf<tasm>& vasm)
	{
		if(!tconf::c_op_ebp)
		{
			return true;
		}
		for(int i=0;i<vasm.count();i++)
		{
			for(int j=0;j<vasm[i].vstr.count();j++)
			{
				if(vasm[i].vstr[j]==rskey(c_ebp))
				{
					return true;
				}
			}
		}
		return false;
	}

	static rbool proc_ret(tsh& sh,const tsent& src,rbuf<tasm>& vasm,
		tfunc& tfi,int level=0)
	{
		rstr name=src.vword.get_bottom().val;
		if(sh.m_key.is_asm_ins(name)||ybase::is_tag<tword>(src.vword))
		{
			vasm+=ybase::vword_to_vstr(src.vword);
			return true;
		}
		//返回常量
		if(src.vword.count()==1)
		{
			if(src.vword[0].is_cint())
			{
				//经测试如果是小程序rskey(c_mov)比"mov"效率高
				vasm+=rf::vstr(rskey(c_mov),rskey(c_ebx),
					rsoptr(c_comma),src.vword[0].val);
				return true;
			}
			elif(sh.m_key.is_asm_reg(src.vword[0].val))
			{
				vasm+=rf::vstr(rskey(c_mov),rskey(c_ebx),
					rsoptr(c_comma),src.vword[0].val);
				return true;
			}
			elif(src.vword[0].is_cpoint())
			{
				if(src.vword[0].is_caddr())
				{
					vasm+=rf::vstr(rskey(c_mov),
						rskey(c_ebx),rsoptr(c_comma),
						src.vword[0].val);
				}
				else
				{
					vasm+=rf::vstr(rskey(c_mov),
						rskey(c_ebx),rsoptr(c_comma),
						src.vword[0].val.sub_trim(1));
				}
				return true;
			}
		}
		if(src.vword.count()==7&&src.vword[1].val==rsoptr(c_addr))
		{
			vasm+=rf::vstr(rskey(c_mov),rskey(c_ebx),rsoptr(c_comma),
				rsoptr(c_mbk_l),rsoptr(c_addr),rsoptr(c_comma),
				src.vword[3].val,rsoptr(c_comma),src.vword[5].val,
				rsoptr(c_mbk_r));
			return true;
		}
		//返回变量
		tdata* ptdi=yfind::local_search(tfi,
			get_src_in(sh,src).vword.get(0).val);
		if(ptdi!=null)
		{
			if(yfind::is_type_mebx(sh,src.type))
			{
				vasm+=rf::vstr(rskey(c_lea),rskey(c_esi),
					rsoptr(c_comma),rsoptr(c_mbk_l),
					rskey(c_ebp),rsoptr(c_plus),
					rstr(ptdi->off),rsoptr(c_mbk_r));
				//这里还可以优化
				if(!add_esi(sh,ptdi->type,vasm,src))
				{
					rserror("");
					return false;
				}
				vasm+=rf::vstr(rskey(c_mov),rskey(c_ebx),
					rsoptr(c_comma),rsoptr(c_mbk_l),
					rskey(c_esi),rsoptr(c_mbk_r));
				if(ybase::is_quote(src.type)&&
					(ybase::get_tname(src.type)==rskey(c_int)||
					ybase::is_point(ybase::get_tname(src.type))))
				{
					vasm+=rf::vstr(rskey(c_mov),rskey(c_ebx),
						rsoptr(c_comma),rsoptr(c_mbk_l),
						rskey(c_ebx),rsoptr(c_mbk_r));
				}
			}
			return true;
		}
		//返回函数
		return proc_ret_func(sh,src,vasm,tfi,level);
	}

	static rbool proc_ret_func(tsh& sh,const tsent& src,rbuf<tasm>& vasm,
		tfunc& tfi,int level)
	{
		tdata retval;
		if(!a_exp(sh,get_src_in(sh,src),vasm,retval,tfi,level))
		{
			return false;
		}
		int size;
		//多数函数返回void，因此加上这句判断可以提高效率
		if(src.type!=rskey(c_void)&&yfind::is_type_mebx(sh,src.type))
		{
			vasm+=rf::vstr(rskey(c_mov),rskey(c_esi),
				rsoptr(c_comma),rskey(c_esp));
			int cur=vasm.count();
			if(!add_esi(sh,retval.type,vasm,src))
			{
				rserror("");
				return false;
			}
			if(cur==vasm.count())
			{
				vasm.pop();
				vasm+=rf::vstr(rskey(c_mov),rskey(c_ebx),
					rsoptr(c_comma),rsoptr(c_mbk_l),
					rskey(c_esp),rsoptr(c_mbk_r));
			}
			else
			{
				vasm+=rf::vstr(rskey(c_mov),rskey(c_ebx),
					rsoptr(c_comma),rsoptr(c_mbk_l),
					rskey(c_esi),rsoptr(c_mbk_r));
			}
			if(ybase::is_quote(src.type)&&
				(ybase::get_tname(src.type)==rskey(c_int)||
				ybase::is_point(ybase::get_tname(src.type))))
			{
				vasm+=rf::vstr(rskey(c_mov),rskey(c_ebx),
					rsoptr(c_comma),rsoptr(c_mbk_l),
					rskey(c_ebx),rsoptr(c_mbk_r));
			}
		}
		if(!destruct_ret(sh,retval,vasm))
		{
			rserror("");
			return false;
		}
		size=yfind::get_ceil_space(retval);
		vasm+=rf::vstr(rskey(c_add),rskey(c_esp),rsoptr(c_comma),size);
		return true;
	}
	
	//解析表达式生成汇编代码
	static rbool a_exp(tsh& sh,const tsent& src,rbuf<tasm>& vasm,tdata& retval,
		tfunc& tfi,int level=0)
	{
		if(level>c_rs_deep)
		{
			rserror("");
			return false;
		}
		level++;
		if(src.vword.get_bottom().val!=rsoptr(c_mbk_l)||
			src.vword.get_top().val!=rsoptr(c_mbk_r))
		{
			rserror("");
			return false;
		}
		if(src.vword.get(1).val==rsoptr(c_dot))
		{
			rserror("");
			return false;
		}
		int size;
		rbuf<rbuf<tword> > vlisp;
		yexp::get_vlisp(sh,src.vword,vlisp);
		if(src.vword.get(1).val==rskey(c_pcall))
		{
			if(vlisp.count()!=5)
			{
				rserror("");
				return false;
			}
			retval.type=vlisp[1].get(0).val;
			retval.size=yfind::get_type_size(sh,retval.type);
			size=yfind::get_ceil_space(retval);
			vasm+=rf::vstr(rskey(c_sub),rskey(c_esp),
				rsoptr(c_comma),size);
			rbuf<rbuf<tword> > vparam;
			yexp::get_vlisp(sh,vlisp[4],vparam);
			rbuf<tsent> vsent;
			yexp::get_vsent(vparam,vsent,src);
			for(int i=vsent.count()-1;i>=0;i--)
			{
				//todo:已有类型数组无需重新获取
				if(!yexp::p_exp(sh,vsent[i],tfi,level,tenv()))
				{
					return false;
				}
			}
			for(int i=vsent.count()-1;i>=0;i--)
			{
				tdata tdi;
				tdi.type=ybase::get_tname(vsent[i].type);
				tdi.size=yfind::get_type_size(sh,tdi.type);
				if(!pass_param(sh,vsent[i],tdi,vasm,tfi,level))
				{
					return false;
				}
			}
			tsent sent=src;
			sent.vword=vlisp[3];
			if(!yexp::p_exp(sh,sent,tfi,level,tenv()))
			{
				return false;
			}
			if(!proc_ret(sh,sent,vasm,tfi,level))
			{
				return false;
			}
			if(sent.type==rstr("rp<void>"))
			{
				vasm+=rf::vstr(rskey(c_call),rskey(c_ebx));
			}
			else
			{
				rserror("");
				return false;
			}
			return true;
		}
		if(vlisp.count()!=3)
		{
			rserror("");
			return false;
		}
		yexp::get_vlisp(sh,vlisp[2],vlisp);
		rbuf<tsent> vsent;
		yexp::get_vsent(vlisp,vsent,src);
		for(int i=vsent.count()-1;i>=0;i--)
		{
			if(!yexp::p_exp(sh,vsent[i],tfi,level,tenv()))
			{
				return false;
			}
		}
		rstr cname=src.vword.get(1).val;
		rstr fname=src.vword.get(3).val;//p_exp已经合并了name_dec
		tclass* ptci=yfind::class_search(sh,cname);
		if(null==ptci)
		{
			rserror("");
			return false;
		}
		tfunc* ptfi=yfind::func_search_dec(*ptci,fname);
		if(ptfi==null)
		{
			rserror("");
			return false;
		}
		retval=ptfi->retval;
		if(tconf::c_op_empty_func&&
			yfind::is_empty_struct_type(sh,ptci->name)&&
			(yfind::is_destruct(sh,*ptfi)||
			yfind::is_emptystruct(sh,*ptfi)))
		{
			return true;
		}
		if(ptfi->param.count()!=vsent.count())
		{
			rserror("");
			return false;
		}
		size=yfind::get_ceil_space(retval);
		vasm+=rf::vstr(rskey(c_sub),rskey(c_esp),rsoptr(c_comma),size);
		for(int i=vsent.count()-1;i>=0;i--)
		{
			if(!pass_param(sh,vsent[i],ptfi->param[i],vasm,tfi,level))
			{
				return false;
			}
		}
		vasm+=ybase::get_func_declare_call(sh,*ptci,*ptfi);
		return true;
	}

	static rbool pass_param(tsh& sh,const tsent& src,const tdata& dst,rbuf<tasm>& vasm,
		tfunc& tfi,int level)
	{
		if(src.vword.empty())
		{
			rserror("");
			return false;
		}
		int size;
		tsent src_in=get_src_in(sh,src);
		if(src_in.vword.get(1).val==rskey(c_pcall)||
			yfind::is_class(sh,src_in.vword.get(1).val))
		{
			//先申请参数空间
			size=yfind::get_ceil_space(dst);
			vasm+=rf::vstr(rskey(c_sub),rskey(c_esp),
				rsoptr(c_comma),size);
			//递归处理子表达式
			tdata retval;
			if(!a_exp(sh,src_in,vasm,retval,tfi,level))
			{
				return false;
			}
			//获取传递参数的地址分别放入esi和edi中，
			//包括函数返回后再dot，如int.get().m_in
			if(!obtain_var_addr_f(sh,retval,src,vasm))
			{
				rserror("");
				return false;
			}
			//传递参数
			if(!copy_param(sh,src.type,dst.type,vasm))
			{
				rserror("");
				return false;
			}
			//析构返回值
			if(!destruct_ret(sh,retval,vasm))
			{
				rserror("");
				return false;
			}
			//释放栈
			size=yfind::get_ceil_space(retval);
			vasm+=rf::vstr(rskey(c_add),rskey(c_esp),
				rsoptr(c_comma),size);
		}
		elif(src.vword[0].is_cint())
		{
			vasm+=rf::vstr(rskey(c_push),src.vword[0].val);
		}
		elif(src.vword[0].is_cuint())
		{
			vasm+=rf::vstr(rskey(c_push),src.vword[0].val.sub_trim(1));
		}
		elif(src.vword[0].is_cdouble())
		{
			double dval=src.vword[0].val.todouble();
			push_double(sh,vasm,dval);
		}
		elif(src.vword.count()==7&&
			src.vword.get(1).val==rsoptr(c_addr)&&
			src.vword[0].val==rsoptr(c_mbk_l))
		{
			rbuf<rstr> vstr=ybase::vword_to_vstr(src.vword);
			vstr.push_front(rskey(c_push));
			vasm+=vstr;
		}
		elif(src.vword[0].is_cpoint())
		{
			if(src.vword.get_bottom().is_caddr())
			{
				vasm+=rf::vstr(rskey(c_push),src.vword[0].val);
			}
			else
			{
				vasm+=rf::vstr(rskey(c_push),src.vword[0].val.sub_trim(1));
			}
		}
		elif(src.vword[0].is_cstr())
		{
			vasm+=rf::vstr(rskey(c_push),src.vword[0].val);
		}
		elif(sh.m_key.is_asm_reg(src.vword[0].val))
		{
			vasm+=rf::vstr(rskey(c_push),src.vword[0].val);
		}
		else
		{
			rstr name=src_in.vword.get(0).val;
			tdata* ptdi=yfind::local_search(tfi,name);
			if(ptdi==null)
			{
				tfi.name.printl();
				rserror(name);
				return false;
			}
			int cur=vasm.count()+3;
			if(!obtain_var_addr_var(sh,src,dst,ptdi,vasm))
			{
				rserror("");
				return false;
			}
			if(cur==vasm.count()&&tconf::c_op_pass)
			{
				if(yfind::is_op_pass_type(sh,dst.type)&&
					dst.type==src.type)
				{
					vasm.m_count-=3;
					vasm+=rf::vstr(rskey(c_push),rsoptr(c_mbk_l),
						rskey(c_ebp),rsoptr(c_plus),
						rstr(ptdi->off),rsoptr(c_mbk_r));
					return true;
				}
				if(ybase::is_quote(dst.type)&&ybase::is_quote(src.type))
				{
					vasm.m_count-=3;
					vasm+=rf::vstr(rskey(c_push),rsoptr(c_mbk_l),
						rskey(c_ebp),rsoptr(c_plus),
						rstr(ptdi->off),rsoptr(c_mbk_r));
					return true;
				}
				if(ybase::is_quote(dst.type))
				{
					vasm.m_count-=3;
					vasm+=rf::vstr(rskey(c_lea),rskey(c_esi),
						rsoptr(c_comma),rsoptr(c_mbk_l),
						rskey(c_ebp),rsoptr(c_plus),
						rstr(ptdi->off),rsoptr(c_mbk_r));
					vasm+=rf::vstr(rskey(c_push),rskey(c_esi));
					return true;
				}
			}
			//调用拷贝构造函数时还可以优化
			return copy_param(sh,src.type,dst.type,vasm);
		}
		return true;
	}

	static rbool obtain_var_addr_var(const tsh& sh,const tsent& src,const tdata& dst,
		const tdata* ptdi,rbuf<tasm>& vasm)
	{
		int size=yfind::get_ceil_space(dst);
		vasm+=rf::vstr(rskey(c_sub),rskey(c_esp),rsoptr(c_comma),size);
		vasm+=rf::vstr(rskey(c_mov),rskey(c_edi),rsoptr(c_comma),
			rskey(c_esp));
		vasm+=rf::vstr(rskey(c_lea),rskey(c_esi),rsoptr(c_comma),
			rsoptr(c_mbk_l),rskey(c_ebp),rsoptr(c_plus),
			rstr(ptdi->off),rsoptr(c_mbk_r));
		return add_esi(sh,ptdi->type,vasm,src);
	}

	static rbool obtain_var_addr_f(const tsh& sh,tdata& retval,
		const tsent& src,rbuf<tasm>& vasm)
	{
		int size=yfind::get_ceil_space(retval);
		vasm+=rf::vstr(rskey(c_lea),rskey(c_edi),rsoptr(c_comma),
			rsoptr(c_mbk_l),rskey(c_esp),rsoptr(c_plus),
			rstr(size),rsoptr(c_mbk_r));
		vasm+=rf::vstr(rskey(c_mov),rskey(c_esi),rsoptr(c_comma),
			rskey(c_esp));
		return add_esi(sh,retval.type,vasm,src);
	}

	static tsent get_src_in(const tsh& sh,const tsent& src)
	{
		if(src.vword.get(1).val!=rsoptr(c_dot))
		{
			return src;
		}
		tsent sent=src;
		int start=0;
		while(start+2<src.vword.count()&&
			  src.vword[start].val==rsoptr(c_mbk_l)&&
			  src.vword[start+1].val==rsoptr(c_dot))
		{
			start+=3;
		}
		if(src.vword[start].val==rsoptr(c_mbk_l))
		{
			int right=ybase::find_symm_mbk(sh,src.vword,start);
			sent.vword=src.vword.sub(start,right+1);
		}
		else
		{
			sent.vword=src.vword.sub(start,start+1);
		}
		return r_move(sent);
	}
	
	// [ . , [ . , [ . , a , b ] , c ] , d ]
	static rbool add_esi(const tsh& sh,rstr type,rbuf<tasm>& vasm,const tsent& src)
	{
		int start=0;
		const rbuf<tword>& v=src.vword;
		if(v.get(1).val!=rsoptr(c_dot))
		{
			return true;
		}
		while(start+2<v.count()&&
			v[start].val==rsoptr(c_mbk_l)&&
			v[start+1].val==rsoptr(c_dot))
		{
			start+=3;
		}
		int right=start+2;
		if(v[start].val==rsoptr(c_mbk_l))
		{
			right=ybase::find_symm_mbk(sh,v,start);
			if(right>=v.count())
			{
				return false;
			}
			right+=2;
		}
		for(int i=0;i<start;i+=3)
		{
			tclass* ptci=yfind::class_search_t(sh,type);
			if(null==ptci)
			{
				return false;
			}
			tdata* ptdi=yfind::data_member_search(*ptci,v.get(right).val);
			if(null==ptdi)
			{
				return false;
			}
			if(ybase::is_quote(type))
			{
				vasm+=rf::vstr(rskey(c_mov),rskey(c_esi),
					rsoptr(c_comma),rsoptr(c_mbk_l),
					rskey(c_esi),rsoptr(c_mbk_r));
			}
			if(ptdi->off!=0)
			{
				vasm+=rf::vstr(rskey(c_add),rskey(c_esi),
					rsoptr(c_comma),ptdi->off);
			}
			type=ptdi->type;
			right+=3;
		}
		return true;
	}

	//src的真实地址已经存储在esi中,dst的地址在edi中
	static rbool copy_param(const tsh& sh,const rstr& src,
		const rstr& dst,rbuf<tasm>& vasm)
	{
		//指针可以随意转换
		if(ybase::is_point(dst)&&ybase::is_point(src)||
			dst==rskey(c_rd32)&&yfind::get_type_size(sh,src)==4)
		{
			vasm+=rf::vstr(rskey(c_mov),rsoptr(c_mbk_l),
				rskey(c_edi),rsoptr(c_mbk_r),
				rsoptr(c_comma),rsoptr(c_mbk_l),
				rskey(c_esi),rsoptr(c_mbk_r));
			return true;
		}
		//目标是引用
		if(ybase::is_quote(dst))
		{
			if(ybase::is_quote(src))
			{
				//源是引用
				vasm+=rf::vstr(rskey(c_mov),rsoptr(c_mbk_l),
					rskey(c_edi),rsoptr(c_mbk_r),
					rsoptr(c_comma),rsoptr(c_mbk_l),
					rskey(c_esi),rsoptr(c_mbk_r));
			}				
			else
			{
				//源是对象
				vasm+=rf::vstr(rskey(c_mov),rsoptr(c_mbk_l),
					rskey(c_edi),rsoptr(c_mbk_r),
					rsoptr(c_comma),rskey(c_esi));
			}
			return true;
		}
		if(tconf::c_op_pass)
		{
			if(yfind::is_op_pass_type(sh,dst))
			{
				if(ybase::is_quote(src))
				{
					//源是引用
					vasm+=rf::vstr(rskey(c_mov),rskey(c_esi),
						rsoptr(c_comma),rsoptr(c_mbk_l),
						rskey(c_esi),rsoptr(c_mbk_r));
				}
				vasm+=rf::vstr(rskey(c_mov),rsoptr(c_mbk_l),
					rskey(c_edi),rsoptr(c_mbk_r),
					rsoptr(c_comma),rsoptr(c_mbk_l),
					rskey(c_esi),rsoptr(c_mbk_r));
				return true;
			}
			if(dst==rskey(c_double)||dst==rskey(c_int64))
			{
				if(ybase::is_quote(src))
				{
					vasm+=rf::vstr(rskey(c_mov),rskey(c_esi),
						rsoptr(c_comma),rsoptr(c_mbk_l),
						rskey(c_esi),rsoptr(c_mbk_r));
				}
				vasm+=rf::vstr(rskey(c_mov),rsoptr(c_mbk_l),
					rskey(c_edi),rsoptr(c_mbk_r),
					rsoptr(c_comma),rsoptr(c_mbk_l),
					rskey(c_esi),rsoptr(c_mbk_r));
				vasm+=rf::vstr(rskey(c_mov),rsoptr(c_mbk_l),
					rskey(c_edi),rsoptr(c_plus),"4",
					rsoptr(c_mbk_r),rsoptr(c_comma),
					rsoptr(c_mbk_l),rskey(c_esi),
					rsoptr(c_plus),"4",rsoptr(c_mbk_r));
				return true;
			}
		}
		//目标是对象需要调用拷贝构造函数
		if(ybase::is_quote(src))
		{
			//源是引用
			vasm+=rf::vstr(rskey(c_push),rsoptr(c_mbk_l),
				rskey(c_esi),rsoptr(c_mbk_r));
		}
		else
		{
			//源是对象
			vasm+=rf::vstr(rskey(c_push),rskey(c_esi));
		}
		vasm+=rf::vstr(rskey(c_push),rskey(c_edi));
		tclass* ptci=yfind::class_search_t(sh,src);
		if(ptci==null)
		{
			return false;
		}
		tfunc* pcopystruct=yfind::copystruct_search(*ptci);
		if(pcopystruct==null)
		{
			return false;
		}
		vasm+=ybase::get_func_declare_call(sh,*ptci,*pcopystruct);
		return true;
	}

	static rbool destruct_ret(const tsh& sh,const tdata& retval,rbuf<tasm>& vasm)
	{
		rstr type=retval.type;
		if(ybase::is_quote(type))
		{
			return true;
		}
		if(type==rskey(c_void))
		{
			return true;
		}
		tclass* ptci=yfind::class_search(sh,type);
		if(ptci==null)
		{
			return false;
		}
		tfunc* pdestruct=yfind::destruct_search(*ptci);
		if(pdestruct==null)
		{
			return false;
		}
		if(tconf::c_op_empty_func&&yfind::is_empty_struct_type(sh,type))
		{
			return true;
		}
		vasm+=rf::vstr(rskey(c_mov),rskey(c_esi),
			rsoptr(c_comma),rskey(c_esp));
		vasm+=rf::vstr(rskey(c_push),rskey(c_esi));
		vasm+=ybase::get_func_declare_call(sh,*ptci,*pdestruct);
		return true;
	}
};
