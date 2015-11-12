#pragma once

#include "yexp.h"

//惰性求值（短路求值）
struct ylazy
{
	static rbool is_and_or_name(const rstr& s)
	{
		return s=="&&(int,int)"||s=="||(int,int)";
	}

	static rbool exist_and_or(rbuf<tsent>& vsent)
	{
		for(int i=0;i<vsent.count();i++)
		{
			for(int j=0;j<vsent[i].vword.count();j++)
			{
				if(is_and_or_name(vsent[i].vword[j].val))
				{
					return true;
				}
			}
		}
		return false;
	}

	static rbool part_and_or_one(tsh& sh,tfunc& tfi,
		int& tid,rbuf<tsent>& vsent,tsent& sent,tenv env)
	{
		rbuf<tword>& v=sent.vword;
		if(sh.m_key.is_asm_ins(v.get_bottom().val))
		{
			vsent.push(sent);
			return true;
		}
		for(int i=0;i<v.count();i++)
		{
			//[int,&&(int,int),[1,2]]
			ifn(is_and_or_name(v[i].val))
			{
				continue;
			}
			if(i==3)
			{
				continue;
			}
			tclass* ptci=yfind::class_search(sh,v.get(i-2).val);
			if(null==ptci)
			{
				continue;
			}
			int left=i-3;
			if(v.get(left).val!=rsoptr(c_mbk_l))
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
			tdi.name=rskey(c_ltemp)+rstr(tid);
			tid++;
			tfi.local.push(tdi);

			tsent temp;
			temp.type=rskey(c_int);
			temp.vword=v.sub(left,right+1);
			temp.pos=sent.pos;
			vsent+=temp;

			temp.type=rskey(c_void);
			temp.vword.clear();
			temp.vword+=tdi.name;
			temp.vword+=rsoptr(c_equal);
			temp.vword+=rskey(c_ebx);
			ifn(yexp::p_exp(sh,temp,tfi,0,env))
			{
				return false;
			}
			vsent+=temp;

			ybase::clear_word_val(v,left,right+1);
			v[left].val=tdi.name;
			ybase::arrange(v);
			ifn(yexp::p_exp(sh,sent,tfi,0,env))
			{
				return false;
			}
			vsent+=sent;

			return true;
		}
		vsent.push(sent);
		return true;
	}

	static rbool part_and_or(tsh& sh,tfunc& tfi,int& tid,tenv env)
	{
		rbuf<tsent> v;
		for(int i=0;i<tfi.vsent.count();i++)
		{
			ifn(part_and_or_one(sh,tfi,tid,v,tfi.vsent[i],env))
			{
				return false;
			}
		}
		tfi.vsent=r_move(v);
		return true;
	}

	static rbool extact_and_or(tsh& sh,tfunc& tfi,int& tid,tenv env)
	{
		ifn(exist_and_or(tfi.vsent))
		{
			return true;
		}
		for(int i=0;i<c_rs_deep;i++)
		{
			int cur=tfi.vsent.count();
			ifn(part_and_or(sh,tfi,tid,env))
			{
				rserror(tfi.vsent[i],"replace_and_or");
				return false;
			}
			if(cur==tfi.vsent.count())
			{
				return true;
			}
		}
		return false;
	}

	static rbool replace_and_or(tsh& sh,tfunc& tfi)
	{
		for(int i=0;i<tfi.vsent.count();i++)
		{
			ifn(replace_and_one(sh,tfi.vsent[i].vword,tfi.id))
			{
				return false;
			}
			ifn(replace_or_one(sh,tfi.vsent[i].vword,tfi.id))
			{
				return false;
			}
		}
		ybase::part_vsent(tfi.vsent);
		return true;
	}

	static rbool replace_and_one(tsh& sh,rbuf<tword>& v,int& id)
	{
		if(v.get(3)!=rstr("&&(int,int)"))
		{
			return true;
		}
		if(v.get_left()!=rsoptr(c_mbk_l))
		{
			return false;
		}
		if(v.get_right()!=rsoptr(c_mbk_r))
		{
			return false;
		}
		rbuf<rbuf<tword> > param=ybase::comma_split<tword>(sh,v.sub(6,v.count()-2));
		if(param.count()!=2)
		{
			return false;
		}
		rbuf<tword> result;
		result+=param[0];
		result+=rsoptr(c_semi);
		result+=rskey(c_jebxz);
		result+=rstr("_and_false_")+id;
		result+=rsoptr(c_semi);
		result+=param[1];
		result+=rsoptr(c_semi);
		result+=rstr("cnesb");
		result+=rstr("ebx");
		result+=rstr(",");
		result+=rstr("0");
		result+=rsoptr(c_semi);
		result+=rskey(c_jmp);
		result+=rstr("_and_end_")+(id+1);
		result+=rsoptr(c_semi);
		result+=rstr("_and_false_")+id;
		result+=rstr(":");
		result+=rsoptr(c_semi);
		result+=rstr("mov");
		result+=rstr("ebx");
		result+=rstr(",");
		result+=rstr("0");
		result+=rsoptr(c_semi);
		result+=rstr("_and_end_")+(id+1);
		result+=rstr(":");
		result+=rsoptr(c_semi);
		id+=2;
		v=r_move(result);
		return true;
	}

	static rbool replace_or_one(tsh& sh,rbuf<tword>& v,int& id)
	{
		if(v.get(3)!=rstr("||(int,int)"))
		{
			return true;
		}
		if(v.get_left()!=rsoptr(c_mbk_l))
		{
			return false;
		}
		if(v.get_right()!=rsoptr(c_mbk_r))
		{
			return false;
		}
		rbuf<rbuf<tword> > param=ybase::comma_split<tword>(sh,v.sub(6,v.count()-2));
		if(param.count()!=2)
		{
			return false;
		}
		rbuf<tword> result;
		result+=param[0];
		result+=rsoptr(c_semi);
		result+=rskey(c_jebxnz);
		result+=rstr("_or_true_")+id;
		result+=rsoptr(c_semi);
		result+=param[1];
		result+=rsoptr(c_semi);
		result+=rstr("cnesb");
		result+=rstr("ebx");
		result+=rstr(",");
		result+=rstr("0");
		result+=rsoptr(c_semi);
		result+=rskey(c_jmp);
		result+=rstr("_or_end_")+(id+1);
		result+=rsoptr(c_semi);
		result+=rstr("_or_true_")+id;
		result+=rstr(":");
		result+=rsoptr(c_semi);
		result+=rstr("mov");
		result+=rstr("ebx");
		result+=rstr(",");
		result+=rstr("1");
		result+=rsoptr(c_semi);
		result+=rstr("_or_end_")+(id+1);
		result+=rstr(":");
		result+=rsoptr(c_semi);
		id+=2;
		v=r_move(result);
		return true;
	}
};
