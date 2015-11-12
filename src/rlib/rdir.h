#pragma once

#include "rstrw.h"
#include "rlist.h"

struct rdir
{
	enum
	{
		c_max_path=260,
		c_file_attr_dir=0x00000010,
	};
	
	static rstrw erase_suffix_w(const rstrw& name)
	{
		rstrw s=get_suffix_w(name);
		return name.sub(0,name.count()-s.count()-1);
	}

	static rstr erase_suffix(const rstr& path)
	{
		return erase_suffix_w(path).torstr();
	}

	static rstrw get_suffix_w(const rstrw& name)
	{
		ushort ch=r_char('.');
		int pos=name.find_last(ch);
		if(pos<name.count())
		{
			return name.sub(pos+1);
		}
		return rstrw();
	}

	static rstr get_suffix(const rstr& name)
	{
		return get_suffix_w(name).torstr();
	}
	
	static rstrw get_prev_dir_w(rstrw path)
	{
		if(path.empty())
		{
			return rstrw();
		}
		path.pop();
		ushort ch=r_char('/');
		int pos=path.find_last(ch);
		if(pos>=path.count())
		{
			return rstrw();
		}
		return path.sub(0,pos+1);
	}

	static rstr get_prev_dir(const rstr& path)
	{
		return get_prev_dir_w(path).torstr();
	}

	static rstrw get_name_w(const rstrw& path)
	{
		if(path.empty())
		{
			return rstrw();
		}
		ushort ch=r_char('/');
		int pos=path.find_last(ch);
		if(pos>=path.count())
		{
			return rstrw();
		}
		return path.sub(pos+1);
	}

	static rstr get_name(const rstr& path)
	{
		return get_name_w(path).torstr();
	}

	static rstrw get_real_name_w(rstrw name)
	{
		rstrw ret;
		wchar ch=r_char('.');
		int pos=name.m_buf.find_last(ch);
		if(pos<name.count())
		{
			ret=name.sub(0,pos);
		}
		return ret;
	}

	static rstr get_real_name(rstr name)
	{
		return get_real_name_w(name).torstr();
	}

	static rstrw get_exe_name_w()
	{
		rstrw ret;
		rbuf<wchar> buf(c_max_path+1);
		ifn(xf::get_exe_name(buf.begin(),c_max_path))
		{
			return ret;
		}
		ret.set_w(buf.begin());
		return dir_std_w(ret);
	}

	static rstr get_exe_name()
	{
		return get_exe_name_w().torstr();
	}

	static rstrw get_exe_dir_w()
	{
		return get_prev_dir_w(get_exe_name_w());
	}

	static rstr get_exe_dir()
	{
		return get_exe_dir_w().torstr();
	}

	static rstrw get_cur_dir_w()
	{
		rstrw ret;
		rbuf<wchar> buf(c_max_path+1);
		ifn(xf::get_cur_dir(buf.begin(),c_max_path))
		{
			return ret;
		}
		ret.set_w(buf.begin());
		ret=dir_std_w(ret);
		ret+=r_char('/');
		return r_move(ret);
	}

	static rstr get_cur_dir()
	{
		return get_cur_dir_w().torstr();
	}
	
	//非win系统区分大小写
	static rstrw dir_std_w(rstrw s)
	{
		for(ushort* p=s.begin();p!=s.end();++p)
		{
			if(r_char('\\')==*p)
			{
				*p=r_char('/');
			}
		}
		return r_move(s);
	}

	static rstr dir_std(const rstr& s)
	{
		return dir_std_w(s).torstr();
	}

	static rstrw dir_std_rev_w(rstrw s)
	{
		for(ushort* p=s.begin();p!=s.end();++p)
		{
			if(r_char('/')==*p)
			{
				*p=r_char('\\');
			}
		}
		return r_move(s);
	}

	static rstr dir_std_rev(const rstr& s)
	{
		return dir_std_rev_w(s).torstr();
	}

	static rbuf<rstr> get_file_bfs(rstr path)
	{
		rbuf<rstr> ret;
		if(path.count()>4096||0==path.count())//ubuntu's max path len is 4096
		{
			return ret;
		}
		path=dir_std(path);
		if(path.count()>=1&&path.get(path.count()-1)!=r_char('/'))
		{
			path+="/";
		}
		rlist<rstr> queue;
		queue.push(path);
		while(!queue.empty())
		{
			path=queue.pop_front();
			R_WIN32_FIND_DATAW wfd;
			void* handle=xf::FindFirstFileW(
				rcode::utf8_to_utf16(path+"*.*").cstrw_t(),&wfd); 
			if((int)handle==-1)
			{
				continue;
			}
			while(true)
			{
				rstr name;
				int wsize=xf::strlenw((wchar*)(&wfd.cFileName))*2;
				name.set_size(wsize);
				xf::memcpy(name.begin(),&wfd.cFileName,wsize);
				name=rcode::utf16_to_utf8(name);
				name=dir_std(name);
				if("."!=name&&".."!=name)
				{
					if(AND(wfd.dwFileAttributes,
						FILE_ATTRIBUTE_DIRECTORY))
					{
						queue.push(path+name+"/");
						ret.push(path+name+"/");
					}
					else
					{
						ret.push(path+name);
					}
				}
				ifn(xf::FindNextFileW(handle,&wfd))
				{
					break;
				}
			}
			xf::FindClose(handle);
		}
		return ret;
	}
};
