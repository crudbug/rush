#pragma once

#define c_rs_deep 150
#define c_point_size 4

#define rskey(a) (sh.m_key[tkey::a])
#define rsoptr(a) (sh.m_optr[toptr::a])

#define v_next_ins_js cur=(tins*)((uchar*)cur+r_size(tasm));goto next
#define v_next_ins reg.eip+=r_size(tasm);goto next

#define v_ato_int(a) (*r_to_pint(&(a)))
#define v_pto_int(a) (*r_to_pint(a))
#define v_ato_uint(a) (*r_to_puint(&(a)))
#define v_pto_uint(a) (*r_to_puint(a))
#define v_ato_char(a) (*r_to_pchar(&(a)))
#define v_pto_char(a) (*r_to_pchar(a))
#define v_pto_int64(a) (*r_to_pint64(a))
#define v_pto_f64(a) (*r_to_pdouble(a))
#define v_pto_rstr(a) (*r_to_prstr(a))

#define v_pto_pchar(a) ((char*)v_pto_int(a))
#define v_pto_pvoid(a) ((void*)v_pto_int(a))

#define v_get_imme_u(a) (r_to_uint((a).val))
#define v_get_reg_u(a) v_pto_uint(r_to_puchar(&reg)+(a).off)
#define v_get_addr_u(a) v_pto_uint(v_get_reg(a)+(a).val)

#define v_get_imme(a) ((a).val)
#define v_get_reg(a) v_pto_int(r_to_puchar(&reg)+(a).off)
#define v_get_addr(a) v_pto_int(v_get_reg(a)+(a).val)
#define v_get_lea(a) (v_get_reg(a)+v_get_imme(a))
#define v_get_addr_8(a) v_pto_char(v_get_reg(a)+(a).val)
#define v_get_addr_64(a) v_pto_int64(v_get_reg(a)+(a).val)
#define v_get_addr_f64(a) v_pto_f64(v_get_reg(a)+(a).val)

#define rsja znasm::get_opnd1_v(vstr)
#define rsjb znasm::get_opnd2_v(vstr)
#define rsj4(a,b,c,d) (zjiti::a(build_ins(sh,rskey(c_nop),b,c,d)))
#define rsjf(a,b) sh.m_func_list[a]=(void*)&b

#define rserror rf::printl(rstr("rs_src:")+__FILE__+" "+__LINE__);ybase::error
