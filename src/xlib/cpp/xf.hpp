#pragma once

#include "xbase.hpp"

typedef void (* xthread_start)(void*);

#ifdef _MSC_VER
typedef uint xthread_ret;//32 bit
#define xthread_call __stdcall
typedef uint (__stdcall* pxthread)(void*);
#else
typedef void* xthread_ret;
#define xthread_call
typedef void* (* pxthread)(void*);
#endif

typedef xthread_ret (xthread_call* thread_start)(void*);

struct xf
{
	int argc;
	void* argv;

	enum
	{
		X_SEEK_SET=SEEK_SET,
		X_SEEK_END=SEEK_END,
	};

	static void print(const char* s)
	{
		::printf("%s",s);//必须加上%s防止字符串中出现特殊字符
	}

	static void printf(const char* f,const char* s)
	{
		::printf(f,s);
	}

	static void vsnprintf(char* dst,int count,const char* f,char* args)
	{
#ifdef _MSC_VER
		::_vsnprintf(dst,count,f,args);
#else
#ifdef EMSCRIPTEN
		::vsnprintf(dst,count,f,(int*)args);
#else
		::vsnprintf(dst,count,f,args);
#endif
#endif
	}

	static void sprintf(char* src,const char* f,uint n)
	{
		::sprintf(src,f,n);
	}

	static void sprintf64(char* src,const char* f,double n)
	{
		::sprintf(src,f,n);
	}

	static void sscanf(const char* src,const char* f,uint* n)
	{
		::sscanf(src,f,n);
	}

	static void itoa(void* dst,int n)
	{
		::sprintf((char*)dst,"%d",n);
	}

	static void utoa(void* dst,uint n)
	{
		::sprintf((char*)dst,"%u",n);
	}

	static void dtoa(void* dst,double n)
	{
		::sprintf((char*)dst,"%f",n);
	}

	static void int64toa(void* dst,int64 n)
	{
#ifdef _MSC_VER
		::sprintf((char*)dst,"%I64d",n);
#else
		::sprintf((char*)dst,"%lld",n);
#endif
	}

	static int atoi(char* s)
	{
		int result;
		::sscanf(s,"%d",&result);
		return result;
	}

	static uint atou(char* s)
	{
		uint result;
		::sscanf(s,"%u",&result);
		return result;
	}

	static double atod(char* s)
	{
		double result;
		::sscanf(s,"%lf",&result);
		return result;
	}

	static void atoint64(char* s,int64* result)
	{
#ifdef _MSC_VER
		::sscanf(s,"%I64d",result);
#else
		::scanf(s,"%lld",result);
#endif
	}

	static void exit(int a)
	{
		::exit(a);
	}

	static uchar* malloc(int size)
	{
		return (uchar*)::malloc(size);
	}

	static void free(void* p)
	{
		::free(p);
	}

	static void memcpy(void* dst,const void* src,int size)
	{
		::memcpy(dst,src,size);
	}

	static int strlen(const char* s)
	{
		return (int)::strlen(s);
	}

	static int strlenw(const wchar* p)
	{
#ifdef _MSC_VER
		return ::lstrlenW(p);
#else
		int ret=0;
		while(*p!=0)
		{
			ret++;
			p++;
		}
		return ret;
#endif
	}

	static int tick()
	{
#ifdef _MSC_VER
		return ::GetTickCount();
#else
		return (int)((double)::clock()/CLOCKS_PER_SEC*1000);
#endif
	}

	static void sleep(int milliseconds)
	{
		if(milliseconds<50)
		{
			milliseconds=50;
		}
#ifdef _MSC_VER
		::Sleep(milliseconds);
#else
		if(milliseconds>2100000000)
		{
			::sleep(milliseconds/1000);
		}
		else
		{
			::usleep(milliseconds*1000);
		}
#endif
	}

	static int getch()
	{
#ifdef _MSC_VER
		return ::getch();
#else
		struct termios oldt,newt;
		int ch;
		::tcgetattr(STDIN_FILENO,&oldt);
		newt=oldt;
		newt.c_lflag&=~(ICANON|ECHO);
		::tcsetattr(STDIN_FILENO,TCSANOW,&newt);
		ch=::getchar();
		::tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
		return ch;
#endif
	}

	static void srand()
	{
		::srand((uint)::time(null));
	}

	static int rand()
	{
		return ::rand();
	}

	static int cmd(const char* p)
	{
		return ::system(p);
	}

	static int get_exe_name(wchar* buf,int size)
	{
#ifdef _MSC_VER
		return GetModuleFileNameW(null,buf,size);
#else
#ifdef EMSCRIPTEN
		if(size>=10)
		{
			buf[0]='/';
			buf[1]='r';
			buf[2]=0;
			return true;
		}
		return false;
#else
		if(!get_cur_dir(buf,size))
		{
			return false;
		}
		int len=strlenw(buf);
		buf[len]='/';
		buf[len+1]='r';
		buf[len+2]=0;
		return true;
#endif
#endif
	}

	static int get_cur_dir(wchar* buf,int size)
	{
#ifdef _MSC_VER
		return GetCurrentDirectoryW(size,buf);
#else
#ifdef EMSCRIPTEN
		if(size>=2)
		{
			buf[0]=0;
			buf[1]=0;
			return true;
		}
		return false;
#else
		FILE* fp=popen("pwd","r");
		if(fp==null)
		{
			return false;
		}
		char tmp[4096];
		if(fgets(tmp,r_size(tmp),fp)==null)
		{
			return false;
		}
		pclose(fp);
		int i;
		for(i=0;i<strlen(tmp);i++)
		{
			buf[i]=tmp[i];
		}
		buf[i]=0;
		return true;
#endif
#endif
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
		{
			return 0;
		}
		if(!::GetFileTime(hFile,null,null,(FILETIME*)&ft))
		{
			return 0;
		}
		::CloseHandle(hFile);
		return ft;
#else
		return 0;
		struct stat buf;
		int ret=stat(path,&buf);
		if(0!=ret)
		{
			return false;
		}
		*ft=(int64)buf.st_mtime;
		return true;
#endif
	}*/

	/*static int get_exe_size(uchar* data)
	{
#ifdef _MSC_VER
		PIMAGE_DOS_HEADER dos_head=(PIMAGE_DOS_HEADER)data;
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
		return size;
#endif
	}*/

	static int ftell(void* fp)
	{
		return ::ftell((FILE*)fp);
	}

	static int64 ftell64(void* fp)
	{
#ifdef _MSC_VER
		return ::_ftelli64((FILE*)fp);
#else
		return ::ftello((FILE*)fp);
#endif
	}

	static int fseek(void* fp,int off,int start)
	{
		return ::fseek((FILE*)fp,off,start);
	}

	static int fseek64(void* fp,int64 off,int start)
	{
#ifdef _MSC_VER
		return ::_fseeki64((FILE*)fp,off,start);
#else
		return ::fseeko((FILE*)fp,off,start);
#endif
	}

	static void* _wfopen(wchar* name,wchar* mode)
	{
#ifdef _MSC_VER
		return (void*)::_wfopen(name,mode);
#else
		return null;
#endif
	}

	static void* fopen(char* name,char* mode)
	{
		return ::fopen(name,mode);
	}

	static int fclose(void* fp)
	{
		return ::fclose((FILE*)fp);
	}

	static int fread(void* buf,int size,int count,void* fp)
	{
		return ::fread(buf,size,count,(FILE*)fp);
	}

	static int fwrite(const void* buf,int size,int count,void* fp)
	{
		return ::fwrite(buf,size,count,(FILE*)fp);
	}

	static int _wremove(wchar* name)
	{
#ifdef _MSC_VER
		return ::_wremove(name);
#else
		return 0;
#endif
	}
	
	static int remove(char* name)
	{
		return ::remove(name);
	}

	/*static int _wrename(wchar* oldname,wchar* newname)
	{
#ifdef _MSC_VER
		return ::_wrename(oldname,newname);
#else
		return 0;
#endif
	}*/

	static int create_thr(thread_start start,void* param=null)
	{
		int tid;
#ifdef _MSC_VER
		tid=(int)::_beginthreadex(null,0,start,param,0,null);
		if(0==tid||1==tid)
		{
			return 0;
		}
		else
		{
			return tid;
		}
#else
		if(0!=::pthread_create((pthread_t*)&tid,null,start,param))
		{
			return 0;
		}
		else
		{
			return tid;
		}
#endif
	}

	static void wait_thr(int tid)
	{
#ifdef _MSC_VER
		::WaitForSingleObject((HANDLE)tid,INFINITE);
#else
		::pthread_join((pthread_t)tid,null);
#endif
	}

	static int MultiByteToWideChar(int a,int b,const char* src,int c,wchar* dst,int count)
	{
#ifdef _MSC_VER
		//即使是英语操作系统应该也安装936代码页
		return ::MultiByteToWideChar(a,b,src,c,dst,count);
#else
		int len=strlen(src);
		if(dst==null)
		{
			return len+1;
		}
		for(int i=0;i<len;i++)
		{
			dst[i]=src[i];
		}
		dst[len]=0;
		return len+1;
#endif
	}

	static int gbk_to_utf16(const char* src,wchar* dst,int count)
	{
		//即使是英语操作系统应该也安装936代码页
		return MultiByteToWideChar(936,0,src,-1,dst,count);
	}

	static int utf16_to_gbk(const wchar* src,char* dst,int buf_size)
	{
#ifdef _MSC_VER
		return ::WideCharToMultiByte(936,0,src,-1,dst,buf_size,null,null);
#else
		int len=strlenw(src);
		if(dst==null)
		{
			return len+1;
		}
		for(int i=0;i<len;i++)
		{
			dst[i]=(char)src[i];
		}
		dst[len]=0;
		return len+1;
#endif
	}

	static uchar* alloc_v(int len)
	{
#ifdef _MSC_VER
		return (uchar*)VirtualAlloc(null,len,
			MEM_COMMIT,PAGE_EXECUTE_READWRITE);
#else
		return null;
#endif
	}

	static void free_v(uchar* addr)
	{
#ifdef _MSC_VER
		VirtualFree(addr,0,MEM_RELEASE);
#endif
	}

	static xf* get_xf()
	{
		static xf g_xf;
		return &g_xf;
	}

	static char* GetCommandLineA()
	{
#ifdef _MSC_VER
		return ::GetCommandLineA();
#else
#ifdef EMSCRIPTEN
		return "rpp -interpret src/example/test/10_1.rpp";
#else
		static char temp[4096];
		temp[0]=0;
		for(int i=0;i<get_xf()->argc;i++)
		{
			strcat(temp,((char**)get_xf()->argv)[i]);
			strcat(temp," ");
		}
		return temp;
#endif
#endif
	}

	static void* FindFirstFileW(wchar* name,R_WIN32_FIND_DATAW* wfd)
	{
#ifdef _MSC_VER
		return ::FindFirstFileW(name,wfd);
#else
		return null;
#endif
	}

	static rbool FindNextFileW(void* handle,R_WIN32_FIND_DATAW* wfd)
	{
#ifdef _MSC_VER
		return ::FindNextFileW(handle,wfd);
#else
		return false;
#endif
	}

	static void FindClose(void* handle)
	{
#ifdef _MSC_VER
		::FindClose(handle);
#endif
	}

	static void InitializeCriticalSection(void* p)
	{
#ifdef _MSC_VER
		::InitializeCriticalSection((CRITICAL_SECTION*)p);
#endif
	}

	static void DeleteCriticalSection(void* p)
	{
#ifdef _MSC_VER
		::DeleteCriticalSection((CRITICAL_SECTION*)p);
#endif
	}

	static void EnterCriticalSection(void* p)
	{
#ifdef _MSC_VER
		::EnterCriticalSection((CRITICAL_SECTION*)p);
#endif
	}

	static void LeaveCriticalSection(void* p)
	{
#ifdef _MSC_VER
		::LeaveCriticalSection((CRITICAL_SECTION*)p);
#endif
	}

	/*static void* find_dll_full(const char* name)
	{
#ifdef _MSC_VER
		PBYTE pb=null;
		MEMORY_BASIC_INFORMATION mbi;
		while(VirtualQuery(pb,&mbi,r_size(mbi))==r_size(mbi))
		{
			if(mbi.State==MEM_FREE)
			{
				mbi.AllocationBase=mbi.BaseAddress;
			}
			if(mbi.AllocationBase==mbi.BaseAddress&&
				mbi.AllocationBase!=null) 
			{
				void* ret=GetProcAddress(
					(HINSTANCE)mbi.AllocationBase,name);
				if(ret!=null)
				{
					return ret;
				}
			} 
			pb+=mbi.RegionSize;
		}
		return null;
#endif
	}

	//非多线程安全
	static void set_dll_list()
	{
#ifdef _MSC_VER
		tsh& sh=*get_psh();
		PBYTE pb=null;
		MEMORY_BASIC_INFORMATION mbi;
		while(VirtualQuery(pb,&mbi,r_size(mbi))==r_size(mbi))
		{
			if(mbi.State==MEM_FREE)
			{
				mbi.AllocationBase=mbi.BaseAddress;
			}
			if(mbi.AllocationBase==mbi.BaseAddress&&
				mbi.AllocationBase!=null)
			{
				//todo 去掉GetProcAddress
				void* ret=GetProcAddress(
					(HINSTANCE)mbi.AllocationBase,"glutSpecialFunc");
				if(ret!=null)
				{
					set_dll_list_one(
						sh,(HINSTANCE)mbi.AllocationBase);
				}
				ret=GetProcAddress(
					(HINSTANCE)mbi.AllocationBase,"glBegin");
				if(ret!=null)
				{
					set_dll_list_one(
						sh,(HINSTANCE)mbi.AllocationBase);
				}
			}
			pb+=mbi.RegionSize;
		}
#endif
	}*/

/*#ifdef _MSC_VER
	static void set_dll_list_one(tsh& sh,HINSTANCE hModule)
	{
		char *pRet=null;
		PIMAGE_DOS_HEADER pImageDosHeader=NULL;
		PIMAGE_NT_HEADERS pImageNtHeader=NULL;
		PIMAGE_EXPORT_DIRECTORY pImageExportDirectory=null;

		pImageDosHeader=(PIMAGE_DOS_HEADER)hModule;
		pImageNtHeader=(PIMAGE_NT_HEADERS)((
			DWORD)hModule+pImageDosHeader->e_lfanew);
		pImageExportDirectory=(PIMAGE_EXPORT_DIRECTORY)(
			(DWORD)hModule+pImageNtHeader->OptionalHeader.DataDirectory[
				IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

		DWORD dwExportRVA=pImageNtHeader->OptionalHeader.DataDirectory[
			IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
		DWORD dwExportSize=pImageNtHeader->OptionalHeader.DataDirectory[
			IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

		DWORD* pAddressOfFunction=(DWORD*)(
			pImageExportDirectory->AddressOfFunctions+(DWORD)hModule);
		DWORD* pAddressOfNames=(DWORD*)(
			pImageExportDirectory->AddressOfNames+(DWORD)hModule);
		DWORD dwNumberOfNames=(DWORD)(
			pImageExportDirectory->NumberOfNames);
		DWORD dwBase=(DWORD)(pImageExportDirectory->Base);

		WORD* pAddressOfNameOrdinals=(WORD*)(
			pImageExportDirectory->AddressOfNameOrdinals+(DWORD)hModule);

		for(int i=0;i<(int)dwNumberOfNames;i++)
		{
			char *strFunction=(char*)(pAddressOfNames[i]+(DWORD)hModule);
			if(*strFunction==0)
			{
				continue;
			}
			pRet=(char*)(pAddressOfFunction[
				pAddressOfNameOrdinals[i]]+(DWORD)hModule);
			if((DWORD)pRet<dwExportRVA+(DWORD)hModule||
				(DWORD)pRet>dwExportRVA+(DWORD)hModule+dwExportSize)
			{
				sh.m_dll_func[strFunction]=(void*)pRet;
			}
		}
	}
#endif*/
};
