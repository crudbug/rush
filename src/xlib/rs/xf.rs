import xbase.rs

namespace xf
{
	enum
	{
		X_SEEK_SET=0,
		X_SEEK_END=2,
	};

	static void print(const char* s)
	{
		push s
		push "%s"
		calle "printf",8
		add esp,8
	}

	static void vsnprintf(char* dst,int count,const char* f,char* args)
	{
		push args
		push f
		push count
		push dst
		calle "_vsnprintf",16
		add esp,16
	}

	static void sscanf(const char* src,const char* f,uint* n)
	{
		push n
		push f
		push src
		calle "sscanf",12
		add esp,12
	}
	
	void sprintf(char* dst,char* format,int n)
	{
		push n
		push format
		push dst
		calle "sprintf",12
		add esp,12
	}

	void sprintf(char* dst,char* format,uint n)
	{
		push n
		push format
		push dst
		calle "sprintf",12
		add esp,12
	}
	
	void sprintf64(char* dst,char* format,int64 n)
	{
		lea esi,[ebp+s_off n]
		push [esi+4]
		push [esi]
		push format
		push dst
		calle "sprintf",16
		add esp,16
	}

	void sprintf64(char* dst,char* format,double n)
	{
		lea esi,[ebp+s_off n]
		push [esi+4]
		push [esi]
		push format
		push dst
#ifdef _RNASM
		calle "sprintf",16
#else
		calle "sprintf64",16
#endif
		add esp,16
	}

	static void itoa(void* dst,int n)
	{
		push n
		push "%d"
		push dst
		calle "sprintf",12
		add esp,12
	}

	static void utoa(void* dst,uint n)
	{
		push n
		push "%u"
		push dst
		calle "sprintf",12
		add esp,12
	}

	static void dtoa(void* dst,double n)
	{
		sprintf64(dst,"%f",n);
	}

	static void int64toa(void* dst,int64 n)
	{
#ifdef _RNASM
		sprintf64(dst,"%I64d",n);
#else
		lea esi,[ebp+s_off n]
		push [esi+4]
		push [esi]
		push dst
		calle "int64toa",12
		add esp,12
#endif
	}

	static int atoi(char* s)
	{
		int result;
		sscanf(s,"%d",&result);
		return result;
	}

	static uint atou(char* s)
	{
		uint result;
		sscanf(s,"%u",&result);
		return result;
	}

	static double atod(char* s)
	{
		double result;
		sscanf(s,"%lf",&result);
		return result;
	}

	static void atoint64(char* s,int64* result)
	{
		push result
		push s
		calle "atoint64",8
		add esp,8
	}

	static void exit(int a)
	{
		push a
		calle "exit",4
		add esp,4
	}

	static uchar* malloc(int size)
	{
		push size
		calle "malloc",5
		mov s_ret,eax
		add esp,4
	}

	static void free(void* p)
	{
		push p
		calle "free",4
		add esp,4
	}

	static void memcpy(void* dst,const void* src,int size)
	{
		push size
		push src
		push dst
		calle "memcpy",12
		add esp,12
	}

	static int strlen(const char* s)
	{
		push s
		calle "strlen",5
		mov s_ret,eax
		add esp,4
	}

	static int strlenw(const wchar* p)
	{
		int ret=0;
		while(*p!=0)
		{
			ret++;
			p++;
		}
		return ret;
	}

	static int tick()
	{
		calle "GetTickCount",1
		mov s_ret,eax
	}

	static void sleep(int milliseconds)
	{
		push milliseconds
		calle "Sleep",4
		add esp,4
	}

	static int getch()
	{
		calle "_getch",1
		mov s_ret,eax
	}

	/*static void srand()
	{
		::srand((uint)::time(null));
	}

	static int rand()
	{
		return ::rand();
	}*/

	static int cmd(const char* p)
	{
		push p
		calle "system",5
		mov s_ret,eax
		add esp,4
	}

	static int get_exe_name(wchar* buf,int size)
	{
		push size
		push buf
		push 0
		calle "GetModuleFileNameW",13
		mov s_ret,eax
		//return GetModuleFileNameW(null,buf,size)!=0;
	}

	static int get_cur_dir(wchar* buf,int size)
	{
		push buf
		push size
		calle "GetCurrentDirectoryW",9
		mov s_ret,eax
		//return GetCurrentDirectoryW(size,buf)!=0;
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

	static int get_exe_size(uchar* data)
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
	
	static int ftell(void* fp)
	{
		push fp
		calle "ftell",5
		add esp,4
		mov s_ret,eax
	}

	static int64 ftell64(void* fp)
	{
		return ftell(fp)
	}
	
	static int fseek(void* fp,int off,int start)
	{
		push start
		push off
		push fp
		calle "fseek",13
		add esp,12
		mov s_ret,eax
	}

	static int fseek64(void* fp,int64 off,int start)
	{
		return fseek(fp,off.toint,start)
	}

	static void* _wfopen(wchar* name,wchar* mode)
	{
		push mode
		push name
		calle "_wfopen",9
		mov s_ret,eax
		add esp,8
	}

	static int fclose(void* fp)
	{
		push fp
		calle "fclose",5
		add esp,4
		mov s_ret,eax
	}

	static int fread(void* buf,int size,int count,void* fp)
	{
		push fp
		push count
		push size
		push buf
		calle "fread",17
		add esp,16
		mov s_ret,eax
	}

	static int fwrite(const void* buf,int size,int count,void* fp)
	{
		push fp
		push count
		push size
		push buf
		calle "fwrite",17
		add esp,16
		mov s_ret,eax
	}

	static int _wremove(wchar* name)
	{
		push name
		calle "_wremove",5
		add esp,4
		mov s_ret,eax
	}

	/*static int rename(wchar* oldname,wchar* newname)
	{
		push newname
		push oldname
		calle "_wrename",9
		add esp,8
		mov s_ret,eax
	}*/

	static int create_thr(void* start,void* param=null)
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

	static void wait_thr(int tid)
	{
		WaitForSingleObject(tid,-1);
	}

	static int gbk_to_utf16(const char* src,wchar* dst,int count)
	{
		push count
		push dst
		push (-1)
		push src
		push 0
		push 936
		calle "MultiByteToWideChar",25
		mov s_ret,eax
#ifndef _RNASM
		add esp,24
#endif
	}

	static int utf16_to_gbk(const wchar* src,char* dst,int buf_size)
	{
		push 0
		push 0
		push buf_size
		push dst
		push (-1)
		push src
		push 0
		push 936
		calle "WideCharToMultiByte"
		mov s_ret,eax

		//return ::WideCharToMultiByte(936,0,src,-1,dst,buf_size,null,null);
	}

	static uchar* alloc_v(int len)
	{
#ifdef _MSC_VER
		return (uchar*)VirtualAlloc(null,len,
			MEM_COMMIT,PAGE_EXECUTE_READWRITE);
#endif
	}

	static void free_v(uchar* addr)
	{
#ifdef _MSC_VER
		VirtualFree(addr,0,MEM_RELEASE);
#endif
	}
	
	static char* GetCommandLineA()
	{
		calle "GetCommandLineA",1
		mov s_ret,eax
	}
	
	static void* FindFirstFileW(wchar* name,void* wfd)
	{
		push wfd
		push name
		calle "_FindFirstFileW",9
		mov s_ret,eax
#ifndef _RNASM
		add esp,8
#endif
	}

	static rbool FindNextFileW(void* handle,void* wfd)
	{
		push wfd
		push handle
		calle "_FindNextFileW",9
		mov s_ret,eax
#ifndef _RNASM
		add esp,8
#endif
	}

	static void FindClose(void* handle)
	{
		push handle
		calle "_FindClose",4
#ifndef _RNASM
		add esp,4
#endif
	}
	
	static void InitializeCriticalSection(void* p)
	{
		push p
		calle "InitializeCriticalSection"
	}
	
	static void DeleteCriticalSection(void* p)
	{
		push p
		calle "DeleteCriticalSection"
	}
	
	static void EnterCriticalSection(void* p)
	{
		push p
		calle "EnterCriticalSection"
	}
	
	static void LeaveCriticalSection(void* p)
	{
		push p
		calle "LeaveCriticalSection"
	}
	
	static void* find_func(char* p)
	{
		push p
		calle "find_func"
		mov s_ret,eax
		add esp,4
	}
	
	void* get_stdin()
	{
		calle "__iob_func"
		mov s_ret,eax
	}

	void* get_stdout()
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
	
	int _beginthreadex(void* _Security,int _StackSize,
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
	
	void WaitForSingleObject(int handle,int milliseconds)
	{
		push milliseconds
		push handle
		calle "WaitForSingleObject"
	}

	bool CloseHandle(int handle)
	{
		push handle
		calle "CloseHandle"
		mov s_ret,eax
	}
	
	void* find_dll_func(rstr s)
	{
		return find_dll_func(s.cstr)
	}

	void* find_dll_func(char* p)
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
