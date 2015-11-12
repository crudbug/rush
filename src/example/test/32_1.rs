
void main()
{
	test('1')
	test('2')
	test('3')
}

void test(rstr s)
{
	if s=='1':
		printl 'pass 1'
	elif s=='2':
		printl 'pass 2'
	else:
		printl 'other'

	if(s=='1')
	{
		printl 'pass 1'
	}
	else
	{
		printl 'other'
	}

	switch s
		case '1'
			printl 'pass 1'
		case '2'
			printl 'pass 2'
		default
			printl 'default'

	switch s:
		case '1'
		{
			printl 'pass 1'
		}
		case '2'
		{
			printl 'pass 2'
		}
		default
		{
			printl 'default'
		}

	switch(s)
	{
		case '1':
		{
			printl 'pass 1'
		}
		case '2':
		{
			printl 'pass 2'
		}
		default:
		{
			printl 'default'
		}
	}
	
	switch(s)
	{
	case '1':
	{
		printl 'pass 1'
	}
	case '2':
	{
		printl 'pass 2'
	}
	default:
	{
		printl 'default'
	}
	}
	
	switch(s)
	{
	case '1':
		printl 'pass 1'
	case '2':
		printl 'pass 2'
	default:
		printl 'default'
	}
}
