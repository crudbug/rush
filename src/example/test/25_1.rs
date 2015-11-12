/*
二维数组
*/

import xlib/rs/rbufm.rs

void main()
{
	rbufm<int> arr(5,5)

	for i=0;i<5;i++
		for j=0;j<5;j++
			arr[i,j]=i
			arr[i,j].print
}