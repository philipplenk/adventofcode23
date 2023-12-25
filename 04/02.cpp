#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>

int main(int argc, char* argv[])
{
	int total = 0;
	std::vector<int> factors(1,1);

	std::string line;
	for(std::size_t idx = 0; std::getline(std::cin,line);++idx)
	{
		std::stringstream strm{line};
		std::string skip;
		strm>>skip>>skip;

		std::string winning_numbers_string, my_numbers_string;
		std::getline(strm,winning_numbers_string,'|');
		std::getline(strm,my_numbers_string);

		std::stringstream winning_numbers_strm{winning_numbers_string};
		std::stringstream my_numbers_strm{my_numbers_string};
		const std::unordered_set<int> winning_numbers{std::istream_iterator<int>{winning_numbers_strm},{}};

		const auto won = std::count_if(std::istream_iterator<int>{my_numbers_strm},{},[&](int number)
		{
			return winning_numbers.contains(number);
		});

		const auto factor = factors[idx];
		total+=factor;

		factors.resize(std::max({factors.size(),idx+2,idx+won+1}),1);
		for(std::size_t i=idx+1;i<=idx+won;++i)
			factors[i]+=factor;
	}

	std::cout<<total;
	
	return 0;
}
