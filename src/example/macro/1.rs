/*
下面是编译期计算示例，sum是一个宏，计算从0加到N
此程序仅支持jit模式运行
*/

import back/zmain.h

void main()
{
	printl sum(100)//输出5050
	printl sum(10)//输出55
}

macro void sum(rbuf<tword>& v,int index)
{
	result=0
	param=v.get(index+2).val.toint
	for i=0;i<=param;i++
		result+=i
	v[index].val=result
	for i=1;i<=3;i++
		v[index+i].val.clear
}
