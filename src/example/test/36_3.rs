/*
哈希表
*/

import rhash.h

void main()
{
	rhash<int> a
	a['abc']=3
	a['121213']=4
	a['1']=1
	for i=1;i<=1000;i++
		a[i.torstr]=i
	a['121213'].printl
}
