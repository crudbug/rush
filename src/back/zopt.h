#pragma once

#include "../struct/tbase.h"

//优化类
struct yopt
{
	static rbool is_add_sub(const tsh& sh,const rstr& s)
	{
		return s==rskey(c_add)||s==rskey(c_sub);
	}

	static rbool is_add_sub(const tsh& sh,const rbuf<rstr>& v)
	{
		return (v.count()==4&&is_add_sub(sh,v[0])&&
			v[2]==rsoptr(c_comma)&&v[3].is_number());
	}

	static rbool op_add_sub(const tsh& sh,rbuf<tasm>& vasm)
	{
		rbuf<tasm> result;
		for(int i=0;i<vasm.count();i++)
		{
			if(is_add_sub(sh,vasm[i].vstr))
			{
				int j;
				for(j=i+1;j<vasm.count();j++)
				{
					if(!is_add_sub(sh,vasm[j].vstr))
					{
						break;
					}
					if(vasm[j].vstr.get(1)!=vasm[i].vstr.get(1))
					{
						break;
					}
				}
				int val=0;
				for(int k=i;k<j;k++)
				{
					if(vasm[k].vstr.get(0)==rskey(c_add))
					{
						val+=vasm[k].vstr.get(3).toint();
					}
					else
					{
						val-=vasm[k].vstr.get(3).toint();
					}
				}
				if(val<0)
				{
					val=-val;
					vasm[i].vstr[0]=rskey(c_sub);
				}
				else
				{
					vasm[i].vstr[0]=rskey(c_add);
				}
				vasm[i].vstr[3]=rstr(val);
				if(val!=0)
				{
					result.push(vasm[i]);
				}
				i=j-1;
			}
			elif(tconf::c_op_nop&&vasm[i].vstr.count()==1&&
				vasm[i].vstr[0]==rskey(c_nop))
			{
				;
			}
			else
			{
				result.push(vasm[i]);
			}
		}
		vasm=r_move(result);
		return true;
	}

	static rbool match(const rbuf<rstr>& item,const rbuf<rstr>& pick)
	{
		if(item.count()!=pick.count())
		{
			return false;
		}
		for(int i=0;i<item.count();i++)
		{
			if(pick[i]=="@@")
			{
				continue;
			}
			if(pick[i]=="@n"&&item[i].is_number())
			{
				continue;
			}
			if(pick[i]!=item[i])
			{
				return false;
			}
		}
		return true;
	}

	static rbool match(const tasm& item,const tasm& pick)
	{
		return match(item.vstr,pick.vstr);
	}

	static rbool replace(const rbuf<tasm>& item,const rbuf<tasm>& pick,rbuf<tasm>& dst)
	{
		rbuf<rstr> vstr;
		for(int i=0;i<pick.count();i++)
		{
			for(int j=0;j<pick[i].vstr.count();j++)
			{
				if(pick[i].vstr[j].get(0)==r_char('@'))
				{
					vstr.push(item[i].vstr[j]);
				}
			}
		}
		for(int i=0;i<dst.count();i++)
		{
			for(int j=0;j<dst[i].vstr.count();j++)
			{
				//@后面的数字暂时只能是1位数
				if(dst[i].vstr[j].get(0)==r_char('@'))
				{
					int num=dst[i].vstr[j].sub(1).toint();
					if(num>=vstr.count())
					{
						return false;
					}
					dst[i].vstr[j]=vstr[num];
				}
			}
		}
		return true;
	}

	static rbool op_match(const tsh& sh,rbuf<tasm>& vasm)
	{
		rbuf<tasm> result;
		for(int i=0;i<vasm.count();i++)
		{
			int j;
			for(j=0;j<sh.m_match.count();j++)
			{
				int len=sh.m_match[j].src.count();
				if(i+len>vasm.count())
				{
					continue;
				}
				int k;
				for(k=i;k<i+len;k++)
				{
					ifn(match(vasm[k],sh.m_match[j].src[k-i]))
					{
						break;
					}
				}
				if(k<i+len)
				{
					continue;
				}
				rbuf<tasm> temp=sh.m_match[j].dst;
				ifn(replace(vasm.sub(i,i+len),sh.m_match[j].src,temp))
				{
					return false;
				}
				result+=r_move(temp);
				i=i+len-1;
				break;
			}
			if(j>=sh.m_match.count())
			{
				result+=vasm[i];
			}
		}
		vasm=r_move(result);
		return true;
	}
};
