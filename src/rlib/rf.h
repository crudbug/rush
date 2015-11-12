#pragma once

#include "../xlib/xfs.h"

struct rf
{
	static void printl(rstr s=rstr())
	{
		print(s+"\n");
	}

	static void print(rstr s=rstr())
	{
		xf::print((char*)(s.cstr()));
	}

	static int cmd_utf8(rstr s)
	{
		return xf::cmd(rcode::utf8_to_gbk(s).cstr_t());
	}

	static int cmd(rstr s)
	{
		return xf::cmd(s.cstr_t());
	}

	static rbuf<rstr> get_param()
	{
		return r_split<rstr>(xf::GetCommandLineA()," ");
	}

	static rbuf<rstr> param_split(rstr s)
	{
		rbuf<rstr> v;
		int j;
		for(int i=0;i<s.count();i++)
		{
			if(s[i]==r_char('\"'))
			{
				j=s.find("\"",i+1);
				v.push(s.sub(i+1,j));
				i=j;
			}
			elif(s[i]==r_char(' '))
			{
				continue;
			}
			else
			{
				for(j=i;j<s.count();j++)
				{
					if(s[j]==r_char(' '))
					{
						break;
					}
				}
				v.push(s.sub(i,j));
				i=j;
			}
		}
		return r_move(v);
	}

	static rbuf<rstr> vstr(const rstr& s1)
	{
		rbuf<rstr> result;
		result+=s1;
		return result;
	}

	static rbuf<rstr> vstr(const rstr& s1,const rstr& s2)
	{
		rbuf<rstr> result;
		result+=s1;
		result+=s2;
		return result;
	}

	static rbuf<rstr> vstr(const rstr& s1,const rstr& s2,
		const rstr& s3)
	{
		rbuf<rstr> result;
		result+=s1;
		result+=s2;
		result+=s3;
		return result;
	}

	static rbuf<rstr> vstr(const rstr& s1,const rstr& s2,
		const rstr& s3,const rstr& s4)
	{
		rbuf<rstr> result;
		result+=s1;
		result+=s2;
		result+=s3;
		result+=s4;
		return result;
	}

	static rbuf<rstr> vstr(const rstr& s1,const rstr& s2,
		const rstr& s3,const rstr& s4,const rstr& s5,const rstr& s6)
	{
		rbuf<rstr> result;
		result+=s1;
		result+=s2;
		result+=s3;
		result+=s4;
		result+=s5;
		result+=s6;
		return result;
	}

	static rbuf<rstr> vstr(const rstr& s1,const rstr& s2,
		const rstr& s3,const rstr& s4,const rstr& s5,const rstr& s6,
		const rstr& s7)
	{
		rbuf<rstr> result;
		result+=s1;
		result+=s2;
		result+=s3;
		result+=s4;
		result+=s5;
		result+=s6;
		result+=s7;
		return result;
	}

	static rbuf<rstr> vstr(const rstr& s1,const rstr& s2,
		const rstr& s3,const rstr& s4,const rstr& s5,const rstr& s6,
		const rstr& s7,const rstr& s8)
	{
		rbuf<rstr> result;
		result+=s1;
		result+=s2;
		result+=s3;
		result+=s4;
		result+=s5;
		result+=s6;
		result+=s7;
		result+=s8;
		return result;
	}

	static rbuf<rstr> vstr(const rstr& s1,
		const rstr& s2,const rstr& s3,const rstr& s4,
		const rstr& s5,const rstr& s6,const rstr& s7,const rstr& s8,
		const rstr& s9,const rstr& s10)
	{
		rbuf<rstr> result;
		result+=s1;
		result+=s2;
		result+=s3;
		result+=s4;
		result+=s5;
		result+=s6;
		result+=s7;
		result+=s8;
		result+=s9;
		result+=s10;
		return result;
	}

	static rbuf<rstr> vstr(const rstr& s1,
		const rstr& s2,const rstr& s3,const rstr& s4,
		const rstr& s5,const rstr& s6,const rstr& s7,const rstr& s8,
		const rstr& s9,const rstr& s10,const rstr& s11,const rstr& s12)
	{
		rbuf<rstr> result;
		result+=s1;
		result+=s2;
		result+=s3;
		result+=s4;
		result+=s5;
		result+=s6;
		result+=s7;
		result+=s8;
		result+=s9;
		result+=s10;
		result+=s11;
		result+=s12;
		return result;
	}
};
