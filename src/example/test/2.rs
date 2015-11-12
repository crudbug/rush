﻿/*
与上一个程序对应的C风格，
由此可以看到rush的一些特点：
*更彻底地类型推断，C++11需要使用关键字auto，rush可以省略auto
*函数定义和函数调用均可省略后面的小括号
*语句后面的分号也可以省略
*对于返回值为void的函数，可以省略void（标准C默认返回int）
*/

void main()
{
	int a=1;
	int b=2;
	print(a+b);
}