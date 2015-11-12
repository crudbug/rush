import xlib/gpp/xbase.rs

namespace xf
{
	enum
	{
		X_SEEK_SET=0,
		X_SEEK_END=2,
	};

	final void print(const char* s)
	{
		printf("%s",s);
	}

	final void vsnprintf(char* dst,int count,const char* f,char* args)
	{
		_vsnprintf((CHAR*)dst,count,(CHAR*)f,(va_list)args);
	}

	final void sscanf(const char* src,const char* f,uint* n)
	{
		sscanf((CHAR*)src,(CHAR*)f,n);
	}
	
	final void sprintf(char* dst,char* format,int n)
	{
		push n
		push format
		push dst
		calle "sprintf"
		add esp,12
	}

	final void sprintf(char* dst,char* format,uint n)
	{
		push n
		push format
		push dst
		calle "sprintf"
		add esp,12
	}
	
	final void sprintf(char* dst,char* format,int64 n)
	{
		lea esi,[ebp+s_off n]
		push [esi+4]
		push [esi]
		push format
		push dst
		calle "sprintf"
		add esp,16
	}

	final void sprintf(char* dst,char* format,double n)
	{
		sprintf(dst,format,n);
	}

	final void itoa(void* dst,int n)
	{
		sprintf((CHAR*)dst,"%d",n);
	}

	final void utoa(void* dst,uint n)
	{
		sprintf((CHAR*)dst,"%u",n);
	}

	final void dtoa(void* dst,double n)
	{
		sprintf((CHAR*)dst,"%f",n);
	}

	final void int64toa(void* dst,int64 n)
	{
		sprintf((CHAR*)dst,"%I64d",n);
	}

	int atoi(char* s)
	{
		int result;
		sscanf(s,"%d",&result);
		return result;
	}

	uint atou(char* s)
	{
		uint result;
		sscanf(s,"%u",&result);
		return result;
	}

	double atod(char* s)
	{
		double result;
		sscanf(s,"%lf",&result);
		return result;
	}

	int64 atoint64(char* s)
	{
		int64 result;
		sscanf(s,"%I64d",&result);
		return result;
	}

	final void exit(int a)
	{
		exit(a);
	}

	final uchar* malloc(int size)
	{
		/*uchar* temp=malloc(size);
		printf("malloc %x %d\n",temp,size);
		return temp;*/
		//printf("malloc %d\n",size);
		return malloc(size);
	}

	final void free(void* p)
	{
		//printf("free %x\n",p);
		free(p);
	}

	final void memcpy(void* dst,const void* src,int size)
	{
		memcpy(dst,src,size);
	}

	final int strlen(const char* s)
	{
		return strlen((CHAR*)s);
	}

	int strlenw(const wchar* p)
	{
		int ret=0;
		while(*p!=0)
		{
			ret++;
			p++;
		}
		return ret;
	}

	final int tick()
	{
		return GetTickCount();
	}

	final void sleep(int milliseconds)
	{
		push milliseconds
		calle "Sleep"
	}

	final int getch()
	{
		return getch();
	}

	/*static void srand()
	{
		::srand((uint)::time(null));
	}

	static int rand()
	{
		return ::rand();
	}*/

	final int cmd(const char* p)
	{
		push p
		calle "system"
		mov s_ret,eax
		add esp,4
	}

	final int get_exe_name(wchar* buf,int size)
	{
		return GetModuleFileNameW(0,(wchar_t*)buf,size);
	}

	final int get_cur_dir(wchar* buf,int size)
	{
		return GetCurrentDirectoryW(size,(wchar_t*)buf);
	}

	/*static int64 get_file_update_time(wchar* path)
	{
#ifdef _MSC_VER
		int64 ft;
		HANDLE hFile=::CreateFileW(path,
			GENERIC_READ,
			FILE_SHARE_READ,
			null,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			null);
		if(hFile==INVALID_HANDLE_VALUE)
			return 0;
		if(!::GetFileTime(hFile,null,null,(FILETIME*)&ft))
			return 0;
		::CloseHandle(hFile);
		return ft;
#else
		return 0;
		/*struct stat buf;
		int ret=stat(path,&buf);
		if(0!=ret)
		{
			return false;
		}
		*ft=(int64)buf.st_mtime;
		return true;*/
#endif
	}*/

	final int get_exe_size(uchar* data)
	{
		/*PIMAGE_DOS_HEADER dos_head=(PIMAGE_DOS_HEADER)data;
		PIMAGE_OPTIONAL_HEADER op_head=(PIMAGE_OPTIONAL_HEADER)(
			data+dos_head->e_lfanew+4+20);
		PIMAGE_FILE_HEADER file_head=(PIMAGE_FILE_HEADER)(
			data+dos_head->e_lfanew+4);
		PIMAGE_SECTION_HEADER sec=(PIMAGE_SECTION_HEADER)(
			data+dos_head->e_lfanew+r_size(IMAGE_NT_HEADERS));
		int size=op_head->SizeOfHeaders;
		for(int i=0;i<file_head->NumberOfSections;i++)
		{
			size+=sec[i].SizeOfRawData;
		}
		return size;*/
	}
	
	final int ftell(void* fp)
	{
		return ftell((FILE*)fp);
	}

	int64 ftell64(void* fp)
	{
		return ftell(fp)
	}
	
	final int fseek(void* fp,int off,int start)
	{
		return fseek((FILE*)fp,off,start);
	}

	int fseek64(void* fp,int64 off,int start)
	{
		return fseek(fp,off.toint,start)
	}

	final void* _wfopen(wchar* name,wchar* mode)
	{
		return _wfopen((wchar_t*)name,(wchar_t*)mode);
	}

	final int fclose(void* fp)
	{
		return fclose((FILE*)fp);
	}

	final int fread(void* buf,int size,int count,void* fp)
	{
		return fread(buf,size,count,(FILE*)fp);
	}

	final int fwrite(const void* buf,int size,int count,void* fp)
	{
		return fwrite(buf,size,count,(FILE*)fp);
	}

	final int _wremove(wchar* name)
	{
		return _wremove((wchar_t*)name);
	}

	final int rename(wchar* oldname,wchar* newname)
	{
		push newname
		push oldname
		calle "_wrename"
		add esp,8
		mov s_ret,eax
		//return 0==::_wrename(oldname,newname);
	}

	final int create_thr(void* start,void* param=null)
	{
		int tid=_beginthreadex(null,0,start,param,0,null);
		if(0==tid||1==tid)
		{
			return 0;
		}
		else
		{
			return tid;
		}
	}

	final void wait_thr(int tid)
	{
		WaitForSingleObject(tid,-1);
	}

	final int gbk_to_utf16(const char* src,wchar* dst,int count)
	{
		return MultiByteToWideChar(936,0,(CHAR*)src,-1,(wchar_t*)dst,count);
	}

	final int utf16_to_gbk(const wchar* src,char* dst,int buf_size)
	{
		return WideCharToMultiByte(936,0,(wchar_t*)src,-1,(CHAR*)dst,buf_size,0,0);
	}

	final uchar* alloc_v(int len)
	{
		return 0;
#ifdef _MSC_VER
		return (uchar*)VirtualAlloc(null,len,
			MEM_COMMIT,PAGE_EXECUTE_READWRITE);
#endif
	}

	final void free_v(uchar* addr)
	{
#ifdef _MSC_VER
		VirtualFree(addr,0,MEM_RELEASE);
#endif
	}
	
	final char* GetCommandLineA()
	{
		return GetCommandLineA();
	}
	
	final void* FindFirstFileW(wchar* name,void* wfd)
	{
		return FindFirstFileW((wchar_t*)name,(WIN32_FIND_DATAW*)wfd);
	}

	final rbool FindNextFileW(void* handle,void* wfd)
	{
		return FindNextFileW(handle,(WIN32_FIND_DATAW*)wfd);
	}

	final void FindClose(void* handle)
	{
		FindClose(handle);
	}
	
	final void InitializeCriticalSection(void* p)
	{
		push p
		calle "InitializeCriticalSection"
	}
	
	final void DeleteCriticalSection(void* p)
	{
		push p
		calle "DeleteCriticalSection"
	}
	
	final void EnterCriticalSection(void* p)
	{
		push p
		calle "EnterCriticalSection"
	}
	
	final void LeaveCriticalSection(void* p)
	{
		push p
		calle "LeaveCriticalSection"
	}
	
	final void* find_func(char* p)
	{
		push p
		calle "find_func"
		mov s_ret,eax
		add esp,4
	}
	
	final void* get_stdin()
	{
		calle "__iob_func"
		mov s_ret,eax
	}

	final void* get_stdout()
	{
		calle "__iob_func"
		add eax,32
		mov s_ret,eax
	}
	
	rstr scanl()
	{
		rstr result;
		result.clear();
		char c;
		void* fp=get_stdin();
		while(1==xf::fread(&c,1,1,fp))
		{
			if(13==c.toint||10==c.toint)
			{
				if(0==result.count())
				{
					continue;
				}
				else
				{
					return result;
				}
			}
			result+=c;
		}
		return result;
	}
	
	final int _beginthreadex(void* _Security,int _StackSize,
		void* start,void* _ArgList,int _InitFlag,void* addr)
	{
		push addr
		push _InitFlag
		push _ArgList
		push start
		push _StackSize
		push _Security
		calle "_beginthreadex"
		add esp,24
		mov s_ret,eax
	}
	
	final void WaitForSingleObject(int handle,int milliseconds)
	{
		push milliseconds
		push handle
		calle "WaitForSingleObject"
	}

	final bool CloseHandle(int handle)
	{
		push handle
		calle "CloseHandle"
		mov s_ret,eax
	}
	
	final void* find_dll_func(rstr s)
	{
		return find_dll_func(s.cstr)
	}

	final void* find_dll_func(char* p)
	{
		push p
		calle "find_dll"
		mov s_ret,eax
		add esp,4
	}
}

#define FILE_ATTRIBUTE_DIRECTORY 0x00000010

struct R_WIN32_FIND_DATAW
{
	int dwFileAttributes;
	int64 ftCreationTime;
	int64 ftLastAccessTime;
	int64 ftLastWriteTime;
	int nFileSizeHigh;
	int nFileSizeLow;
	int dwReserved0;
	int dwReserved1;
	wchar cFileName[260];
	wchar cAlternateFileName[14];
};

struct R_CRITICAL_SECTION
{
	char buf[24];
};
