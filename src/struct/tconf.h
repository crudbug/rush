#pragma once

#include "../rlib/rdic.h"

struct tconf
{
	enum
	{
		c_auto_addinit=1,
		c_auto_addfunc=1,//自动增加构造析构等函数
		c_auto_import=1,
		c_op_empty_func=1,//优化基本数据类型空构造析构函数开关
		c_op_nop=1,
		c_op_zero=1,//优化add esp,0
		c_op_merge=1,//同一行连续加减同一个寄存器合并为一条指令
		c_op_ebp=1,//如果本函数未使用ebp可以不用push ebp节省3条指令
		c_op_base_calc=1,//优化基础运算，如调用int.opreator+可变成一条add指令
		c_op_pass=1,//优化参数传递，如传递int不需要调用拷贝构造函数
		c_op_const_eval=1,//常量表达式求值
		c_op_match=1,//汇编模式匹配优化
		c_run=1,
		c_stack_size=(2*1024*1024),
	};
};

struct toptr
{
	rbuf<rstr> m_optr;//编译器使用
	rbuf<rstr> m_optr_s;//排好序的运算符，因为词法分析是贪心的
	rdic<int> m_dic;//字典中存放的是优先级，值小的优先
	rdic<int> m_num;
	int m_optr_max;

	enum
	{
		c_sbk_l,
		c_sbk_r,//()小括号
		c_mbk_l,
		c_mbk_r,//[]中括号
		c_bbk_l,
		c_bbk_r,//{}大括号
		
		c_arrow_r,//->右箭头
		c_dot,//.
		c_domain,//::作用域运算符
		c_semi,//;分号
		c_colon,//:冒号
		c_comma,//,
		c_sharp,//预处理符号#
		c_sharp_sharp,//表达式起止符号,使用一个根本不会出现的符号
		c_at,
		c_at_at,

		c_destruct,//~
		c_addr,//&
		c_not,//!
		c_plusplus,//++
		c_minusminus,//--
		c_negative,
		c_dollar,
		
		c_mul,//*
		c_divide,//除
		c_modulo,
		
		c_plus,//+
		c_minus,//-
		
		c_less_equal,
		c_great_equal,
		c_equal_equal,
		c_not_equal,

		c_tbk_l,
		c_tbk_r,//<>尖括号

		c_and,
		c_or,
		
		c_arrow_l,//<-
		c_exchange,//<->

		c_equal,//=
		c_temp_94,//+=
		c_temp_95,//-=
		c_temp_96,//*=
		c_temp_97,// /=
		c_temp_98,// %=
		c_temp_99,// =>
		c_less_great,
	};

	toptr()
	{
		m_optr_max=0;
#ifndef _RS
		char* optr_arr[]={
#else
		optr_arr=array<char*>[
#endif
			"(","0","0",
			")","0","0",
			"[","0","0",
			"]","0","0",
			"{","0","0",
			"}","0","0",
			"->","0","2",
			".","0","2",
			"::","0","2",
			";","0","0",
			":","0","0",
			",","0","0",
			"#","0","0",
			"##","10","0",
			"@","0","0",
			"@@","0","0",

			"~","1","1",
			"&","1","1",
			"!","1","1",
			"++","1","1",
			"--","1","1",
			"neg","1","1",//todo:运算符不能用字母
			"$","0","1",

			"*","2","2",
			"/","2","2",
			"%","2","2",

			"+","3","2",
			"-","3","2",

			"<=","4","2",
			">=","4","2",
			"==","4","2",
			"!=","4","2",

			"<","4","2",
			">","4","2",

			"&&","5","2",
			"||","6","2",

			"<-","9","2",
			"<->","9","2",

			"=","9","2",
			"+=","9","2",
			"-=","9","2",
			"*=","9","2",
			"/=","9","2",
			"%=","9","2",
			"=>","9","2",
			"<>","4","2",
#ifndef _RS
		};
#else
		];
#endif
#ifndef _RS
		for(int i=0;i<r_size(optr_arr)/r_size(char*);i+=3)
		{
			m_optr+=optr_arr[i];
			m_dic[optr_arr[i]]=rstr(optr_arr[i+1]).toint();
			m_num[optr_arr[i]]=rstr(optr_arr[i+2]).toint();
		}
#else
		for(int i=0;i<optr_arr.count();i+=3)
		{
			m_optr+=optr_arr[i];
			m_dic[optr_arr[i]]=rstr(optr_arr[i+1]).toint();
			m_num[optr_arr[i]]=rstr(optr_arr[i+2]).toint();
		}
#endif
		sort_optr();
	}

	rstr& operator[](int n) const
	{
		return m_optr[n];
	}

	void sort_optr()
	{
		m_optr_s=m_optr;
		r_qsort<rstr>(m_optr_s);
		m_optr_max=0;
		for(int i=0;i<m_optr_s.count();i++)
		{
			if(m_optr_s[i].count()>m_optr_max)
			{
				m_optr_max=m_optr_s[i].count();
			}
		}
	}

	int get_grade(const rstr& s) const
	{
		int* p=m_dic.find(s);
		if(p==null)
		{
			return 0;
		}
		return *p;
	}
	
	//比较两个运算符的优先级，true表示第一个优先于第二个，相同优先级总是从左到右计算
	rbool precede(const rstr& first,const rstr& second) const
	{
		return get_grade(first)<=get_grade(second);
	}

	rbool is_optr(const rstr& s) const
	{
		return r_find_b<rstr>(m_optr_s,s)<m_optr_s.count();
	}
};

struct tkey
{
	rbuf<rstr> m_key;
	rdic<int> m_dic;

	enum
	{
		c_rjit,

		c_calle,
		c_call,
		c_retn,
		c_reti,

		c_push,
		c_pop,

		c_jmp,
		c_jebxz,//如果ebx等于0则跳转
		c_jebxnz,

		c_halt,
		c_nop,

		c_lea,
		c_mov,
		c_movb,
		c_movl,

		c_add,
		c_sub,
		c_imul,
		c_idiv,
		c_imod,

		c_cesb,
		c_cnesb,
		c_cgsb,//cmp great set ebx带符号设置指令，大于则置1，否则置0
		c_cgesb,
		c_clsb,
		c_clesb,

		c_band,
		c_bor,
		c_bnot,
		c_bxor,
		c_bshl,
		c_bshr,
		c_bsar,

		c_udiv,
		c_umod,
		c_ucgsb,
		c_ucgesb,
		c_uclsb,
		c_uclesb,

		c_rn,

		c_rd8,
		c_rd16,
		c_rd32,
		c_rd64,
		c_rdp,

		c_eip,
		c_esp,
		c_ebp,
		c_eax,
		c_ebx,
		c_ecx,
		c_edx,
		c_esi,
		c_edi,
		c_eflags,

		c_void,
		c_char,
		c_int,
		c_uint,
		c_int64,
		c_double,
		c_rp,
		c_var,
		c_rbuf,
		c_rstr,

		c_js_func,//JS函数
		c_js_obj,
		c_js_findf,

		c_if,
		c_ifn,//与unless功能一样,感觉ifnot或者ifn更清晰
		c_else,
		c_break,
		c_continue,
		c_continued,
		c_return,
		c_class,
		c_template,
		c_this,
		c_sizeof,
		c_main,
		c_include,
		c_import,
		c_endif,
		c_ifdef,
		c_ifndef,
		c_define,
		c_enum,
		c_for,
		c_to,
		c_in,
		c_case,
		c_friend,
		c_cfunc,
		c_dynamic,
		c_private,
		c_protected,
		c_public,
		c_switch,
		c_typename,
		c_typeof,
		c_s_ret,
		c_s_off,
		c_s_param,
		c_s_local,
		c_mac,//宏
		c_export,
		c_lambda,
		c_self,
		c_extern,
		c_btemp,//构造临时对象
		c_pcall,//函数指针调用
		c_LAMBDA,
		c_SELF,
		c_rvm,
		c_rvara,
		c_rvarb,
		c_temp,
		c_ltemp,
		c_plambda,
		c_pmain,//全局数据的引用
		c_penv,//环境指针
	};

	tkey()
	{
#ifndef _RS
		char* key_arr[]={
#else
		key_arr=array<char*>[
#endif
			"rjit",

			"calle",
			"call",
			"retn",
			"reti",

			"push",
			"pop",

			"jmp",
			"jebxz",
			"jebxnz",

			"halt",
			"nop",

			"lea",
			"mov",
			"movb",
			"movl",

			"add",
			"sub",
			"imul",
			"idiv",
			"imod",

			"cesb",
			"cnesb",
			"cgsb",
			"cgesb",
			"clsb",
			"clesb",

			"band",
			"bor",
			"bnot",
			"bxor",
			"bshl",
			"bshr",
			"bsar",

			"_udiv",
			"_umod",
			"_ucgsb",
			"_ucgesb",
			"_uclsb",
			"_uclesb",

			"rn",

			"rd8",
			"rd16",
			"rd32",
			"rd64",
			"rdp",

			"eip",
			"esp",
			"ebp",
			"eax",
			"ebx",
			"ecx",
			"edx",
			"esi",
			"edi",
			"eflags",

			"void",
			"char",
			"int",
			"uint",
			"int64",
			"double",
			"rp",
			"var",
			"rbuf",
			"rstr",

			"function",
			"_js_obj",
			"_js_findf",

			"if",
			"ifn",
			"else",
			"break",
			"continue",
			"continued",
			"return",
			"class",
			"template",
			"this",
			"sizeof",
			"main",
			"include",
			"import",
			"endif",
			"ifdef",
			"ifndef",
			"define",
			"enum",
			"for",
			"to",
			"in",
			"case",
			"friend",
			"cfunc",
			"dynamic",
			"private",
			"protected",
			"public",
			"switch",
			"typename",
			"typeof",
			"s_ret",
			"s_off",
			"s_param",
			"s_local",
			"mac",
			"export",
			"lambda",
			"self",
			"extern",
			"_BTEMP",
			"_PCALL",
			"_LAMBDA",
			"_SELF",
			"_RVM",
			"_RVARA",
			"_RVARB",
			"_TEMP",
			"_LTEMP",
			"_LAMBDA_C_P",
			"_PMAIN",
			"_PENV",
#ifndef _RS
		};
#else
		];
#endif
#ifndef _RS
		for(int i=0;i<r_size(key_arr)/r_size(char*);i++)
		{
			m_key+=key_arr[i];
		}
#else
		for(int i=0;i<key_arr.count();i++)
		{
			m_key+=key_arr[i];
		}
#endif
		sort_asm();
	}

	rstr& operator[](int n) const
	{
		return m_key[n];
	}

	int count() const
	{
		return m_key.count();
	}

	void sort_asm()
	{
		m_dic.clear();
		for(int i=0;i<m_key.count();i++)
		{
			m_dic[m_key[i]]=i;
		}
	}

	int get_key_index(const rstr& s) const
	{
		int* p=m_dic.find(s);
		if(p==null)
		{
			return m_key.count();
		}
		return *p;
	}

	rbool is_asm_ins(const rstr& s) const
	{
		int* p=m_dic.find(s);
		if(p==null)
		{
			return false;
		}
		int index=*p;
		return index>=0&&index<=c_rn;
	}

	rbool is_asm_reg(const rstr& s) const
	{
		for(int i=c_eip;i<=c_edi;++i)
		{
			if(s==m_key[i])
			{
				return true;
			}
		}
		return false;
	}
};
