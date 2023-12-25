#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <vector>

#include <cstddef>

int main(int argc, char* argv[])
{
	struct hashmap_entry
	{
		std::string label;
		int focal_length;
	};
	std::array<std::vector<hashmap_entry>,256> boxes{};
	
	int current_hash = 0;
	std::string current_label;
	for(char c; std::cin>>c;)
	{
		switch(c)
		{
			case ',':
			{
				current_hash = 0;
				current_label = "";
				
				break;
			}
			case '=':
			{
				std::cin>>c;
				auto pos = std::ranges::find_if(boxes[current_hash],[&](const auto& entry)
				{
					return entry.label==current_label;
				});

				if(pos!=boxes[current_hash].end())
					pos->focal_length=c-'0';
				else
					boxes[current_hash].push_back({current_label,c-'0'});
				
				break;
			}
			case '-':
			{
				std::erase_if(boxes[current_hash],[&](const auto& entry)
				{
					return entry.label==current_label;
				});
				
				break;
			}
			default:
			{
				current_label+=c;

				current_hash+=c;
				current_hash*=17;
				current_hash%=boxes.size();
			}
		}
	}

	int sum = 0;
	for(std::size_t i=0;i<boxes.size();++i)
	{
		for(std::size_t slot=0;slot<boxes[i].size();++slot)
		{
			sum+=(i+1)*(slot+1)*boxes[i][slot].focal_length;
		}
	}
	std::cout<<sum;

	return 0;
}
