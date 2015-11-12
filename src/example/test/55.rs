/*
栈排序与栈颠倒
*/

void main()
{
	rbuf<int> stack
	#stack.push(3,4,1,9,10,2)
	out(stack)
	func(stack)
	out(stack)
	reverse(stack)
	out(stack)
	sort(stack)
	out(stack)
}

void out(rbuf<int>& stack)
{
	for i=0;i<stack.count;i++
		printl(stack[i])
	printl
}

void sort(rbuf<int>& stack)
{
	if stack.empty
		return
	a=stack.pop
	if stack.empty
		stack.push(a)
		return
	sort(stack)
	b=stack.pop
	if a<b
		stack.push(a)
		sort(stack)
		stack.push(b)
	else
		stack.push(b)
		sort(stack)
		stack.push(a)
}

void func(rbuf<int>& stack)
{
	if stack.count<=1
		return
	a=stack.pop
	func(stack)
	b=stack.pop
	if a<b
		stack.push(a)
		func(stack)
		stack.push(b)
	else
		stack.push(b)
		stack.push(a)
}

void reverse(rbuf<int>& stack)
{
	if stack.count<=1
		return
	a=stack.pop
	reverse(stack)
	b=stack.pop
	reverse(stack)
	stack.push(a)
	reverse(stack)
	stack.push(b)
}