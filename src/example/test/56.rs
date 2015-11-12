/*
组合遍历
*/

void main()
{
	n=7
	sum=0
	for i=0;i<n;i++
		for j=i+1;j<n;j++
			for k=j+1;k<n;k++
				sum++
	printl(sum)

	printl(A(3,6))
	printl(A(6,6))
	printl(C(3,7))
}

int A(int a,int b)
{
	temp=1
	for i=0;i<a;i++
		temp*=b-i
	return temp
}

int C(int a,int b)
{
	return A(a,b)/A(a,a)
} 