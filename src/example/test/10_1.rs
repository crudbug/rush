/*
模板函数
*/

void main()
{
	printl func<+>(10)
	printl func<*>(10)
	
	printl func<+> 10
	printl func<*> 10
}

int func<T>(int a)
{
	if a<=1
		return 1
	return func<T>(a-1) T a
}