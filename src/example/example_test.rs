/*
分别以解释方式和编译方式运行example下所有例子，
并与标准答案进行对比，以此来检验编译器的正确性。
*/

import rdir.h
import rfile.h

int g_sta_ok
int g_total

void main()
{
	use_paral=rf.get_param.get_top=='-paral'
	g_sta_ok=1
	start=xf.tick
	g_total=0
	v=rdir.get_file_bfs('../src/example/test/')
	rbuf<rstr> vname
	for i=0;i<v.count;i++
		name=rdir.get_name(v[i])
		suffix=rdir.get_suffix(name)
		if suffix!='rs'
			continue
		a=name.sub(0,name.count-suffix.count-1)
		if is_ignore(a)
			continue
		vname.push(a)
	rbuf<int> vtime
	num=4
	
	time=xf.tick
	
	for i=0;i<vname.count;i++
		check_vm(vname[i])
	
	vtime.push(xf.tick-time)
	time=xf.tick
	
	for i=0;i<vname.count;i++
		check_jit(vname[i])
		
	vtime.push(xf.tick-time)
	time=xf.tick
	
	for i=0;i<vname.count;i++
		check_nasm(vname[i])
	
	vtime.push(xf.tick-time)
	time=xf.tick
	
	for i=0;i<vname.count;i++
		check_gpp(vname[i])
		
	vtime.push(xf.tick-time)

	printl
	for i=0;i<vtime.count;i++
		printl 'time'+i+': '+vtime[i]
	printl
	printl('total: '+g_total)
	printl('time: '+(xf.tick-start))
	printl('state: '+r_cond<rstr>(g_sta_ok,'ok','error'))
	xf.getch
}

bool is_ignore(rstr a)
{
	ifn a.sub(0,1).is_number
		return true
	if a=='36_8'
		return true
	return false
}

void check_vm(rstr a)
{
	rf.cmd('rush ../src/example/test/'+a+'.rs > ../src/example/answer/'+a+'_tmp.txt')
	check_vm('../src/example/answer/'+a+'_tmp.txt',
		'../src/example/answer/'+a+'.txt')
}

void check_jit(rstr a)
{
	rf.cmd('rush -jit ../src/example/test/'+a+'.rs > ../src/example/answer/'+a+'_tmp.txt')
	check_jit('../src/example/answer/'+a+'_tmp.txt','../src/example/answer/'+a+'.txt')
}

void check_nasm(rstr a)
{
	rf.cmd('rnasm \"../src/example/test/'+a+'.rs\" > \"../src/example/answer/'+a+'_tmp.txt\"')
	check_nasm('../src/example/answer/'+a+'_tmp.txt','../src/example/answer/'+a+'.txt')
}

void check_gpp(rstr a)
{
	if a=='17_1'
		return
	rf.cmd('gpp \"../src/example/test/'+a+'.rs\" > \"../src/example/answer/'+a+'_tmp.txt\"')
	check_gpp('../src/example/answer/'+a+'_tmp.txt','../src/example/answer/'+a+'.txt')
}

void check_vm(rstr a,rstr b)
{
	//不严谨，应该使用互斥体
	g_total++
	cont=rfile.read_all_n(a);
	if cont.count>0&&cont==rfile.read_all_n(b)
		print('ok ')
	else
		print('* * * error ')
		g_sta_ok=0
	printl('vm '+rdir.get_name(a)+' vs '+rdir.get_name(b))
	if rfile.remove(a)
		printl('* * * error ')
		g_sta_ok=0
}

void check_jit(rstr a,rstr b)
{
	g_total++
	cont=rfile.read_all_n(a);
	if cont.count>0&&cont==rfile.read_all_n(b)
		print('ok ')
	else
		print('* * * error ')
		g_sta_ok=0
	printl('jit '+rdir.get_name(a)+' vs '+rdir.get_name(b))
	if rfile.remove(a)
		printl('* * * error ')
		g_sta_ok=0
}

void check_nasm(rstr a,rstr b)
{
	g_total++
	cont=rfile.read_all_n(a)
	cont=cont.sub(cont.find('\x73\x0d\x0a')+3)//去掉golink打印的信息
	if cont.count>0&&cont==rfile.read_all_n(b)
		print('ok ')
	else
		print('* * * error ')
		g_sta_ok=0
	printl('nasm '+rdir.get_name(a)+' vs '+rdir.get_name(b))
	if rfile.remove(a)
		printl('* * * error ')
		g_sta_ok=0
}

void check_gpp(rstr a,rstr b)
{
	g_total++
	cont=rfile.read_all_n(a);
	if cont.count>0&&cont==rfile.read_all_n(b)
		print('ok ')
	else
		print('* * * error ')
		g_sta_ok=0
	printl('gpp '+rdir.get_name(a)+' vs '+rdir.get_name(b))
	if rfile.remove(a)
		printl('* * * error ')
		g_sta_ok=0
}
