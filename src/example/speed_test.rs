/*
sum前加上int8可以求出正确的和
如改成sum+=i+j可测试出C++的真实速度
*/

void main()
{
	start=xf.tick
	
	sum=0
	for i=0;i<=10000;i++
		for j=0;j<=10000;j++
			sum+=i
	printl sum
	
	printl xf.tick-start
}
