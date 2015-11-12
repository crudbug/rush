#pragma once

#include "cpp/xbase.hpp"

struct xsock
{
	static void init_sock()
	{
#ifdef _MSC_VER
		WORD wVersionRequested;
		WSADATA wsaData;

		wVersionRequested=MAKEWORD(1,1);

		if(0!=::WSAStartup(wVersionRequested,&wsaData))
		{
			return;
		}

		if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1)
		{
			return;
		}
#endif
	}

	static void clean_sock()
	{
#ifdef _MSC_VER
		::WSACleanup();
#endif
	}

	static int socket(int af,int type,int protocol)
	{
#ifdef _MSC_VER
		return ::socket(af,type,protocol);
#else
		return 0;
#endif
	}

	static int connect(int s,void* name,int namelen)
	{
#ifdef _MSC_VER
		return ::connect(s,(const struct sockaddr*)name,namelen);
#else
		return 0;
#endif
	}

	static int close(int s)
	{
#ifdef _MSC_VER
		return ::closesocket(s);
#else
		return 0;//return ::close(s);
#endif
	}

	static int send(int s,void* data,int size,int flags)
	{
#ifdef _MSC_VER
		return ::send(s,(const char*)data,size,flags);
#else
		return 0;
#endif
	}

	static int recv(int s,void* data,int size,int flags)
	{
#ifdef _MSC_VER
		return ::recv(s,(char*)data,size,flags);
#else
		return 0;
#endif
	}

	static int bind(int s,void* addr,int namelen)
	{
#ifdef _MSC_VER
		return ::bind(s,(const struct sockaddr*)addr,namelen);
#else
		return 0;
#endif
	}

	static int listen(int s,int backlog)
	{
#ifdef _MSC_VER
		return ::listen(s,backlog);
#else
		return 0;
#endif
	}

	static int accept(int s,void* addr,int* addrlen)
	{
#ifdef _MSC_VER
		return ::accept((SOCKET)s,(struct sockaddr*)addr,addrlen);
#else
		return 0;//return ::accept(s,(struct sockaddr*)addr,addrlen);
#endif
	}
};

struct xsock_t
{
	xsock_t()
	{
		xsock::init_sock();
	}

	~xsock_t()
	{
		xsock::clean_sock();
	}
};
