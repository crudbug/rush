#pragma once

template<typename TA,typename TB>
struct rpair
{
	TA first;
	TB second;

	friend rbool operator<(rpair<TA,TB>& a,rpair<TA,TB>& b)
	{
		return a.first<b.first;
	}
};
