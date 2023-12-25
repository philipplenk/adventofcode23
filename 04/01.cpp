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
	for(std::string line; std::getline(std::cin,line);)
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

		if(won>0)
			total+=1<<(won-1);
	}

	std::cout<<total;
	
	return 0;
}
