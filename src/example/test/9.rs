/*
pseudo-code like 'Introduction to Algorithms'
*/

define ← =

void insertion_sort(rstr& a):
	for j ← 1; j<a.count; j ← j+1
		key ← a[j]
		i ← j-1
		while i>=0 && a[i]>key
			a[i+1] ← a[i]
			i ← i-1
		a[i+1] ← key

void main():
	rstr a="cab132"
	printl a
	insertion_sort a
	printl a
	