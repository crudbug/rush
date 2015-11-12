#pragma once

#include "ybase.h"

//替换控制结构并生成语句表，包括return
//tfunc.vword->vsent
struct ycontrol
{
	static rbool proc_func(const tsh& sh,tfunc& tfi)
	{
		if(!replace_switch(sh,tfi.vword))
		{
			return false;
		}
		ifn(replace_ifn(sh,tfi.vword))
		{
			return false;
		}
		ifn(add_semi(sh,tfi.vword))
		{
			return false;
		}
		ifn(replace_for(sh,tfi.vword,tfi.id))
		{
			return false;
		}
		ifn(add_else(sh,tfi.vword))
		{
			return false;
		}
		ifn(replace_elif(sh,tfi.vword))
		{
			return false;
		}
		ifn(replace_if(sh,tfi.vword,tfi.id))
		{
			return false;
		}
		if(!del_bbk(sh,tfi.vword))
		{
			return false;
		}
		tfi.vword+=rstr("_func_end");
		tfi.vword+=rsoptr(c_colon);
		tfi.vword+=rsoptr(c_semi);
		ifn(replace_return(sh,tfi.vword,tfi))
		{
			return false;
		}
		rbuf<rbuf<tword> > vsent=r_split_a<tword>(
			tfi.vword,tword(rsoptr(c_semi)));
		for(int i=0;i<vsent.count();i++)
		{
			tsent sent;
			sent.vword=vsent[i];
			tfi.vsent+=sent;
		}
		return true;
	}

	static rbool del_bbk(const tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rsoptr(c_bbk_l))
			{
				continue;
			}
			int left=i;
			int right=ybase::find_symm_bbk(sh,v,left);
			if(right>=v.count())
			{
				rserror(v.get(i),"miss }");
				return false;
			}
			v[left].val.clear();
			v[right].val.clear();
		}
		ybase::arrange(v);
		return true;
	}

	static rbool add_semi(const tsh& sh,rbuf<tword>& v)
	{
		rbuf<tword> result;
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val==rsoptr(c_bbk_l)||v[i].val==rsoptr(c_bbk_r))
			{
				result+=v[i];
				continue;
			}
			if(v[i]==rskey(c_if)||v[i]==rskey(c_else)||
				v[i]==rskey(c_for)||v[i]==rskey(c_switch)||
				v[i]==rskey(c_case))
			{
				int end=v.find(rsoptr(c_bbk_l),i+1);
				if(end>=v.count())
				{
					return false;
				}
				result+=v.sub(i,end+1);
				i=end;
				continue;
			}
			int end=yformat::get_sent_end(sh,v,i,true);
			if(end>=v.count())
			{
				return false;
			}
			result+=v.sub(i,end+1);
			if(v[end]!=rsoptr(c_semi))
			{
				tword item;
				item.pos=v[end].pos;
				item.val=rsoptr(c_semi);
				result+=item;
			}
			i=end;
		}
		v=result;
		return true;
	}

	static rbool replace_ifn(const tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rskey(c_ifn))
			{
				continue;
			}
			int pos=v.find(rsoptr(c_bbk_l),i+1);
			if(pos>=v.count())
			{
				rserror(v[i]);
				return false;
			}
			rbuf<tword> vcond=v.sub(i+1,pos);
			ybase::clear_word_val(v,i,pos);
			v[i].multi.push("if");
			v[i].multi.push("!");
			v[i].multi.push("(");
			v[i].multi+=ybase::vword_to_vstr(vcond);
			v[i].multi.push(")");
			i=pos;
		}
		ybase::arrange(v);
		return true;
	}

	//switch中不需要break
	static rbool replace_switch(const tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rskey(c_switch))
			{
				continue;
			}
			int pos=v.find(rsoptr(c_bbk_l),i+1);
			if(pos>=v.count())
			{
				rserror(v[i]);
				return false;
			}
			rbuf<tword> vcond=v.sub(i+1,pos);
			ybase::clear_word_val(v,i,pos);
			pos++;
			rbool first=true;
			int left;
			int right;
			while(pos<v.count())
			{
				if(v[pos]!=rskey(c_case)&&v[pos]!=rskey(c_else))
				{
					break;
				}
				left=v.find(rsoptr(c_bbk_l),pos);
				if(left>=v.count())
				{
					rserror(v[pos]);
					return false;
				}
				right=ybase::find_symm_bbk(sh,v,left);
				if(right>=v.count())
				{
					rserror(v[pos]);
					return false;
				}
				rbuf<tword> val;
				if(v[left-1]==rsoptr(c_colon))
				{
					val=v.sub(pos+1,left-1);
				}
				else
				{
					val=v.sub(pos+1,left);
				}
				if(v[pos]==rskey(c_case))
				{
					if(first)
					{
						v[pos].multi.push(rskey(c_if));
						first=false;
					}
					else
					{
						v[pos].multi.push(rskey(c_else));
						v[pos].multi.push(rskey(c_if));
					}
					v[pos].multi+=ybase::vword_to_vstr(val);
					v[pos].multi.push(rsoptr(c_equal_equal));
					v[pos].multi.push(rsoptr(c_sbk_l));
					v[pos].multi+=ybase::vword_to_vstr(vcond);
					v[pos].multi.push(rsoptr(c_sbk_r));
				}
				else
				{
					v[pos].multi.push(rskey(c_else));
				}
				ybase::clear_word_val(v,pos,left);
				pos=right+1;
			}
		}
		ybase::arrange(v);
		return true;
	}

	static rbool replace_return(const tsh& sh,rbuf<tword>& v,const tfunc& tfi)
	{
		rbuf<tword> result;
		for(int i=0;i<v.count();i++)
		{
			if(v[i]!=rskey(c_return))
			{
				result.push(v[i]);
				continue;
			}
			int pos=v.find(rsoptr(c_semi),i);
			if(pos>=v.count())
			{
				return false;
			}
			rbuf<tword> sent=v.sub(i+1,pos);
			ifn(sent.empty())
			{
				if(ybase::is_quote(tfi.retval.type))
				{
					//todo: 应放在后端处理
					if(sh.m_mode==tsh::c_gpp)
					{
						result+=sent;
						result+=rsoptr(c_semi);
					}
					else
					{
						result+=rsoptr(c_addr);
						result+=rsoptr(c_sbk_l);
						result+=sent;
						result+=rsoptr(c_sbk_r);
						result+=rsoptr(c_semi);

						result+=rskey(c_mov);
						result+=rsoptr(c_mbk_l);
						result+=rskey(c_ebp);
						result+=rsoptr(c_plus);
						result+=rskey(c_s_off);
						result+=tfi.retval.name;
						result+=rsoptr(c_mbk_r);
						result+=rsoptr(c_comma);
						result+=rskey(c_ebx);
						result+=rsoptr(c_semi);
					}
				}
				else
				{
					result+=tfi.retval.name;
					result+=rsoptr(c_sbk_l);
					result+=rsoptr(c_sbk_l);
					result+=sent;
					result+=rsoptr(c_sbk_r);
					result+=rsoptr(c_sbk_r);
					result+=rsoptr(c_semi);
				}
			}
			result+=rskey(c_jmp);
			result+=rstr("_func_end");
			result+=rsoptr(c_semi);
			i=pos;
		}
		v=result;
		return true;
	}

	static rbool replace_for(const tsh& sh,rbuf<tword>& v,int& id)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i]!=rskey(c_for))
			{
				continue;
			}
			int left=v.find(rsoptr(c_bbk_l),i);
			if(left>=v.count())
			{
				return false;
			}
			int right=ybase::find_symm_bbk(sh,v,left);
			if(right>=v.count())
			{
				return false;
			}
			rbuf<rbuf<tword> > vcond;
			if(v.get(i+1)==rsoptr(c_sbk_l)&&v.get(left-1)==rsoptr(c_sbk_r))
			{
				vcond=r_split_e_a<tword>(
					v.sub(i+2,left-1),tword(rsoptr(c_semi)));
			}
			else
			{
				vcond=r_split_e_a<tword>(
					v.sub(i+1,left),tword(rsoptr(c_semi)));
			}
			if(vcond.empty())
			{
				vcond+=rbuf<tword>(1,tword(rstr("1")));//todo:
			}
			if(vcond.count()==1)
			{
				vcond.push(rbuf<tword>());
				vcond.push_front(rbuf<tword>());
			}
			if(vcond.count()!=3)
			{
				return false;
			}
			int cur_id=id;
			id++;
			rbuf<tword> center=v.sub(left+1,right);
			ifn(replace_break_continue(sh,center,cur_id))
			{
				return false;
			}
			rbuf<tword> result;
			result+=v.sub(0,i);
			result+=vcond[0];
			result+=rsoptr(c_semi);
			result+=rstr("_for_judge_")+cur_id;
			result+=rsoptr(c_colon);
			result+=rsoptr(c_semi);
			result+=vcond[1];
			result+=rsoptr(c_semi);
			result+=rstr("jebxz");
			result+=rstr("_for_end_")+cur_id;
			result+=rsoptr(c_semi);
			ifn(replace_for(sh,center,id))
			{
				return false;
			}
			result+=center;
			result+=rstr("_for_add_")+cur_id;
			result+=rsoptr(c_colon);
			result+=rsoptr(c_semi);
			result+=vcond[2];
			result+=rsoptr(c_semi);
			result+=rstr("jmp");
			result+=rstr("_for_judge_")+cur_id;
			result+=rsoptr(c_semi);
			result+=rstr("_for_end_")+cur_id;
			result+=rsoptr(c_colon);
			result+=rsoptr(c_semi);
			center=v.sub(right+1);
			ifn(replace_for(sh,center,id))
			{
				return false;
			}
			result+=center;
			v=result;
			return true;
		}
		return true;
	}

	static rbool replace_break_continue(const tsh& sh,rbuf<tword>& v,int cur_id)
	{
		rbuf<tword> result;
		for(int i=0;i<v.count();i++)
		{
			if(v[i]==rskey(c_for))
			{
				int left=v.find(rsoptr(c_bbk_l),i);
				if(left>=v.count())
				{
					return false;
				}
				int right=ybase::find_symm_bbk(sh,v,left);
				if(right>=v.count())
				{
					return false;
				}
				result+=v.sub(i,right+1);
				i=right;
			}
			elif(v[i]==rskey(c_break))
			{
				result+=rstr("jmp");
				result+=rstr("_for_end_")+cur_id;
			}
			elif(v[i]==rskey(c_continue))
			{
				result+=rstr("jmp");
				result+=rstr("_for_add_")+cur_id;
			}
			else
			{
				result+=v[i];
			}
		}
		v=result;
		return true;
	}

	static rbool add_else(const tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i]!=rskey(c_if))
			{
				continue;
			}
			int left=v.find(rsoptr(c_bbk_l),i);
			if(left>=v.count())
			{
				return false;
			}
			int right=ybase::find_symm_bbk(sh,v,left);
			if(right>=v.count())
			{
				return false;
			}
			if(v.get(right+1)==rskey(c_else))
			{
				continue;
			}
			rbuf<tword> result;
			result+=v.sub(0,left+1);
			rbuf<tword> temp=v.sub(left+1,right);
			ifn(add_else(sh,temp))
			{
				return false;
			}
			result+=temp;
			result+=rsoptr(c_bbk_r);
			result+=rskey(c_else);
			result+=rsoptr(c_bbk_l);
			result+=rsoptr(c_bbk_r);
			temp=v.sub(right+1);
			ifn(add_else(sh,temp))
			{
				return false;
			}
			result+=temp;
			v=result;
			return true;
		}
		return true;
	}

	static rbool replace_elif(const tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i]!=rskey(c_if))
			{
				continue;
			}
			int left=v.find(rsoptr(c_bbk_l),i);
			if(left>=v.count())
			{
				return false;
			}
			int right=ybase::find_symm_bbk(sh,v,left);
			if(right>=v.count())
			{
				return false;
			}
			if(v.get(right+1)!=rskey(c_else)||
				v.get(right+2)!=rskey(c_if))
			{
				continue;
			}
			rbuf<tword> result;
			result+=v.sub(0,left+1);
			rbuf<tword> temp=v.sub(left+1,right);
			ifn(replace_elif(sh,temp))
			{
				return false;
			}
			result+=temp;
			int first=v.find(rsoptr(c_bbk_l),right+2);
			if(first>=v.count())
			{
				return false;
			}
			int second=ybase::find_symm_bbk(sh,v,first);
			if(second>=v.count())
			{
				return false;
			}
			while(v.get(second+1)==rskey(c_else))
			{
				if(v.get(second+2)==rsoptr(c_bbk_l))
				{
					first=second+2;
					second=ybase::find_symm_bbk(sh,v,first);
					break;
				}
				first=v.find(rsoptr(c_bbk_l),second+2);
				second=ybase::find_symm_bbk(sh,v,first);
			}
			result+=rsoptr(c_bbk_r);
			result+=rskey(c_else);
			result+=rsoptr(c_bbk_l);
			temp=v.sub(right+2,second+1);
			ifn(replace_elif(sh,temp))
			{
				return false;
			}
			result+=temp;
			result+=rsoptr(c_bbk_r);
			temp=v.sub(second+1);
			ifn(replace_elif(sh,temp))
			{
				return false;
			}
			result+=temp;
			v=result;
			return true;
		}
		return true;
	}

	static rbool replace_if(const tsh& sh,rbuf<tword>& v,int& id)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i]!=rskey(c_if))
			{
				continue;
			}
			int left=v.find(rsoptr(c_bbk_l),i);
			if(left>=v.count())
			{
				return false;
			}
			int right=ybase::find_symm_bbk(sh,v,left);
			if(right>=v.count())
			{
				return false;
			}
			if(v.get(right+1)!=rskey(c_else))
			{
				continue;
			}
			rbuf<tword> result;
			int cur_id=id;
			id++;
			result+=v.sub(0,i);
			result+=v.sub(i+1,left);
			result+=rsoptr(c_semi);
			result+=rstr("jebxz");
			result+=rstr("_else_start_")+cur_id;
			result+=rsoptr(c_semi);
			rbuf<tword> temp=v.sub(left+1,right);
			ifn(replace_if(sh,temp,id))
			{
				return false;
			}
			result+=temp;
			result+=rstr("jmp");
			result+=rstr("_else_end_")+cur_id;
			result+=rsoptr(c_semi);
			result+=rstr("_else_start_")+cur_id;
			result+=rsoptr(c_colon);
			result+=rsoptr(c_semi);
			left=right+2;
			right=ybase::find_symm_bbk(sh,v,left);
			temp=v.sub(left+1,right);
			ifn(replace_if(sh,temp,id))
			{
				return false;
			}
			result+=temp;
			result+=rstr("_else_end_")+cur_id;
			result+=rsoptr(c_colon);
			result+=rsoptr(c_semi);
			temp=v.sub(right+1);
			ifn(replace_if(sh,temp,id))
			{
				return false;
			}
			result+=temp;
			v=result;
			return true;
		}
		return true;
	}
};
