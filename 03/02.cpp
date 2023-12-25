#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <cctype>
#include <cstddef>

int main(int argc, char* argv[])
{
	std::vector<std::string> grid{std::istream_iterator<std::string>{std::cin},{}};
	const int height = grid.size();
	const int width = grid[0].size();

	const auto invalid_number_id = std::numeric_limits<std::size_t>::max(); 
	std::vector<int> numbers;
	std::vector<std::vector<std::size_t>> number_id_at(height,std::vector<std::size_t>(width,invalid_number_id));

	for(int y=0; y<height; ++y)
	{
		for(int x=0; x<width; ++x)
		{
			if(!std::isdigit(grid[y][x])) continue;
			   
			const auto next_number_id = numbers.size();

			int num = 0;
			for(;std::isdigit(grid[y][x]);++x)
			{
				num = num*10 + grid[y][x] - '0';
				number_id_at[y][x] = next_number_id;
				
			}
			--x;
			numbers.push_back(num);
		}
	}

	int result = 0;
	for(int y=0;y<height;++y)
	{
		for(int x=0;x<width;++x)
		{
			if(grid[y][x]!='*') continue;

			std::unordered_set<std::size_t> adjacent_numbers;
			const auto insert_if_valid = [&](int x, int y)
			{
				if(x<0 || y<0 || x>=width || y>=height) return;
				if(number_id_at[y][x]==invalid_number_id) return;
				adjacent_numbers.insert(number_id_at[y][x]);
			};

			insert_if_valid(x+1, y); 
			insert_if_valid(x, y+1); 
			insert_if_valid(x-1, y); 
			insert_if_valid(x, y-1); 
			insert_if_valid(x-1, y-1); 
			insert_if_valid(x+1, y+1); 
			insert_if_valid(x+1, y-1); 
			insert_if_valid(x-1, y+1);

			if(adjacent_numbers.size()==2)
			{
				const auto a = numbers[*adjacent_numbers.begin()];
				const auto b = numbers[*std::next(adjacent_numbers.begin())];
				result+=a*b;
			}
		}
	}


	std::cout<<result;
	
	return 0;
}
