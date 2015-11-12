#pragma once

#include "ylambda.h"
#include "ycontrol.h"
#include "ymac.h"
#include "yrep.h"
#include "ylazy.h"

//单词转换为语句，控制结构线性化，运算符替换成标准函数调用的S表达式
//即[ class func []]形式，例如[ rf print []]
//从这里开始行号已经放在sent中
struct ysent
{
	static rbool process(tsh& sh,tfunc& tfi,tenv env)
	{
		if(env.ptfi!=null)
		{
			add_class(sh,tfi,env);
		}
		ifn(ylambda::function_replace(sh,tfi.vword))
		{
			return false;
		}
		ylambda::lambda_var_replace(sh,tfi);
		ifn(ylambda::lambda_replace(sh,tfi))
		{
			return false;
		}
		//强制转换替换
		ifn(yrep::trans_replace(sh,tfi.vword))
		{
			return false;
		}
		ifn(macro_replace(sh,tfi))
		{
			return false;
		}
		if(!ymac::replace(sh,*tfi.ptci,tfi.vword))
		{
			return false;
		}
		if(!yfunctl::ftl_replace(sh,*tfi.ptci,tfi.vword,null))
		{
			return false;
		}
		if(!ycontrol::proc_func(sh,tfi))
		{
			return false;
		}
		ifn(ymac::func_mac_replace(sh,tfi.vsent))
		{
			return false;
		}
		ylambda::lambda_proc(sh,tfi);
		//增加全局变量引用
		add_main_quote(sh,tfi);
		yrep::replace_neg(sh,tfi.vsent);
		//常量dot替换成临时变量
		//如 0.toint -> int(0).toint
		// "abc"+a  ->  rstr("abc")+a
		yrep::const_replace(sh,tfi.vsent);
		//找到直接定义的局部变量
		yrep::local_var_replace(sh,tfi);
		//函数指针常量替换
		yrep::fpoint_replace(sh,tfi);
		//sizeof s_off替换成0，稍后处理
		if(!yrep::size_off_to_zero(sh,tfi))
		{
			return false;
		}
		//进行类型推断
		if(!proc_type_infer(sh,tfi,env))
		{
			return false;
		}
		if(!yrep::typeof_replace(sh,tfi,env))
		{
			return false;
		}
		//变量构造函数替换如a(1) -> int.int(a,1)
		//这个变量必须是已定义的，不能是类型推断
		if(!yrep::var_struct_replace(sh,tfi))
		{
			return false;
		}
		//表达式标准化
		if(!yexp::p_exp_all(sh,tfi,env))
		{
			return false;
		}
		ifn(ylazy::extact_and_or(sh,tfi,tfi.tid,env))
		{
			return false;
		}
		ifn(ylazy::replace_and_or(sh,tfi))
		{
			return false;
		}
		//临时变量替换，替换后有些表达式类型会变空
		if(!replace_temp_var(sh,tfi,tfi.tid))
		{
			return false;
		}
		ifn(tfi.sdynamic.empty())
		{
			tdata tdi;
			tdi.type=rskey(c_int);
			tdi.name="_EBP";
			tfi.local+=tdi;

			tsent sent;
			sent.pos=tfi.pos;
			sent.vword.push(tword(rstr("mov")));//tword没有设置pos
			sent.vword.push(tword(rstr("_EBP")));//todo:
			sent.vword.push(tword(rstr(",")));
			sent.vword.push(tword(rstr("ebp")));
			tfi.vsent.push_front(r_move(sent));
		}
		if(env.ptfi!=null)
		{
			tsent sent;
			sent.pos=tfi.pos;
			sent.vword.push(tword(rstr("mov")));//tword没有设置pos
			sent.vword.push(tword(rstr("_PENV")));
			sent.vword.push(tword(rstr(",")));
			sent.vword.push(tword(rstr(env.v_ebp)));
			tfi.vsent.push_front(r_move(sent));
		}
		
		//增加局部变量和成员变量的构造和析构
		yadd::add_local_and_memb(sh,tfi);
		//获取局部变量偏移
		obtain_local_off(sh,tfi.local);
		obtain_param_off(tfi);
		//增加全局变量初始化汇编
		add_main_init_asm(sh,tfi);
		ylambda::lambda_add_init_asm(sh,tfi);
		//sizeof s_off替换成实际值
		if(!yrep::size_off_to_zero(sh,tfi))
		{
			return false;
		}
		if(!yrep::size_off_replace(sh,tfi))
		{
			return false;
		}
		//汇编语句常量求值
		if(!asm_const_eval(sh,tfi))
		{
			return false;
		}
		if(!ymac::replace(sh,*tfi.ptci,tfi.vword))
		{
			return false;
		}
		//成员变量里有sizeof s_off的情况需要再次替换
		//注意成员变量初始化的时候不能使用临时变量
		//再处理一次，获取所有表达式的类型
		return yexp::p_exp_all(sh,tfi,tenv());
	}

	static rbool macro_replace(tsh& sh,tfunc& tfi)
	{
		extern rbool r_func_to_x86(tsh& sh,tfunc& tfi,tenv env);
		rbuf<tword>& v=tfi.vword;
		for(int i=0;i<v.count();i++)
		{
			if(sh.m_macro.exist(v[i].val))
			{
				tfunc* ptfi=sh.m_macro[v[i].val];
				ifn(r_func_to_x86(sh,*ptfi,tenv()))
				{
					return false;
				}
#ifdef _MSC_VER
				((void (__stdcall *)(rbuf<tword>&,int))(ptfi->code))(v,i);
#endif
			}
		}
		ybase::arrange(v);
		return true;
	}

	static void add_class(tsh& sh,tfunc& tfi,tenv env)
	{
		rstr name="_func_class_"+env.ptfi->name_dec;
		if(!yfind::is_class(sh,name))
		{
			tclass item;
			item.name=name;
			for(int i=0;i<env.ptfi->local.count();i++)
			{
				item.vdata.push(env.ptfi->local[i]);
			}
			for(int i=0;i<env.ptfi->param.count();i++)
			{
				item.vdata.push(env.ptfi->param[i]);
			}
			sh.m_class.insert(item);
		}

		tdata tdi;
		tdi.type=name+"&";
		tdi.name=rskey(c_penv);
		tdi.size=4;
		tdi.count=1;
		tfi.local.push(tdi);
	}

	//增加全局变量引用的初始化汇编语句
	static void add_main_init_asm(tsh& sh,tfunc& tfi)
	{
		if(sh.m_mode==tsh::c_gpp)
		{
			return;
		}
		if(tfi.name!="main")
		{
			int i;
			for(i=0;i<tfi.local.count();i++)
			{
				if(tfi.local[i].type=="main&"&&
					tfi.local[i].name==rskey(c_pmain))
				{
					break;
				}
			}
			if(i>=tfi.local.count())
			{
				return;
			}
		}
		tsent sent;
		sent.pos=tfi.pos;
		sent.vword.push(tword(rstr("mov")));//tword没有设置pos
		sent.vword.push(tword(rskey(c_pmain)));
		sent.vword.push(tword(rstr(",")));//todo:
		if(sh.m_mode==tsh::c_vm||sh.m_mode==tsh::c_jit)
		{
			sent.vword.push(tword(rstr((uint)(sh.m_main_data.begin()))));
		}
		else
		{
			sent.vword.push(tword(rstr("_PMAIN_A")));
		}
		tfi.vsent.push_front(r_move(sent));
	}

	//全局变量和成员变量都不能使用类型推断
	//全局变量必须用g_开头
	static void add_main_quote(tsh& sh,tfunc& tfi)
	{
		ifn(tfi.ptci->name==rskey(c_main)&&tfi.name==rskey(c_main))
		{
			int i;
			for(i=0;i<tfi.vword.count();i++)
			{
				if(yfind::data_member_search(
					*sh.m_main,tfi.vword[i].val)!=null)
				{
					break;
				}
			}
			if(i>=tfi.vword.count())
			{
				return;
			}
		}
		tdata tdi;
		tdi.type="main&";
		tdi.name=rskey(c_pmain);
		tdi.size=4;
		tdi.count=1;
		tfi.local.push(tdi);
	}

	static rbool asm_const_eval(tsh& sh,tfunc& tfi)
	{
		for(int i=0;i<tfi.vsent.count();++i)
		{
			if(sh.m_key.is_asm_ins(tfi.vsent[i].vword.get_bottom().val))
			{
				ifn(yconsteval::op_const_exp(sh,tfi.vsent[i].vword,true))
				{
					return false;
				}
			}
		}
		return true;
	}

	static void obtain_local_off(tsh& sh,rbuf<tdata>& local)
	{
		int off=c_point_size;
		for(int i=0;i<local.count();++i)
		{
			local[i].size=yfind::get_type_size(sh,local[i].type);
			local[i].off=off;
			off+=yfind::get_ceil_space(local[i]);
		}
	}

	static void obtain_param_off(tfunc& tfi)
	{
		int off=yfind::get_func_local_size(tfi);
		off+=c_point_size;//ebp占用的
		off+=c_point_size;//返回地址占用的
		for(int i=0;i<tfi.param.count();++i)
		{
			tfi.param[i].off=off;
			off+=yfind::get_ceil_space(tfi.param[i]);
		}
		tfi.retval.off=off;
	}

	static rbool proc_type_infer(tsh& sh,tfunc& tfi,tenv env)
	{
		for(int i=0;i<tfi.vsent.count();++i)
		{
			if(!proc_type_infer(sh,tfi.vsent[i],tfi,env))
			{
				return false;
			}
		}
		ybase::part_vsent(tfi);
		return true;
	}

	static rbool proc_type_infer(tsh& sh,tsent& sent,tfunc& tfi,tenv env)
	{
		tclass& tci=*tfi.ptci;
		if(sent.vword.count()>=3&&
			sent.vword[1].val==rsoptr(c_equal)&&
			sent.vword[0].is_name())
		{
			//处理类型推断
			rstr name=sent.vword[0].val;
			if(null!=yfind::local_search(tfi,name))
			{
				return true;
			}
			if(env.ptfi!=null&&null!=yfind::local_search(*env.ptfi,name))
			{
				return true;
			}
			if(null!=yfind::data_member_search(tci,name))
			{
				return true;
			}
			if(null!=yfind::data_member_search(*sh.m_main,name))
			{
				return true;
			}
			tsent temp=sent.sub(2,sent.vword.count());
			if(!yrep::typeof_replace(sh,tfi,temp,env))
			{
				return false;
			}
			if(!yexp::p_exp(sh,temp,tfi,0,env))
			{
				return false;
			}
			tdata tdi;
			tdi.name=name;
			tdi.type=ybase::get_tname(temp.type);//引用推断为对象
			
			tfi.local.push(tdi);
			rbuf<tword> vtemp;
			yadd::add_destructor_func(sh,tci,tdi,vtemp,false);
			yadd::add_structor_func(sh,tdi,vtemp);
			vtemp+=r_move(sent.vword);
			sent.vword=r_move(vtemp);
		}
		return true;
	}

	static rbool replace_temp_var_v(tsh& sh,tfunc& tfi,rbuf<tword>& v,int& tid)
	{
		for(int i=1;i<v.count();i++)
		{
			if(v[i].val!=rskey(c_btemp))
			{
				continue;
			}
			tclass* ptci=yfind::class_search(sh,v.get(i+2).val);
			if(null==ptci)
			{
				continue;
			}
			int left=i-1;
			if(v[left].val!=rsoptr(c_mbk_l))
			{
				continue;
			}
			int right=ybase::find_symm_mbk(sh,v,left);
			if(right>=v.count())
			{
				return false;
			}
			tdata tdi;
			tdi.type=ptci->name;
			tdi.name=rskey(c_temp)+rstr(tid);
			tid++;
			rbuf<tword> result;
			result+=rsoptr(c_mbk_l);
			result+=tdi.type;
			result+=rsoptr(c_comma);
			result+=tdi.type;
			result+=rsoptr(c_comma);
			result+=rsoptr(c_mbk_l);
			result+=tdi.name;
			rbuf<tword> vparam=v.sub(left+6,right-1);
			ifn(vparam.empty())
			{
				result+=rsoptr(c_comma);
				result+=vparam;
			}
			result+=rsoptr(c_mbk_r);
			result+=rsoptr(c_mbk_r);
			result+=rsoptr(c_semi);
			tfi.local.push(tdi);
			for(int j=left;j<=right;j++)
			{
				v[j].clear();
			}
			v[left].val=tdi.name;
			ybase::arrange(v);
			result+=r_move(v);
			result+=rsoptr(c_semi);
			//这里加入析构可能破坏ebx，所以所有的析构函数都要保护ebx
			yadd::add_destructor_func(sh,*tfi.ptci,tdi,result,false);
			v=r_move(result);
			return true;
		}
		return true;
	}

	static rbool replace_temp_var(tsh& sh,tfunc& tfi,int& tid)
	{
		for(int i=0;i<tfi.vsent.count();++i)
		{
			if(sh.m_key.is_asm_ins(tfi.vsent[i].vword.get_bottom().val))
			{
				continue;
			}
			for(int j=0;j<c_rs_deep;j++)//最多150层嵌套
			{
				//刚好是先构造内层的临时变量，再构造外层的临时变量
				//这种循环替换的方式好像比递归更清晰
				int temp=tid;
				if(!replace_temp_var_v(sh,tfi,tfi.vsent[i].vword,tid))
				{
					rserror(tfi.vsent[i],"replace_temp_var_v");
					return false;
				}
				if(temp==tid)
				{
					break;
				}
			}
		}
		ybase::part_vsent(tfi);
		return true;
	}
};
