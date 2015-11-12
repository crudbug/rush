/*
rush ..\src\example\symbol.rs rp3Cchar3E2Erp3Cchar3E28rp3Cchar3E262Crp3Cchar3E29
*/

void main()
{
	s=rf.get_param.get(2)//xf.scanl
	result=''
	for i=0;i<s.count;i++
		if rstr.is_number(s[i])&&(rstr.is_number(s.get(i+1))||s.get(i+1)>=`A&&s.get(i+1)<=`F)
			temp=0
			if rstr.is_number(s.get(i+1))
				temp=s.get(i+1).toint-48
			else
				temp=s.get(i+1).toint-`A+10
			result+=((s[i].toint-48)*16+temp).tochar
			i++
		else
			result+=s[i]
	printl result
}
