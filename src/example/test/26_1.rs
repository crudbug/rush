/*
重载
*/

void func(int& a)
{
	printl 'func int'
}

void func(char& a)
{
	printl 'func char'
}

void main()
{
	func int()
	func char()
}