/*
使用多参数组定义三维数组
*/

import xlib/rs/rbufm.rs

void main()
{
	rbufm<int> arr(5,3,4)

	for i=0;i<5;i++
		for j=0;j<3;j++
			for k=0;k<4;k++
				arr[i,j,k]=j
				arr[i,j,k].print
}