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

		bool valid = true;
		for(std::string round;std::getline(strm,round,';');)
		{
			std::stringstream rstrm{round};
			int count;
			std::string color;
			while(rstrm>>count>>color)
			{
				if(!std::isalpha(color.back()))
					color.pop_back();

				if(color=="red" && count>12) valid = false;
				if(color=="green" && count>13) valid = false;
				if(color=="blue" && count>14) valid = false;
			}
		}

		if(valid)
			sum+=id;
	}
	
	std::cout<<sum;
	
	return 0;
}
