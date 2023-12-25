#include <advent.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
	int current = 0, sum = 0;
	for(char c; std::cin>>c;)
	{
		if(c==',')
		{
			sum+=current;
			current = 0;
			continue;
		}
		current+=c;
		current*=17;
		current%=256;
	}
	sum+=current;
	std::cout<<sum;

	return 0;
}
