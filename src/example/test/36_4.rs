/*
split、快排、二分
*/

import "ralgo.h"

void main()
{
	rbuf<rstr> v=r_split<rstr>('321;1343;fj242i;12',';')
	for i=0;i<v.count;i++
		v[i].printl
	printl
	
	r_qsort<rstr>(v)
	for i=0;i<v.count;i++
		v[i].printl
	printl
	
	v.get(r_find_b<rstr>(v,'12')).printl
	
	'1234'.find('34').printl
}
