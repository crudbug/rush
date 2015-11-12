/*
typeof后面可以跟表达式或者类型

typeof(1+2)
等价于
"int"

sizeof后面只能跟类型
*/

void main()
{
	printl typeof(1+2)
	
	A<int> a
	a.func
	
	printl sizeof(A<int>)
}

class A<T>
{
	void func()
	{
		printl typeof(T)
	}
}