#include <iostream>
#include <string>

#include <cctype>

int main(int argc, char* argv[])
{
	int sum = 0;
	for(std::string line;std::getline(std::cin,line);)
	{
		char first = 0, last=0;
		for(auto c: line)
		{
			if(std::isdigit(c))
			{
				if(!first)
					first = c;
				last = c;
			}
		}
		sum+=(first-'0')*10+last-'0';
	}
	std::cout<<sum;

	return 0;
}
