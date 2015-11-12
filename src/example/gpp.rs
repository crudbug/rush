/*
编译生成exe并运行
*/

import rfile.h
import rdir.h

void main()
{
	v=rf.get_param
	if v.count<3
		printl('invaild param')
		return
	opt='-O2'
	if v.count>3
		opt=v[3]
	src=v[2]
	if src.get_left==r_char('\"')
		src=src.sub(1,src.count-1)
	if rf.cmd('rush -gpp '+src)
		printl('compile error')
		return
	src_name=rdir.get_real_name(src)
	//printl src_name
	//return
	if rf.cmd('..\\ext\\mingw\\bin\\build '+src_name+'.cpp '+
		src_name+'.exe '+opt)
		clear(src_name)
		printl('cpp error')
		return
	//printl('..\\ext\\mingw\\bin\\build '+src_name+'.cpp '+
	//	src_name+'.exe')
	
	ifn v.count>=5&&v[4]=='-build'
		rf.cmd(rdir.dir_std_rev(src_name+'.exe'))
	
	//clear(src_name)
	
	//rfile.remove(rcode.gbk_to_utf8(src_name)+'.exe')
	return
	cmd='..\\ext\\nasm\\GoLink.exe /console /mix /entry _main \"'
	cmd+=src_name+'.obj\" msvcrt.dll ws2_32.dll kernel32.dll user32.dll'
	if rf.cmd(cmd)
		clear(src_name)
		printl('link error')
		return
	clear(src_name)
	if v[3]=='-build'
		;
	elif v[3]=='-del'
		rf.cmd(rdir.dir_std_rev(src_name+'.exe'))
		while rfile.remove(rcode.gbk_to_utf8(src_name)+'.exe')
			xf.sleep(100)//很可能是由于golink没有关闭EXE导致无法删除
	elif v[3]=='-check'
		if rfile.remove(rcode.gbk_to_utf8(src_name)+'.exe')
			printl 'delete error'
}

void clear(rstr name)
{
	name=rcode.gbk_to_utf8(name)
	rfile.remove(name+'.exe')
	rfile.remove(name+'.cpp')
}