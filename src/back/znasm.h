#pragma once

#include "zbin.h"

struct znasm
{
	static rbool process(tsh& sh)
	{
		tfunc* ptfi=yfind::func_search(*sh.m_main,"main");
		if(ptfi==null)
		{
			rf::printl("main not find");
			return false;
		}
		rstr result;
		rbuf<rstr> vconst;
		rset<rstr> scall;
		ifn(proc_func(sh,*ptfi,result,vconst,scall))
		{
			return false;
		}
		rstr head;
		head+=("%include '"+rcode::utf8_to_gbk(
			ybase::get_rs_dir())+"ext/nasm/windemos.inc'\n");
		for(rstr* p=scall.begin();p!=scall.end();p=scall.next(p))
		{
			ifn(is_ex_func(sh,*p))
			{
				head+="IMPORT "+*p+"\n";
			}
		}
		//head+="IMPORT _getch\n"; nasm的一个bug?
		head+="entry demo11\n";
		head+="\n";
		head+="[section .bss]\n";
		head+="align 32\n";
		head+="resb 4\n";
		head+=rstr("_PMAIN_A: resb ")+sh.m_main_data.size()+"\n";
		add_lambda(sh,head);
		head+="\n";
		head+="[section .data]\n";
		head+="align 32\n";
		head+="db 0,0,0,0\n";//防止数据段为空
		add_str(sh,head,vconst);
		
		head+="\n";
		head+="[section .text]\n";
		head+="proc demo11, ptrdiff_t argcount, ptrdiff_t cmdline\n";
		head+="locals none\n";
		head+="	sub esp , 4\n";
		head+="	call main2Emain2829\n";
		head+="	add esp , 4\n";
		head+="	xor eax , eax\n";
		head+="endproc\n";
		head+="proc _rs_main\n";
		head+="locals none\n";
	
		head+=result;
		head+="\n";
		head+="endproc\n";
		head+="\n";

		head+=("%include '"+rcode::utf8_to_gbk(
			ybase::get_rs_dir())+"ext/nasm/exfunc.inc'\n");

		rstr name=ybase::get_main_name(sh)+".asm";
		rfile file;
		ifn(file.open_n(name,"rw"))
		{
			rf::printl("file open error");
			return false;
		}
		ifn(file.write(head.size(),head.begin()))
		{
			rf::printl("file write error");
			return false;
		}
		return true;
	}

	static rbool is_ex_func(tsh& sh,const rstr& s)
	{
		return (s==rskey(c_bshl)||s==rskey(c_bshr)||s==rskey(c_bsar)||
			s=="faddl"||s=="fsubl"||s=="fmull"||s=="fdivl"||s=="fclsbl"||
			s=="addl"||s=="subl");
	}

	static void add_str(tsh& sh,rstr& result,rbuf<rstr>& vconst)
	{
		for(int i=0;i<vconst.count();i++)
		{
			zbin::trans_cstr(vconst[i]);
			add_str_one(result,i,vconst[i]);
		}
	}

	static void add_str_one(rstr& result,int index,const rstr& s)
	{
		result+="_RC_"+rstr(index)+": db ";
		for(int i=0;i<s.count()-1;i++)
		{
			result+=rstr((int)(s[i]))+",";
		}
		result+="0\n";
	}

	static void add_lambda(tsh& sh,rstr& result)
	{
		for(tclass* p=sh.m_class.begin();
			p!=sh.m_class.end();p=sh.m_class.next(p))
		{
			add_lambda(result,*p);
		}
	}

	static void add_lambda(rstr& result,tclass& tci)
	{
		for(tfunc* p=tci.vfunc.begin();p!=tci.vfunc.end();p=tci.vfunc.next(p))
		{
			if(p->lambda_data.empty())
			{
				continue;
			}
			result+=p->name+"_C_P_A: resb "+p->lambda_data.size()+"\n";
		}
	}

	static tfunc* call_find(tsh& sh,tasm& item)
	{
		int i;
		for(i=1;i<item.vstr.count();i++)
		{
			if(item.vstr[i]=="&")
			{
				break;
			}
		}
		//一条指令里最多出现一次&,而且 &操作数 只能是最后一个操作数
		if(i>=item.vstr.count())
		{
			return null;
		}
		i+=2;
		tclass* ptci=yfind::class_search(sh,item.vstr.get(i));
		if(ptci==null)
		{
			return null;
		}
		i+=2;
		rstr fname=item.vstr.get(i);
		return yfind::func_search_dec(*ptci,fname);
	}

	static rstr symbol_trans(const rstr& s)
	{
		rstr ret;
		for(int i=0;i<s.count();i++)
		{
			if(rstr::is_alpha(s[i])||rstr::is_number(s[i])||s[i]==r_char('_'))
			{
				ret+=s[i];
			}
			else
			{
				ret+=rstr::format("%02X",(int)(s[i]));
			}
		}
		return r_move(ret);
	}

	static rstr get_nasm_symbol(tfunc& tfi)
	{
		return symbol_trans(tfi.ptci->name+"."+tfi.name_dec);
	}

	static rbool is_jmp_ins_nasm(const rstr& s)
	{
		return (s=="call"||s=="je"||s=="jne"||s=="jg"||s=="jge"||
			s=="jl"||s=="jle"||s=="ja"||s=="jae"||s=="jb"||s=="jbe");
	}

	//将一个函数翻译成NASM汇编代码
	static rbool proc_func(tsh& sh,tfunc& tfi,rstr& result,
		rbuf<rstr>& vconst,rset<rstr>& scall,int level=0)
	{
		if(level>300)
		{
			rserror(tfi,"nasm level overflow");
			return false;
		}
		level++;
		if(tfi.count==1)
		{
			return true;
		}
		tfi.count=1;
		if(tfi.vasm.empty())
		{
			if(!zbin::cp_vword_to_vasm(sh,tfi,tenv()))
			{
				return false;
			}
		}
		rstr symbol=get_nasm_symbol(tfi);
		result+="\n"+symbol+":\n";
		for(int i=0;i<tfi.vasm.count();i++)
		{
			if(ybase::is_tag<rstr>(tfi.vasm[i].vstr))
			{
				//todo:_func_end标号有时不是必须的
				result+=symbol+"_"+tfi.vasm[i].vstr[0];
				result+=tfi.vasm[i].vstr[1];
				result+="\n";
				continue;
			}
			rstr s;
			ifn(proc_asm(sh,tfi,tfi.vasm[i],s,vconst,scall))
			{
				rserror(tfi.vasm[i]);
				rserror(tfi);
				return false;
			}
			result+=s;
		}
		for(int i=0;i<tfi.vasm.count();i++)
		{
			tfunc* ptfi=call_find(sh,tfi.vasm[i]);
			if(ptfi==null)
			{
				continue;
			}
			ifn(proc_func(sh,*ptfi,result,vconst,scall,level))
			{
				return false;
			}
		}
		return true;
	}

	static void proc_const_str(rbuf<rstr>& vstr,rbuf<rstr>& vconst)
	{
		if(vstr.count()>=2&&vstr[0]=="calle")
		{
			return;
		}
		for(int i=0;i<vstr.count();i++)
		{
			if(vstr[i].get_bottom()==r_char('\"')&&vstr[i].count()>=2)
			{
				vconst.push(vstr[i]);
				vstr[i]="_RC_"+rstr(vconst.count()-1);
			}
		}
	}

	static rbool have_single_esp(tsh& sh,tasm& item)
	{
		for(int i=0;i<item.vstr.count();i++)
		{
			if(item.vstr[i]==rskey(c_esp)||item.vstr[i]==rskey(c_ebp))
			{
				if(item.vstr.get(i-1)!=rsoptr(c_mbk_l))
				{
					return true;
				}
				if(item.vstr.get(i+2).toint()<4)
				{
					return true;
				}
				if(item.vstr.get(i+1)!=rsoptr(c_plus))
				{
					return true;
				}
			}
			if(item.vstr[i].get_bottom()==r_char('\"')&&item.vstr[i].count()>=2)
			{
				return true;
			}
		}
		return false;
	}

	static void fix_esp(tsh& sh,tasm& item)
	{
		for(int i=0;i<item.vstr.count();i++)
		{
			if(item.vstr[i]!=rskey(c_esp)&&item.vstr[i]!=rskey(c_ebp))
			{
				continue;
			}
			if(item.vstr.count()>i+2&&item.vstr[i+2].is_number())
			{
				item.vstr[i+2]=item.vstr[i+2].toint()-4;
			}
		}
	}

	static rbool proc_asm(tsh& sh,tfunc& tfi,tasm& item,
		rstr& result,rbuf<rstr>& vconst,rset<rstr>& scall)
	{
		rbuf<rstr>& vstr=item.vstr;
		proc_const_str(vstr,vconst);
		if(vstr.empty())
		{
			return false;
		}
		int type=sh.m_key.get_key_index(vstr[0]);
		tfunc* ptfi;
		switch(type)
		{
		case tkey::c_calle:
			if(ybase::del_quote(vstr.get(1))!="printf")//todo:
			{
				scall.insert_c(ybase::del_quote(vstr.get(1)));
			}
			result+="	invoke "+ybase::del_quote(vstr.get(1))+"\n";
			return true;
		case tkey::c_call:
			ptfi=call_find(sh,item);
			if(ptfi==null)
			{
				result+=("	call dword "+
					link_vstr(vstr.sub(1))+"\n");
				return true;
			}
			result+="	call "+get_nasm_symbol(*ptfi)+"\n";
			return true;
		case tkey::c_retn:
			result+="	ret\n";
			return true;
		case tkey::c_reti:
			result+="	ret "+vstr.get(1)+"\n";
			return true;
		case tkey::c_push:
			ptfi=call_find(sh,item);
			if(ptfi==null)
			{
				result+="	push dword "+link_vstr(vstr.sub(1))+"\n";
			}
			else
			{
				result+="	push "+get_nasm_symbol(*ptfi)+"\n";
			}
			return true;
		case tkey::c_pop:
			result+="	pop dword "+link_vstr(vstr.sub(1))+"\n";
			return true;
		case tkey::c_jmp:
			result+="	jmp "+get_nasm_symbol(tfi)+"_"+vstr.get(1)+"\n";
			return true;
		case tkey::c_jebxz:
			result+="	cmp ebx , 0\n";
			result+="	jz "+get_nasm_symbol(tfi)+"_"+vstr.get(1)+"\n";
			return true;
		case tkey::c_jebxnz:
			result+="	cmp ebx , 0\n";
			result+="	jnz "+get_nasm_symbol(tfi)+"_"+vstr.get(1)+"\n";
			return true;
		case tkey::c_nop:
			result+="	nop\n";
			return true;
		case tkey::c_lea:
			if(count_mbk_l(vstr)==2)
			{
				result+="	lea ecx , "+get_opnd2(vstr)+"\n";
				result+="	mov "+get_opnd1(vstr)+" , ecx\n";
			}
			else
			{
				result+="	lea dword "+link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		case tkey::c_mov:
			if(count_mbk_l(vstr)==2)
			{
				result+="	mov ecx , "+get_opnd2(vstr)+"\n";
				result+="	mov "+get_opnd1(vstr)+" , ecx\n";
				return true;
			}
			ptfi=call_find(sh,item);
			if(ptfi==null)
			{
				result+="	mov dword "+link_vstr(vstr.sub(1))+"\n";
			}
			else
			{
				result+=("	mov dword "+get_opnd1(vstr)+" , "+
					get_nasm_symbol(*ptfi)+"\n");
			}
			return true;
		case tkey::c_movb:
			result+="	mov cl , "+get_opnd2(vstr)+"\n";
			result+="	mov "+get_opnd1(vstr)+" , cl\n";
			return true;
		case tkey::c_movl:
			return false;
		case tkey::c_add:
			if(count_mbk_l(vstr)==2)
			{
				result+="	mov ecx , "+get_opnd2(vstr)+"\n";
				result+="	add "+get_opnd1(vstr)+" , ecx\n";
			}
			else
			{
				result+=("	add dword "+
					link_vstr(vstr.sub(1))+"\n");
			}
			return true;
		case tkey::c_sub:
			if(count_mbk_l(vstr)==2)
			{
				result+="	mov ecx , "+get_opnd2(vstr)+"\n";
				result+="	sub "+get_opnd1(vstr)+" , ecx\n";
			}
			else
			{
				result+="	sub dword "+link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		case tkey::c_imul:
			result+="	mov ecx , "+get_opnd1(vstr)+"\n";
			result+="	imul ecx , "+get_opnd2(vstr)+"\n";
			result+="	mov "+get_opnd1(vstr)+" , ecx\n";
			return true;
		case tkey::c_idiv:
			result+="	mov eax , "+get_opnd1(vstr)+"\n";
			result+="	cdq\n";
			result+="	mov ecx , "+get_opnd2(vstr)+"\n";
			result+="	idiv ecx\n";
			result+="	mov "+get_opnd1(vstr)+" , eax\n";
			return true;
		case tkey::c_imod:
			result+="	mov eax , "+get_opnd1(vstr)+"\n";
			result+="	cdq\n";
			result+="	mov ecx , "+get_opnd2(vstr)+"\n";
			result+="	idiv ecx\n";
			result+="	mov "+get_opnd1(vstr)+" , edx\n";
			return true;
		case tkey::c_cesb:
			return ncmp(result,vstr,"	sete bl\n");
		case tkey::c_cnesb:
			return ncmp(result,vstr,"	setne bl\n");
		case tkey::c_cgsb:
			return ncmp(result,vstr,"	setg bl\n");
		case tkey::c_cgesb:
			return ncmp(result,vstr,"	setge bl\n");
		case tkey::c_clsb:
			return ncmp(result,vstr,"	setl bl\n");
		case tkey::c_clesb:
			return ncmp(result,vstr,"	setle bl\n");
		case tkey::c_band:
			if(count_mbk_l(vstr)==2)
			{
				result+="	mov ecx , "+get_opnd2(vstr)+"\n";
				result+="	and "+get_opnd1(vstr)+" , ecx\n";
			}
			else
			{
				result+="	and dword "+link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		case tkey::c_bor:
			if(count_mbk_l(vstr)==2)
			{
				result+="	mov ecx , "+get_opnd2(vstr)+"\n";
				result+="	or "+get_opnd1(vstr)+" , ecx\n";
			}
			else
			{
				result+="	or dword "+link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		case tkey::c_bnot:
			result+="	not dword "+link_vstr(vstr.sub(1))+"\n";
			return true;
		case tkey::c_bxor:
			if(count_mbk_l(vstr)==2)
			{
				result+="	mov ecx , "+get_opnd2(vstr)+"\n";
				result+="	xor "+get_opnd1(vstr)+" , ecx\n";
			}
			else
			{
				result+="	xor dword "+link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		case tkey::c_bshl:
			return false;
		case tkey::c_bshr:
			return false;
		case tkey::c_bsar:
			return false;
		case tkey::c_udiv:
			result+="	mov eax , "+get_opnd1(vstr)+"\n";
			result+="	xor edx , edx\n";
			result+="	mov ecx , "+get_opnd2(vstr)+"\n";
			result+="	div ecx\n";
			result+="	mov "+get_opnd1(vstr)+" , eax\n";
			return true;
		case tkey::c_umod:
			result+="	mov eax , "+get_opnd1(vstr)+"\n";
			result+="	xor edx , edx\n";
			result+="	mov ecx , "+get_opnd2(vstr)+"\n";
			result+="	div ecx\n";
			result+="	mov "+get_opnd1(vstr)+" , edx\n";
			return true;
		case tkey::c_ucgsb:
			return ncmp(result,vstr,"	seta bl\n");
		case tkey::c_ucgesb:
			return ncmp(result,vstr,"	setae bl\n");
		case tkey::c_uclsb:
			return ncmp(result,vstr,"	setb bl\n");
		case tkey::c_uclesb:
			return ncmp(result,vstr,"	setbe bl\n");
		case tkey::c_rn:
			if(vstr.count()==3&&
				!sh.m_key.is_asm_reg(vstr[2])&&
				is_jmp_ins_nasm(vstr[1]))
			{
				result+=("	"+vstr[1]+" "+get_nasm_symbol(tfi)+"_"+
					vstr.top()+"\n");
			}
			else
			{
				result+="	"+link_vstr(vstr.sub(1))+"\n";
			}
			return true;
		}
		return false;
	}

	static rbool ncmp(rstr& result,rbuf<rstr>& vstr,const rstr& cont)
	{
		if(count_mbk_l(vstr)==2)
		{
			result+="	mov ecx , "+get_opnd2(vstr)+"\n";
			result+="	cmp "+get_opnd1(vstr)+" , ecx\n";
		}
		else
		{
			result+="	cmp dword "+link_vstr(vstr.sub(1))+"\n";
		}
		result+="	mov ebx , 0\n";
		result+=cont;
		return true;
	}

	static rstr link_vstr(const rbuf<rstr>& v)
	{
		return rstr::join<rstr>(v," ");
	}

	static rstr get_opnd1(rbuf<rstr>& vstr)
	{
		return link_vstr(get_opnd1_v(vstr));
	}

	static rbuf<rstr> get_opnd1_v(rbuf<rstr>& vstr)
	{
		int i;
		int count=0;
		for(i=1;i<vstr.count();i++)
		{
			if("("==vstr[i])
			{
				count++;
			}
			elif(")"==vstr[i])
			{
				count--;
			}
			elif(count==0&&vstr[i]==",")
			{
				break;
			}
		}
		return vstr.sub(1,i);
	}

	static rstr get_opnd2(rbuf<rstr>& vstr)
	{
		return link_vstr(get_opnd2_v(vstr));
	}
	
	//todo: merge to get_opnd1_v
	static rbuf<rstr> get_opnd2_v(rbuf<rstr>& vstr)
	{
		int i;
		int count=0;
		for(i=1;i<vstr.count();i++)
		{
			if("("==vstr[i])
			{
				count++;
			}
			elif(")"==vstr[i])
			{
				count--;
			}
			elif(count==0&&vstr[i]==",")
			{
				break;
			}
		}
		return vstr.sub(i+1);
	}

	static int count_mbk_l(rbuf<rstr>& vstr)
	{
		int sum=0;
		for(int i=0;i<vstr.count();i++)
		{
			if(vstr[i]=="[")
			{
				sum++;
			}
		}
		return sum;
	}
};
