/*
红黑树
*/

import rset.h

void main()
{
	rset<int> s
	
	s.insert(3)
	s.insert(2)
	s.insert_c(2)
	s.insert(1)
	for p=s.begin;p!=s.end;p=s.next(p)
		p->printl
	s.erase 2
	for p=s.begin;p!=s.end;p=s.next(p)
		p->printl
		
	b=s
	printl
	for p=b.begin;p!=b.end;p=b.next(p)
		p->printl
}
