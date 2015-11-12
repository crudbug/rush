/*
常量作为对象使用，只是编译器提供的一个语法糖

1.print
会替换为
int(1).print

"123"+"abc"
会替换为
rstr("123")+rstr("abc")

和C++一样，
类名后面接小括号表示生成临时对象
rstr("123")
会生成一个临时的rstr对象
int(1)
会生成一个临时的int对象

*/

void main()
{
	1.print
	"abc".printl
	("123"+"abc").printl
	printl "123"+"abc"
}