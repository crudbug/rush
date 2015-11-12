/*
统计当前目录下代码行数
如果使用utf16统计更准确
*/

import rdir.h
import rfile.h

int g_sum=0

void count_line(rstr name)
{
	s=rfile.read_all_n(name)
	sum=1
	for i=0;i<s.count;i++
		if s[i]==0xa
			sum++
	g_sum+=sum
	printl(name+': '+sum+' lines')
}

void main()
{
	path='../src'
#ifdef _RVM
	if rf.get_param.count==3
		path=rf.get_param.top
#endif
		
	v=rdir.get_file_bfs(path)
	for i=0;i<v.count;i++
		name=v[i]
		suffix=rdir.get_suffix(name)
		if suffix=='h'||suffix=='cpp'||suffix=='rs'
			count_line(name)
	printl('total: '+g_sum)
}