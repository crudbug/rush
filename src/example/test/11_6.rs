/*
对比宏和模板函数
注意宏是无格式的，因此花括号和分号不可省略
*/

void main()
{
	printl fb<int>(1,2)
	printl fb<double>(0.5,0.6)

	printl fa_int(1,2)
	printl fa_double(0.5,0.6)	
}

T fb<T>(T a,T b)
{
	return a+b;
}

fa int
fa double

define$ fa _word
{
	$0 fa_##$0($0 a,$0 b)
	{
		return a+b;
	}	
}