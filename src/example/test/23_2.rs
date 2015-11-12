/*
rush支持自动类型转换，
所以请小心定义拷贝构造函数
*/

void main()
{
	func(2)//整型2自动转换为无符号整型
}

void func(uint a)
{
	print a
}