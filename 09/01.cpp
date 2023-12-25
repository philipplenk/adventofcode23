#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
	const auto next = [](auto begin, auto end, auto rec)
	{
		if(begin==end || std::all_of(begin,end,[](auto v){ return v==0; }))
			return 0;

		const auto value = *std::prev(end);
		std::adjacent_difference(begin,end,begin);
		return value+rec(std::next(begin),end,rec);
	};

	int sum = 0;
	for(std::string line; std::getline(std::cin,line);)
	{
		std::stringstream strm{line};
		std::vector<int> values(std::istream_iterator<int>{strm},{});
		sum+=next(values.begin(),values.end(),next);
	}
	std::cout<<sum;
	
	return 0;
}
