#pragma once

#include "ybase.h"

//有格式代码转无格式
struct yformat
{
	static rbool process(tsh& sh)
	{
		for(tfile* p=sh.m_file.begin();p!=sh.m_file.end();p=sh.m_file.next(p))
		{
			//先进行控制结构替换，因为case后面可能有冒号防止被识别为函数
			ifn(control_replace(sh,p->vword))
			{
				return false;
			}
			class_def_replace(sh,p->vword);
			func_def_replace(sh,p->vword);
		}
		return true;
	}

	static int get_sent_end(const tsh& sh,rbuf<tword>& v,int begin,rbool use_semi)
	{
		int count1=0;
		int count2=0;
		for(int i=begin;i<v.count();i++)
		{
			if(v[i].val==rsoptr(c_sbk_l))
			{
				count1++;
			}
			elif(v[i].val==rsoptr(c_sbk_r))
			{
				count1--;
			}
			elif(v[i].val==rsoptr(c_mbk_l))
			{
				count2++;
			}
			elif(v[i].val==rsoptr(c_mbk_r))
			{
				count2--;
			}
			if(count1==0&&count2==0)
			{
				if(v[i].val==rsoptr(c_bbk_l)||v[i].val==rsoptr(c_bbk_r))
				{
					return i-1;
				}
				if(use_semi&&v[i].val==rsoptr(c_semi))
				{
					return i;
				}
				if(v[i].pos!=v.get(i+1).pos)
				{
					return i;
				}
			}
		}
		return v.count()-1;
	}

	static rbool control_replace(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			ifn(v[i].val==rskey(c_for)||v[i].val==rskey(c_if)||v[i].val==rskey(c_else)||
				v[i].val==rskey(c_ifn)||v[i].val==rskey(c_switch)||v[i].val==rskey(c_case))
			{
				continue;
			}
			int k=get_sent_end(sh,v,i,false);
			if(v.get(k+1).val==rsoptr(c_bbk_l))
			{
				continue;
			}
			if(v[k].val==rsoptr(c_colon))
			{
				v[k].val.clear();
			}
			v[k].multi+=v[k].val;
			v[k].multi+=rsoptr(c_bbk_l);
			v[k].val.clear();
			int j=get_low_end(v,k+1,i);
			v[j-1].multi+=v[j-1].val;
			v[j-1].multi+=rsoptr(c_bbk_r);
			v[j-1].val.clear();
			i=j-1;
		}
		if(ybase::arrange(v))
		{
			return control_replace(sh,v);
		}
		return true;
	}

	static void func_def_replace(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			ifn(v[i].val==rsoptr(c_sbk_r)&&v.get(i+1).val==rsoptr(c_colon))
			{
				continue;
			}
			if(v.get(i+2).val==rsoptr(c_bbk_l))
			{
				v[i+1].val.clear();
				continue;
			}
			int j=get_low_end(v,i+2,i+1);
			v[i+1].val=rsoptr(c_bbk_l);
			v[j-1].multi+=v[j-1].val;
			v[j-1].multi+=rsoptr(c_bbk_r);
			v[j-1].val.clear();
			i=j-1;
		}
		if(ybase::arrange(v))
		{
			func_def_replace(sh,v);
		}
	}

	static void class_def_replace(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rskey(c_class))
			{
				continue;
			}
			for(int k=i+1;k<v.count();k++)
			{
				if(v[k].pos!=v[i].pos)
				{
					break;
				}
				if(v[k].val==rsoptr(c_bbk_l))
				{
					break;
				}
				if(v[k].val==rsoptr(c_colon)&&v[k].pos!=v.get(k+1).pos)
				{
					int j=get_low_end(v,k+1,i);
					v[k].val=rsoptr(c_bbk_l);
					v[j-1].multi+=v[j-1].val;
					v[j-1].multi+=rsoptr(c_bbk_r);
					v[j-1].val.clear();
					i=j-1;
					break;
				}
			}
		}
		if(ybase::arrange(v))
		{
			class_def_replace(sh,v);
		}
	}

	//获取低一层结尾语句的位置
	static int get_low_end(rbuf<tword>& v,int begin,int head)
	{
		for(int i=begin;i<v.count();i++)
		{
			if(ybase::get_word_tab(v[i])<=ybase::get_word_tab(v[head]))
			{
				return i;
			}
		}
		return v.count();
	}
};
