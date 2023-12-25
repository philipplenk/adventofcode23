#include <algorithm>
#include <bit>
#include <iostream>
#include <string>
#include <vector>

#include <cstddef>
#include <cstdint>

int main(int argc, char* argv[])
{
	int result = 0;
	
	while(std::cin)
	{
		std::vector<std::uint32_t> grid, rotated_grid;
		std::size_t width = 0;
		for(std::string line;std::getline(std::cin,line) && !line.empty();)
		{
			std::uint32_t bits{};
			for(std::size_t i=0;i<line.size();++i)
				bits |= (line[i]=='#'?1u:0)<<i;
			grid.push_back(bits);
			
			width = line.size();
		}

		const auto height = grid.size();

		rotated_grid.resize(width,0);
		for(std::size_t y=0;y<height;++y)
		{
			for(std::size_t x=0;x<width;++x)
			{
				const auto bit = (grid[y]>>x)&1;
				const auto new_bit_position = height-1-y;
				rotated_grid[x] |=  bit<<new_bit_position;
			}
		}

		const auto is_reflected_at = [&](const auto& g, std::size_t row, int desired_errors)
		{
			int errors = 0;

			const auto dist_to_check = std::min(row,g.size()-2-row);
			for(std::size_t i=0;i<=dist_to_check;++i)
				errors+=std::popcount(g[row-i]^g[row+i+1]);

			return errors==desired_errors;
		};

		for(std::size_t row=0;row<height-1;++row)
		{
			if(is_reflected_at(grid,row,1))
				result+=(row+1)*100;
		}
	
		for(std::size_t col=0;col<width-1;++col)
		{
			if(is_reflected_at(rotated_grid,col,1))
				result+=col+1;
		}
	}
	
	std::cout<<result;

	return 0;
}
