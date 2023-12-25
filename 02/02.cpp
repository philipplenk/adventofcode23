#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#include <cctype>

int main(int argc, char* argv[])
{
	int sum = 0;
	for(std::string line, skip; std::getline(std::cin,line);)
	{
		std::stringstream strm{line};
		int id;
		
		strm>>skip>>id>>skip;

		std::unordered_map<std::string,int> powers;
		for(std::string round;std::getline(strm,round,';');)
		{
			std::stringstream rstrm{round};
			int count;
			std::string color;
			while(rstrm>>count>>color)
			{
				if(!std::isalpha(color.back()))
					color.pop_back();

				powers[color] = std::max(powers[color],count);
			}
		}
		
		int total = 1;
		for(const auto& [_,power]: powers)
			total*=power;
		sum+=total;
	}
	
	std::cout<<sum;
	
	return 0;
}
