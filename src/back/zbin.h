#pragma once

#include "../front/ysent.h"
#include "../front/yword.h"
#include "zasm.h"

//生成二进制代码类
struct zbin
{
	//将一个函数翻译成二进制代码
	static rbool process(tsh& sh,tfunc& tfi)
	{
		if(!tfi.vasm.empty())
		{
			return true;
		}
		if(!cp_vword_to_vasm(sh,tfi,tenv()))
		{
			return false;
		}
		for(int i=0;i<tfi.vasm.count();i++)
		{
			if(!proc_asm(sh,tfi.vasm,tfi.vasm[i]))
			{
				rserror(tfi.vasm[i],"asm error");
				return false;
			}
			tfi.vasm[i].ptfi=&tfi;
		}
		return true;
	}

	//从函数的词表编译到vasm
	static rbool cp_vword_to_vasm(tsh& sh,tfunc& tfi,tenv env)
	{
		ifn(ysent::process(sh,tfi,env))
		{
			rserror(tfi,"sent error");
			return false;
		}
		ifn(zasm::process(sh,tfi))
		{
			rserror(tfi,"asm error");
			return false;
		}
		return true;
	}

	static rbool proc_vasm(tsh& sh,rbuf<tasm>& vasm)
	{
		for(int i=0;i<vasm.count();i++)
		{
			if(!proc_asm(sh,vasm[i]))
			{
				return false;
			}
		}
		return true;
	}

	static rbool proc_asm(tsh& sh,tasm& oasm)
	{
		rbuf<tasm> vasm;
		return proc_asm(sh,vasm,oasm);
	}
	
	//将汇编代码翻译成二进制代码
	static rbool proc_asm(tsh& sh,rbuf<tasm>& vasm,tasm& oasm)
	{
		if(ybase::is_tag<rstr>(oasm.vstr))
		{
			oasm.ins.type=tins::c_nop_n;
			return true;
		}
		oasm.ins.type=sh.m_key.get_key_index(oasm.vstr.get_bottom());
		if(oasm.ins.type>tkey::c_rn)
		{
			rserror(oasm);
			return false;
		}
		if(oasm.vstr.count()==2&&
			ybase::is_jmp_ins(oasm.ins.type)&&
			!sh.m_key.is_asm_reg(oasm.vstr[1]))
		{
			int line=oasm.vstr[1].toint();
			int i;
			for(i=0;i<vasm.count();i++)
			{
				if(ybase::is_tag<rstr>(vasm[i].vstr)&&
					vasm[i].vstr[0]==oasm.vstr[1])
				{
					break;
				}
			}
			if(i>=vasm.count())
			{
				rserror(oasm);
				return false;
			}
			oasm.ins.type*=6;
			oasm.ins.first.type=topnd::c_imme;
			oasm.ins.first.val=(uint)(&vasm[i]);
			return true;
		}
		oasm.ins.type*=6;
		if(!a_asm(sh,oasm))
		{
			rserror(oasm);
			return false;
		}
		return true;
	}

	static int find_comma(tsh& sh,rbuf<rstr>& v)
	{
		int count1=0;
		int count2=0;
		int i;
		for(i=1;i<v.count();i++)
		{
			if(rsoptr(c_sbk_l)==v[i])
			{
				count1++;
			}
			elif(rsoptr(c_sbk_r)==v[i])
			{
				count1--;
			}
			elif(rsoptr(c_mbk_l)==v[i])
			{
				count2++;
			}
			elif(rsoptr(c_mbk_r)==v[i])
			{
				count2--;
			}
			elif(count1==0&&count2==0&&v[i]==rsoptr(c_comma))
			{
				break;
			}
		}
		return i;
	}

	static rbool a_asm(tsh& sh,tasm& item)
	{
		int i=find_comma(sh,item.vstr);
		if(!a_opnd(sh,item,i-1,item.vstr.sub(1,i),item.ins.first))
		{
			return false;
		}
		if(!a_opnd(sh,item,i+1,item.vstr.sub(i+1),item.ins.second))
		{
			return false;
		}
		if(!obtain_qrun_type(item.ins))
		{
			return false;
		}
		if(item.ins.type==tins::c_calle_i)
		{
			ifn(sh.m_func_list.exist((char*)(item.ins.first.val)))
			{
				rserror((char*)(item.ins.first.val));
				return false;
			}
			item.ins.first.val=(int)(sh.m_func_list[(char*)(item.ins.first.val)]);
		}
		return true;
	}

	static rbool obtain_qrun_type(tins& ins)
	{
		if(ins.second.type==topnd::c_null)
		{
			if(ins.first.type==topnd::c_null)
			{
				;
			}
			elif(ins.first.type==topnd::c_imme)
			{
				;
			}
			elif(ins.first.type==topnd::c_reg)
			{
				ins.type+=1;
			}
			elif(ins.first.type==topnd::c_addr)
			{
				ins.type+=2;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if(ins.second.type==topnd::c_imme)
			{
				if(ins.first.type==topnd::c_reg)
				{
					;
				}
				elif(ins.first.type==topnd::c_addr)
				{
					ins.type+=1;
				}
				else
				{
					;
				}
			}
			elif(ins.second.type==topnd::c_reg)
			{
				if(ins.first.type==topnd::c_reg)
				{
					ins.type+=2;
				}
				elif(ins.first.type==topnd::c_addr)
				{
					ins.type+=3;
				}
				else
				{
					return false;
				}
			}
			elif(ins.second.type==topnd::c_addr)
			{
				if(ins.first.type==topnd::c_reg)
				{
					ins.type+=4;
				}
				elif(ins.first.type==topnd::c_addr)
				{
					ins.type+=5;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		return true;
	}

	static rbool a_opnd(tsh& sh,tasm& item,int index,const rbuf<rstr>& v,topnd& o)
	{
		if(v.empty())
		{
			return true;
		}
		if(v.count()==1)
		{
			if(v.top().is_number())
			{
				//123
				o.type=topnd::c_imme;
				o.val=v.top().touint();
			}
			elif(v[0].get_bottom()==r_char('\"'))
			{
				//"abc"
				trans_cstr(v[0]);
				if(index>=item.vstr.count())
				{
					return false;
				}
				item.vstr[index]=v[0];
				o.type=topnd::c_imme;
				o.val=(uint)(item.vstr[index].begin());
			}
			else
			{
				//ebp
				o.type=topnd::c_reg;
				o.off=get_reg_off(sh,v.top());
			}
		}
		elif(v.count()==3)
		{
			if(v.bottom()==rsoptr(c_mbk_l)&&
				v.top()==rsoptr(c_mbk_r))
			{
				//[ebp]
				o.type=topnd::c_addr;
				o.off=get_reg_off(sh,v[1]);
				o.val=0;
			}
			elif(v.bottom()==rsoptr(c_sbk_l)&&
				v.top()==rsoptr(c_sbk_r)&&
				v[1].is_number())//todo:
			{
				o.type=topnd::c_imme;
				o.val=v[1].touint();
			}
			else
			{
				return false;
			}
		}
		elif(v.count()==5)
		{
			//[ebp+2]
			o.type=topnd::c_addr;
			o.off=get_reg_off(sh,v[1]);
			o.val=v[3].touint();
			if(v[2]==rsoptr(c_minus))
			{
				o.val=-o.val;
			}
		}
		elif(v.count()==7&&v[1]==rsoptr(c_addr))
		{
			tclass* ptci=yfind::class_search(sh,v[3]);
			if(ptci==null)
			{
				return false;
			}
			tfunc* ptfi=yfind::func_search_dec(*ptci,v[5]);
			if(ptfi==null)
			{
				return false;
			}
			ifn(process(sh,*ptfi))
			{
				return false;
			}
			o.type=topnd::c_imme;
			o.val=(int)(ptfi->vasm.begin());
			return true;
		}
		elif(v.count()==0)
		{
			return true;
		}
		else
		{
			return false;
		}
		if(o.off>=r_size(treg))
		{
			return false;
		}
		return true;
	}

	static void trans_cstr(rstr& src)
	{
		if(src.count()<2)
		{
			return;
		}
		rstr dst;
		for(int i=1;i<src.count()-1;i++)
		{
			if(src[i]==r_char('\\'))
			{
				if(src.get(i+1)==r_char('b'))
				{
					dst+=r_char('\b');
				}
				elif(src.get(i+1)==r_char('n'))
				{
					dst+=r_char('\n');
				}
				elif(src.get(i+1)==r_char('r'))
				{
					dst+=r_char('\r');
				}
				elif(src.get(i+1)==r_char('\0'))
				{
					dst+=r_char('\0');
				}
				elif(src.get(i+1)==r_char('x'))
				{
					uchar ch=(uchar)(rstr::hextodec(
						src.sub(i+2,i+4)).touint());
					dst+=ch;
					i=i+3;
					continue;
				}
				else
				{
					dst+=src.get(i+1);
				}
				i++;
			}
			else
			{
				dst+=src[i];
			}
		}
		src=r_move(dst);
		src.m_buf.push((uchar)0);
	}

	static int get_reg_off(tsh& sh,const rstr& s)
	{
		treg reg;
		int ret=r_size(treg);
		if(rskey(c_eax)==s)
		{
			ret=(uchar*)(&reg.eax)-(uchar*)(&reg);
		}
		elif(rskey(c_ebx)==s)
		{
			ret=(uchar*)(&reg.ebx)-(uchar*)(&reg);
		}
		elif(rskey(c_ecx)==s)
		{
			ret=(uchar*)(&reg.ecx)-(uchar*)(&reg);
		}
		elif(rskey(c_edx)==s)
		{
			ret=(uchar*)(&reg.edx)-(uchar*)(&reg);
		}
		elif(rskey(c_esi)==s)
		{
			ret=(uchar*)(&reg.esi)-(uchar*)(&reg);
		}
		elif(rskey(c_edi)==s)
		{
			ret=(uchar*)(&reg.edi)-(uchar*)(&reg);
		}
		elif(rskey(c_esp)==s)
		{
			ret=(uchar*)(&reg.esp)-(uchar*)(&reg);
		}
		elif(rskey(c_ebp)==s)
		{
			ret=(uchar*)(&reg.ebp)-(uchar*)(&reg);
		}
		elif(rskey(c_eip)==s)
		{
			ret=(uchar*)(&reg.eip)-(uchar*)(&reg);
		}
		return ret;
	}
};
