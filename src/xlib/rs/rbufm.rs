
class rbufm<T>
{
	rbuf<T> m_buf
	int m_level
	int m_row
	int m_column

	~rbufm<T>()
	{
	}

	rbufm<T>()
	{
	}

	rbufm<T>(int row,int column)
	{
		m_buf.realloc_n(row*column)
		m_level=0
		m_row=row
		m_column=column
	}

	rbufm<T>(int level,int row,int column)
	{
		m_buf.realloc_n(level*row*column)
		m_level=level
		m_row=row
		m_column=column
	}

	rbufm<T>(rbufm<T>& a)
	{
		m_buf=a.m_buf
		m_level=a.m_level
		m_row=a.m_row
		m_column=a.m_column
	}

	=(rbufm<T>& a)
	{
		m_buf=a.m_buf
		m_level=a.m_level
		m_row=a.m_row
		m_column=a.m_column
	}

	T& [](int i,int j)
	{
		return m_buf[i*m_column+j]
	}

	T& [](int i,int j,int k)
	{
		return m_buf[i*m_row*m_column+j*m_column+k];
	}

	friend bool ==(rbufm<T>& a,rbufm<T>& b)
	{
		if(a.m_buf!=b.m_buf)
			return false
		return (a.m_level==b.m_level&&
			a.m_row==b.m_row&&
			a.m_column==b.m_column)
	}

	friend bool !=(rbufm<T>& a,rbufm<T>& b)
	{
		return !(a==b)
	}
};
