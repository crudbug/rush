#pragma once

#include "zbin.h"
#include "zjitf.h"

struct zvm
{
	static void proc_calle(treg& reg,void* func,int num)
	{
		int count=num/4;
		if(num%4)
		{
			if(count==0)
			{
				reg.eax=((int (*)())func)();
			}
			elif(count==1)
			{
				reg.eax=((int (*)(int))func)(v_pto_int(reg.esp));
			}
			elif(count==2)
			{
				reg.eax=((int (*)(int,int))func)(v_pto_int(reg.esp),v_pto_int(reg.esp+4));
			}
			elif(count==3)
			{
				reg.eax=((int (*)(int,int,int))func)(v_pto_int(reg.esp),v_pto_int(reg.esp+4),v_pto_int(reg.esp+8));
			}
			elif(count==4)
			{
				reg.eax=((int (*)(int,int,int,int))func)(v_pto_int(reg.esp),
					v_pto_int(reg.esp+4),v_pto_int(reg.esp+8),v_pto_int(reg.esp+12));
			}
			elif(count==6)
			{
				reg.eax=((int (*)(int,int,int,int,int,int))func)(v_pto_int(reg.esp),
					v_pto_int(reg.esp+4),v_pto_int(reg.esp+8),v_pto_int(reg.esp+12),
					v_pto_int(reg.esp+16),v_pto_int(reg.esp+20));
			}
		}
		else
		{
			if(count==0)
			{
				((void (*)())func)();
			}
			elif(count==1)
			{
				((void (*)(int))func)(v_pto_int(reg.esp));
			}
			elif(count==2)
			{
				((void (*)(int,int))func)(v_pto_int(reg.esp),v_pto_int(reg.esp+4));
			}
			elif(count==3)
			{
				((void (*)(int,int,int))func)(v_pto_int(reg.esp),v_pto_int(reg.esp+4),v_pto_int(reg.esp+8));
			}
			elif(count==4)
			{
				((void (*)(int,int,int,int))func)(v_pto_int(reg.esp),
					v_pto_int(reg.esp+4),v_pto_int(reg.esp+8),v_pto_int(reg.esp+12));
			}
		}
	}

	//为了提高效率将1条指令分解为6条
	static rbool run(tsh& sh)
	{
#ifndef _RS
		rbuf<tasm> init_vasm;
		rbuf<uchar> stack(tconf::c_stack_size);
		treg reg;
		tins* cur;
		ifn(init(sh,stack,reg,init_vasm))
		{
			rserror("init failed");
			return false;
		}
next:
		cur=(tins*)(reg.eip);
		//((tasm*)cur)->vstr.print();
		//rf::printl();
		
		switch(cur->type)
		{
		case tins::c_rjit_n:
		case tins::c_rjit_n+1:
		case tins::c_rjit_n+2:
		case tins::c_rjit_n+3:
		case tins::c_rjit_n+4:
		case tins::c_rjit_n+5:
			return false;

		case tins::c_calle_i:
			proc_calle(reg,(void*)(v_get_imme(cur->first)),v_get_imme(cur->second));
			v_next_ins;
		case tins::c_calle_i+1:
		case tins::c_calle_i+2:
		case tins::c_calle_i+3:
		case tins::c_calle_i+4:
		case tins::c_calle_i+5:
			return false;

		case tins::c_call_i:
			reg.esp-=4;
			v_pto_uint(reg.esp)=reg.eip+r_size(tasm);
			reg.eip=v_get_imme(cur->first);
			goto next;
		case tins::c_call_r:
			reg.esp-=4;
			v_pto_uint(reg.esp)=reg.eip+r_size(tasm);
			reg.eip=v_get_reg(cur->first);
			goto next;
		case tins::c_call_a:
			reg.esp-=4;
			v_pto_uint(reg.esp)=reg.eip+r_size(tasm);
			reg.eip=v_get_addr(cur->first);
			goto next;
		case tins::c_call_a+1:
		case tins::c_call_a+2:
		case tins::c_call_a+3:
			return false;

		case tins::c_ret_n:
			reg.eip=v_pto_uint(reg.esp);
			reg.esp+=4;
			goto next;
		case tins::c_ret_n+1:
		case tins::c_ret_n+2:
		case tins::c_ret_n+3:
		case tins::c_ret_n+4:
		case tins::c_ret_n+5:
			return false;

		case tins::c_ret_i:
			reg.eip=v_pto_uint(reg.esp);
			reg.esp+=4+v_get_imme(cur->first);
			goto next;
		case tins::c_ret_r:
			reg.eip=v_pto_uint(reg.esp);
			reg.esp+=4+v_get_reg(cur->first);
			goto next;
		case tins::c_ret_a:
			reg.eip=v_pto_uint(reg.esp);
			reg.esp+=4;
			reg.esp+=v_get_addr(cur->first);
			goto next;
		case tins::c_ret_a+1:
		case tins::c_ret_a+2:
		case tins::c_ret_a+3:
			return false;

		case tins::c_push_i:
			reg.esp-=4;
			v_pto_uint(reg.esp)=v_get_imme(cur->first);
			v_next_ins;
		case tins::c_push_r:
			reg.esp-=4;
			v_pto_uint(reg.esp)=v_get_reg(cur->first);
			v_next_ins;
		case tins::c_push_a:
			reg.esp-=4;
			v_pto_uint(reg.esp)=v_get_addr(cur->first);
			v_next_ins;
		case tins::c_push_a+1:
		case tins::c_push_a+2:
		case tins::c_push_a+3:
			return false;

		case tins::c_pop_i:
			return false;
		case tins::c_pop_r:
			v_get_reg(cur->first)=v_pto_uint(reg.esp);
			reg.esp+=4;
			v_next_ins;
		case tins::c_pop_a:
			v_get_addr(cur->first)=v_pto_uint(reg.esp);
			reg.esp+=4;
			v_next_ins;
		case tins::c_pop_a+1:
		case tins::c_pop_a+2:
		case tins::c_pop_a+3:
			return false;

		case tins::c_jmp_i:
			reg.eip=v_get_imme(cur->first);
			goto next;
		case tins::c_jmp_r:
			reg.eip=v_get_reg(cur->first);
			goto next;
		case tins::c_jmp_a:
			reg.eip=v_get_addr(cur->first);
			goto next;
		case tins::c_jmp_a+1:
		case tins::c_jmp_a+2:
		case tins::c_jmp_a+3:
			return false;

		case tins::c_jebxz_i:
			if(reg.ebx)
			{
				reg.eip+=r_size(tasm);
			}
			else
			{
				reg.eip=v_get_imme(cur->first);
			}
			goto next;
		case tins::c_jebxz_r:
			if(reg.ebx)
			{
				reg.eip+=r_size(tasm);
			}
			else
			{
				reg.eip=v_get_reg(cur->first);
			}
			goto next;
		case tins::c_jebxz_a:
			if(reg.ebx)
			{
				reg.eip+=r_size(tasm);
			}
			else
			{
				reg.eip=v_get_addr(cur->first);
			}
			goto next;
		case tins::c_jebxz_a+1:
		case tins::c_jebxz_a+2:
		case tins::c_jebxz_a+3:
			return false;

		case tins::c_jebxnz_i:
			if(reg.ebx)
			{
				reg.eip=v_get_imme(cur->first);
			}
			else
			{
				reg.eip+=r_size(tasm);
			}
			goto next;
		case tins::c_jebxnz_r:
			if(reg.ebx)
			{
				reg.eip=v_get_reg(cur->first);
			}
			else
			{
				reg.eip+=r_size(tasm);
			}
			goto next;
		case tins::c_jebxnz_a:
			if(reg.ebx)
			{
				reg.eip=v_get_addr(cur->first);
			}
			else
			{
				reg.eip+=r_size(tasm);
			}
			goto next;
		case tins::c_jebxnz_a+1:
		case tins::c_jebxnz_a+2:
		case tins::c_jebxnz_a+3:
			return false;

		case tins::c_halt_n:
			break;
		case tins::c_halt_n+1:
		case tins::c_halt_n+2:
		case tins::c_halt_n+3:
		case tins::c_halt_n+4:
		case tins::c_halt_n+5:
			return false;

		case tins::c_nop_n:
			v_next_ins;
		case tins::c_nop_n+1:
		case tins::c_nop_n+2:
		case tins::c_nop_n+3:
		case tins::c_nop_n+4:
		case tins::c_nop_n+5:
			return false;

		case tins::c_lea_ri:
			return false;
		case tins::c_lea_ai:
			return false;
		case tins::c_lea_rr:
			return false;
		case tins::c_lea_ar:
			return false;
		case tins::c_lea_ra:
			v_get_reg(cur->first)=v_get_lea(cur->second);
			v_next_ins;
		case tins::c_lea_aa:
			v_get_addr(cur->first)=v_get_lea(cur->second);
			v_next_ins;

		case tins::c_mov_ri:
			v_get_reg(cur->first)=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_mov_ai:
			v_get_addr(cur->first)=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_mov_rr:
			v_get_reg(cur->first)=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_mov_ar:
			v_get_addr(cur->first)=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_mov_ra:
			v_get_reg(cur->first)=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_mov_aa:
			v_get_addr(cur->first)=v_get_addr(cur->second);
			v_next_ins;

		case tins::c_movb_ri:
		case tins::c_movb_ai:
		case tins::c_movb_rr:
		case tins::c_movb_ar:
		case tins::c_movb_ra:
			return false;
		case tins::c_movb_aa:
			v_get_addr_8(cur->first)=v_get_addr_8(cur->second);
			v_next_ins;

		case tins::c_movl_ri:
		case tins::c_movl_ai:
		case tins::c_movl_rr:
		case tins::c_movl_ar:
		case tins::c_movl_ra:
			return false;
		case tins::c_movl_aa:
			v_get_addr_64(cur->first)=v_get_addr_64(cur->second);
			v_next_ins;

		case tins::c_add_ri:
			v_get_reg(cur->first)+=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_add_ai:
			v_get_addr(cur->first)+=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_add_rr:
			v_get_reg(cur->first)+=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_add_ar:
			v_get_addr(cur->first)+=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_add_ra:
			v_get_reg(cur->first)+=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_add_aa:
			v_get_addr(cur->first)+=v_get_addr(cur->second);
			v_next_ins;

		case tins::c_sub_ri:
			v_get_reg(cur->first)-=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_sub_ai:
			v_get_addr(cur->first)-=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_sub_rr:
			v_get_reg(cur->first)-=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_sub_ar:
			v_get_addr(cur->first)-=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_sub_ra:
			v_get_reg(cur->first)-=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_sub_aa:
			v_get_addr(cur->first)-=v_get_addr(cur->second);
			v_next_ins;
		
		case tins::c_imul_ri:
			v_get_reg(cur->first)*=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_imul_ai:
			v_get_addr(cur->first)*=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_imul_rr:
			v_get_reg(cur->first)*=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_imul_ar:
			v_get_addr(cur->first)*=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_imul_ra:
			v_get_reg(cur->first)*=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_imul_aa:
			v_get_addr(cur->first)*=v_get_addr(cur->second);
			v_next_ins;

		case tins::c_idiv_ri:
			v_get_reg(cur->first)/=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_idiv_ai:
			v_get_addr(cur->first)/=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_idiv_rr:
			v_get_reg(cur->first)/=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_idiv_ar:
			v_get_addr(cur->first)/=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_idiv_ra:
			v_get_reg(cur->first)/=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_idiv_aa:
			v_get_addr(cur->first)/=v_get_addr(cur->second);
			v_next_ins;

		case tins::c_imod_ri:
			v_get_reg(cur->first)%=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_imod_ai:
			v_get_addr(cur->first)%=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_imod_rr:
			v_get_reg(cur->first)%=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_imod_ar:
			v_get_addr(cur->first)%=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_imod_ra:
			v_get_reg(cur->first)%=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_imod_aa:
			v_get_addr(cur->first)%=v_get_addr(cur->second);
			v_next_ins;	
		
		case tins::c_cesb_ri:
			reg.ebx=v_get_reg(cur->first)==v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cesb_ai:
			reg.ebx=v_get_addr(cur->first)==v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cesb_rr:
			reg.ebx=v_get_reg(cur->first)==v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cesb_ar:
			reg.ebx=v_get_addr(cur->first)==v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cesb_ra:
			reg.ebx=v_get_reg(cur->first)==v_get_addr(cur->second);
			v_next_ins;
		case tins::c_cesb_aa:
			reg.ebx=v_get_addr(cur->first)==v_get_addr(cur->second);
			v_next_ins;

		case tins::c_cnesb_ri:
			reg.ebx=v_get_reg(cur->first)!=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cnesb_ai:
			reg.ebx=v_get_addr(cur->first)!=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cnesb_rr:
			reg.ebx=v_get_reg(cur->first)!=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cnesb_ar:
			reg.ebx=v_get_addr(cur->first)!=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cnesb_ra:
			reg.ebx=v_get_reg(cur->first)!=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_cnesb_aa:
			reg.ebx=v_get_addr(cur->first)!=v_get_addr(cur->second);
			v_next_ins;
		
		case tins::c_cgsb_ri:
			reg.ebx=v_get_reg(cur->first)>v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cgsb_ai:
			reg.ebx=v_get_addr(cur->first)>v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cgsb_rr:
			reg.ebx=v_get_reg(cur->first)>v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cgsb_ar:
			reg.ebx=v_get_addr(cur->first)>v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cgsb_ra:
			reg.ebx=v_get_reg(cur->first)>v_get_addr(cur->second);
			v_next_ins;
		case tins::c_cgsb_aa:
			reg.ebx=v_get_addr(cur->first)>v_get_addr(cur->second);
			v_next_ins;

		case tins::c_cgesb_ri:
			reg.ebx=v_get_reg(cur->first)>=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cgesb_ai:
			reg.ebx=v_get_addr(cur->first)>=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_cgesb_rr:
			reg.ebx=v_get_reg(cur->first)>=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cgesb_ar:
			reg.ebx=v_get_addr(cur->first)>=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_cgesb_ra:
			reg.ebx=v_get_reg(cur->first)>=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_cgesb_aa:
			reg.ebx=v_get_addr(cur->first)>=v_get_addr(cur->second);
			v_next_ins;

		case tins::c_clsb_ri:
			reg.ebx=v_get_reg(cur->first)<v_get_imme(cur->second);
			v_next_ins;
		case tins::c_clsb_ai:
			reg.ebx=v_get_addr(cur->first)<v_get_imme(cur->second);
			v_next_ins;
		case tins::c_clsb_rr:
			reg.ebx=v_get_reg(cur->first)<v_get_reg(cur->second);
			v_next_ins;
		case tins::c_clsb_ar:
			reg.ebx=v_get_addr(cur->first)<v_get_reg(cur->second);
			v_next_ins;
		case tins::c_clsb_ra:
			reg.ebx=v_get_reg(cur->first)<v_get_addr(cur->second);
			v_next_ins;
		case tins::c_clsb_aa:
			reg.ebx=v_get_addr(cur->first)<v_get_addr(cur->second);
			v_next_ins;

		case tins::c_clesb_ri:
			reg.ebx=v_get_reg(cur->first)<=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_clesb_ai:
			reg.ebx=v_get_addr(cur->first)<=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_clesb_rr:
			reg.ebx=v_get_reg(cur->first)<=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_clesb_ar:
			reg.ebx=v_get_addr(cur->first)<=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_clesb_ra:
			reg.ebx=v_get_reg(cur->first)<=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_clesb_aa:
			reg.ebx=v_get_addr(cur->first)<=v_get_addr(cur->second);
			v_next_ins;
			
		case tins::c_band_ri:
			v_get_reg_u(cur->first)&=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_band_ai:
			v_get_addr_u(cur->first)&=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_band_rr:
			v_get_reg_u(cur->first)&=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_band_ar:
			v_get_addr_u(cur->first)&=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_band_ra:
			v_get_reg_u(cur->first)&=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_band_aa:
			v_get_addr_u(cur->first)&=v_get_addr_u(cur->second);
			v_next_ins;	

		case tins::c_bor_ri:
			v_get_reg_u(cur->first)|=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bor_ai:
			v_get_addr_u(cur->first)|=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bor_rr:
			v_get_reg_u(cur->first)|=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bor_ar:
			v_get_addr_u(cur->first)|=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bor_ra:
			v_get_reg_u(cur->first)|=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_bor_aa:
			v_get_addr_u(cur->first)|=v_get_addr_u(cur->second);
			v_next_ins;	

		case tins::c_bnot_i:
			return false;
		case tins::c_bnot_r:
			v_get_reg(cur->first)=~v_get_reg(cur->first);
			v_next_ins;	
		case tins::c_bnot_a:
			v_get_addr(cur->first)=~v_get_addr(cur->first);
			v_next_ins;
		case tins::c_bnot_a+1:
		case tins::c_bnot_a+2:
		case tins::c_bnot_a+3:
			return false;

		case tins::c_bxor_ri:
			v_get_reg_u(cur->first)^=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bxor_ai:
			v_get_addr_u(cur->first)^=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bxor_rr:
			v_get_reg_u(cur->first)^=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bxor_ar:
			v_get_addr_u(cur->first)^=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bxor_ra:
			v_get_reg_u(cur->first)^=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_bxor_aa:
			v_get_addr_u(cur->first)^=v_get_addr_u(cur->second);
			v_next_ins;	

		case tins::c_bshl_ri:
			v_get_reg_u(cur->first)<<=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bshl_ai:
			v_get_addr_u(cur->first)<<=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bshl_rr:
			v_get_reg_u(cur->first)<<=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bshl_ar:
			v_get_addr_u(cur->first)<<=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bshl_ra:
			v_get_reg_u(cur->first)<<=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_bshl_aa:
			v_get_addr_u(cur->first)<<=v_get_addr_u(cur->second);
			v_next_ins;	

		case tins::c_bshr_ri:
			v_get_reg_u(cur->first)>>=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bshr_ai:
			v_get_addr_u(cur->first)>>=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_bshr_rr:
			v_get_reg_u(cur->first)>>=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bshr_ar:
			v_get_addr_u(cur->first)>>=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_bshr_ra:
			v_get_reg_u(cur->first)>>=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_bshr_aa:
			v_get_addr_u(cur->first)>>=v_get_addr_u(cur->second);
			v_next_ins;	

		case tins::c_bsar_ri:
			v_get_reg(cur->first)>>=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_bsar_ai:
			v_get_addr(cur->first)>>=v_get_imme(cur->second);
			v_next_ins;
		case tins::c_bsar_rr:
			v_get_reg(cur->first)>>=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_bsar_ar:
			v_get_addr(cur->first)>>=v_get_reg(cur->second);
			v_next_ins;
		case tins::c_bsar_ra:
			v_get_reg(cur->first)>>=v_get_addr(cur->second);
			v_next_ins;
		case tins::c_bsar_aa:
			v_get_addr(cur->first)>>=v_get_addr(cur->second);
			v_next_ins;	

		case tins::c_udiv_ri:
			v_get_reg_u(cur->first)/=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_udiv_ai:
			v_get_addr_u(cur->first)/=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_udiv_rr:
			v_get_reg_u(cur->first)/=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_udiv_ar:
			v_get_addr_u(cur->first)/=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_udiv_ra:
			v_get_reg_u(cur->first)/=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_udiv_aa:
			v_get_addr_u(cur->first)/=v_get_addr_u(cur->second);
			v_next_ins;

		case tins::c_umod_ri:
			v_get_reg_u(cur->first)%=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_umod_ai:
			v_get_addr_u(cur->first)%=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_umod_rr:
			v_get_reg_u(cur->first)%=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_umod_ar:
			v_get_addr_u(cur->first)%=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_umod_ra:
			v_get_reg_u(cur->first)%=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_umod_aa:
			v_get_addr_u(cur->first)%=v_get_addr_u(cur->second);
			v_next_ins;		

		case tins::c_ucgsb_ri:
			reg.ebx=v_get_reg_u(cur->first)>v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_ucgsb_ai:
			reg.ebx=v_get_addr_u(cur->first)>v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_ucgsb_rr:
			reg.ebx=v_get_reg_u(cur->first)>v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_ucgsb_ar:
			reg.ebx=v_get_addr_u(cur->first)>v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_ucgsb_ra:
			reg.ebx=v_get_reg_u(cur->first)>v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_ucgsb_aa:
			reg.ebx=v_get_addr_u(cur->first)>v_get_addr_u(cur->second);
			v_next_ins;

		case tins::c_ucgesb_ri:
			reg.ebx=v_get_reg_u(cur->first)>=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_ucgesb_ai:
			reg.ebx=v_get_addr_u(cur->first)>=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_ucgesb_rr:
			reg.ebx=v_get_reg_u(cur->first)>=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_ucgesb_ar:
			reg.ebx=v_get_addr_u(cur->first)>=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_ucgesb_ra:
			reg.ebx=v_get_reg_u(cur->first)>=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_ucgesb_aa:
			reg.ebx=v_get_addr_u(cur->first)>=v_get_addr_u(cur->second);
			v_next_ins;

		case tins::c_uclsb_ri:
			reg.ebx=v_get_reg_u(cur->first)<v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_uclsb_ai:
			reg.ebx=v_get_addr_u(cur->first)<v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_uclsb_rr:
			reg.ebx=v_get_reg_u(cur->first)<v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_uclsb_ar:
			reg.ebx=v_get_addr_u(cur->first)<v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_uclsb_ra:
			reg.ebx=v_get_reg_u(cur->first)<v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_uclsb_aa:
			reg.ebx=v_get_addr_u(cur->first)<v_get_addr_u(cur->second);
			v_next_ins;

		case tins::c_uclesb_ri:
			reg.ebx=v_get_reg_u(cur->first)<=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_uclesb_ai:
			reg.ebx=v_get_addr_u(cur->first)<=v_get_imme_u(cur->second);
			v_next_ins;
		case tins::c_uclesb_rr:
			reg.ebx=v_get_reg_u(cur->first)<=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_uclesb_ar:
			reg.ebx=v_get_addr_u(cur->first)<=v_get_reg_u(cur->second);
			v_next_ins;
		case tins::c_uclesb_ra:
			reg.ebx=v_get_reg_u(cur->first)<=v_get_addr_u(cur->second);
			v_next_ins;
		case tins::c_uclesb_aa:
			reg.ebx=v_get_addr_u(cur->first)<=v_get_addr_u(cur->second);
			v_next_ins;

		default:
			return false;
		}
#endif
		return true;
	}

	static rbool init(tsh& sh,rbuf<uchar>& stack,
		treg& reg,rbuf<tasm>& init_vasm)
	{
		ifn(asm_init(sh,init_vasm))
		{
			return false;
		}
		reg.esp=(uint)(stack.end());
		reg.eip=(uint)(init_vasm.begin());
		return true;
	}

	static rbool asm_init(tsh& sh,rbuf<tasm>& vasm)
	{
		vasm+=rf::vstr("sub","esp",",","4");
		vasm+=rf::vstr("call",rsoptr(c_mbk_l),rsoptr(c_addr),
			rsoptr(c_comma),"main",rsoptr(c_comma),"main()",
			rsoptr(c_mbk_r));
		vasm+=rf::vstr("add","esp",",","4");
		vasm+=rf::vstr("halt");
		return zbin::proc_vasm(sh,vasm);
	}
};
