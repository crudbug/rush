/*
无缝内联汇编
*/

void main()
{
	//printl esp 
#ifndef _RGPP
	1+2
	mov eax,ebx
	printl eax
	
	if(eax)
		printl "eax is not zero"
	else
		printl "eax is zero"
#else
	printl 3
	printl "eax is not zero"
#endif
}
