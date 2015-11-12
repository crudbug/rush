/*
使用嵌套模板定义三维数组
*/

void main()
{
	rbuf<rbuf<rbuf<int>>> arr

	arr.alloc(5)
	for i=0;i<arr.count;i++
		arr[i].alloc(3)
		for j=0;j<arr[i].count;j++
			arr[i][j].alloc(4)

	for i=0;i<arr.count;i++
		for j=0;j<arr[i].count;j++
			for k=0;k<arr[i][j].count;k++
				arr[i][j][k]=j
				arr[i][j][k].print
}