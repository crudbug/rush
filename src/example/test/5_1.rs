/*
递归求和
举例说明控制结构（if/for/while）如何省略小括号

sum1和sum2两个函数功能完全相同
sum2省略了语句后面的分号和if后面的小括号

如果函数调用的右边没有小括号，
则本行后面所有单词均作为函数的参数
即 
printl sum1(9) 
与
printl(sum1(9))
是一样的

rush里没有函数声明，直接调用即可
*/

void main()
{
	printl(sum1(9))
	printl sum2(9)
	printl sum1 9
}

int sum1(int a)
{
	if(a<=1) 
		return 1;
	return sum1(a-1)+a;
}

int sum2(int a)
{
	if a<=1  
		return 1
	return sum2(a-1)+a
}