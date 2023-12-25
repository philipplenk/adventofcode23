#include <array>
#include <iostream>
#include <optional>
#include <string_view>

#include <cctype>

int main(int argc, char* argv[])
{
	std::array<std::string_view,9> digits{"one","two","three","four","five","six","seven","eight","nine"};
	
	int sum = 0;
	for(std::string line; std::getline(std::cin,line);)
	{
		int first = 0, last=0;
		for(int idx = 0; auto c: line)
		{
			std::optional<int> value = std::isdigit(c)?
				std::make_optional(c-'0'):
				std::nullopt;

			if(!value)
			{
				for(int potential_value=1; const auto& digit: digits)
				{
					if(std::string_view{line}.substr(idx).starts_with(digit))
						value = potential_value;

					++potential_value;
				}
			}
			
			if(value)
			{
				if(!first)
					first = *value;

				last = *value;
			}
			++idx;
		}

		sum+=first*10+last;
	}
	std::cout<<sum;

	return 0;
}
