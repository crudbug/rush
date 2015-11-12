#include "back/zmain.h"

#ifdef EMSCRIPTEN
extern "C" int js_main(char* s)
{
	return !zmain::compile((uchar*)s);
}
#else
#ifndef __GNUC__
int main()
{
	return !zmain::compile(null);
}
#else
int main(int argc,char* argv[])
{
	xf::get_xf()->argc=argc;
	xf::get_xf()->argv=argv;
	return !zmain::compile(null);
}
#endif
#endif
