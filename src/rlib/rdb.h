﻿#pragma once

#include "rfile.h"

#define rdbint rdb<int,int>

//TL是数据长度，TA是地址
template<typename TA,typename TL>//uchar,ushort,uint,uint64
struct rdb
{
	rfile m_file;//use multi thread please define multi object
	rbuf<TA> m_index;

	enum
	{
		c_null,//空数据
		c_real,
		c_cp,//压缩数据
		c_error//异常数据
	};

	rdb()
	{
	}

	rdb(rstr name,rstr mode=rstr("r"))
	{
		open(name,mode);
	}

	rbool close()
	{
		return m_file.close();
	}

	rbool open_off(rstr name,int off,rstr mode="r")
	{
		if(!m_file.open(name,mode))
		{
			return false;
		}
		m_file.set_off(off);
		TL count;
		if(!m_file.read(r_size(TL),&count))
		{
			return false;
		}
		TL cmax;
		if(!m_file.read(r_size(TL),&cmax))//todo: r_size(cmax)
		{
			return false;
		}
		m_index.realloc_n_not_change(cmax);
		if(!m_file.read(count*r_size(TA),m_index.begin()))
		{
			return false;
		}
		m_index.m_count=count;
		for(int i=0;i<m_index.count();i++)
		{
			m_index[i]+=off;
		}
		return true;
	}

	rbool open(rstr name,rstr mode="r")
	{
		if(!rfile::exist(name))
		{
			if(!m_file.open_n(name,"rw"))
			{
				return false;
			}
			TL count=0;
			if(!m_file.write(r_size(TL),&count))
			{
				return false;
			}
			if(!m_file.write(r_size(TL),&count))
			{
				return false;
			}
			if(!m_file.close())
			{
				return false;
			}
		}
		if(!m_file.open(name,mode))
		{
			return false;
		}
		TL count;
		if(!m_file.read(r_size(TL),&count))
		{
			return false;
		}
		TL cmax;
		if(!m_file.read(r_size(TL),&cmax))
		{
			return false;
		}
		m_index.realloc_n_not_change(cmax);
		if(!m_file.read(count*r_size(TA),m_index.begin()))
		{
			return false;
		}
		m_index.m_count=count;
		return true;
	}

	int count() const
	{
		return m_index.count();
	}

	rstr operator[](int i)
	{
		rstr ret;
		if(!read(i,ret))
		{
			null;
		}
		return r_move(ret);
	}
	
	rbool read(int i,rstr& data)
	{
		data.clear();
		if(i>=m_index.count()||i<0)
		{
			return false;
		}
		if(m_index[i]==0)
		{
			return true;
		}
		char flag;
		TA off=m_index[i];
		if(!m_file.read(off,r_size(char),&flag))
		{
			return false;
		}
		if(flag==c_null)
		{
			return true;
		}
		if(flag!=c_real)
		{
			return false;
		}
		TL len;
		off+=r_size(char);
		if(!m_file.read(off,r_size(TL),&len))
		{
			return false;
		}
		data.m_buf.realloc_n(len);
		off+=r_size(TL);
		if(!m_file.read(off,data.count(),data.begin()))
		{
			return false;
		}
		return true;
	}

	rbool write(int i,const rstr& data)
	{
		if(i>=m_index.count()||i<0)
		{
			return false;
		}
		char flag=c_null;
		TA off=m_index[i];
		if(!m_file.write(off,r_size(char),&flag))
		{
			return false;
		}
		flag=c_real;
		off=m_file.size();
		m_index[i]=off;
		if(!m_file.write(off,r_size(char),&flag))
		{
			return false;
		}
		off+=r_size(char);
		TL len=data.count();
		if(!m_file.write(off,r_size(TL),&len))
		{
			return false;
		}
		off+=r_size(TL);
		if(!m_file.write(off,data.count(),data.begin()))
		{
			return false;
		}
		if(!m_file.write(get_index_off(i),r_size(TA),&m_index[i]))
		{
			return false;
		}
		return true;
	}

	rbool write_new(const rstr& data)
	{
		if(m_index.m_count>=m_index.m_max)
		{
			if(!extend())
			{
				return false;
			}
		}
		TA off;
		off=m_file.size();
		m_index.push(off);
		char flag=c_real;
		if(!m_file.write(off,r_size(char),&flag))
		{
			return false;
		}
		off+=r_size(char);
		TL len=data.count();
		if(!m_file.write(off,r_size(TL),&len))
		{
			return false;
		}
		off+=r_size(TL);
		if(!m_file.write(off,data.count(),data.begin()))
		{
			return false;
		}
		int i=m_index.count()-1;
		if(!m_file.write(get_index_off(i),r_size(TA),&m_index[i]))
		{
			return false;
		}
		if(!m_file.write(0,r_size(TL),&m_index.m_count))
		{
			return false;
		}
		return true;
	}

	rbool extend()
	{
		if(m_file.size()>=128*1024*1024)
		{
			return false;
		}
		rbuf<uchar> temp;
		temp.alloc(m_file.size()-get_data_off());
		if(!m_file.read(get_data_off(),temp.size(),temp.begin()))
		{
			return false;
		}
		int cmax=m_index.m_max;
		m_index.realloc_not_change(m_index.extend_num(m_index.count()));
		for(int i=0;i<m_index.count();i++)
		{
			m_index[i]+=(m_index.m_max-cmax)*r_size(TA);
		}
		if(!m_file.write(r_size(TL),r_size(TL),&m_index.m_max))
		{
			return false;
		}
		//这里必须用m_cmax,不能用m_count，因为刚开始只有8个字节
		if(!m_file.write(r_size(TL)*2,
			m_index.m_max*r_size(TA),m_index.begin()))
		{
			return false;
		}
		if(!m_file.write(get_data_off(),temp.size(),temp.begin()))
		{
			return false;
		}
		return true;
	}

	int find(rstr s) const
	{
		for(int i=0;i<count();i++)
		{
			if(operator[](i)==s)
			{
				return i;
			}
		}
		return count();
	}

	TA get_data_off() const
	{
		return m_index.m_max*r_size(TA)+r_size(TL)*2;
	}

	static int get_index_off(int i)
	{
		return r_size(TL)*2+r_size(TA)*i;
	}
};
