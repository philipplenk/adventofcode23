#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <cstddef>

int main(int argc, char* argv[])
{
	int result = 0;
	
	while(std::cin)
	{
		std::vector<std::string> grid, rotated_grid;
		for(std::string line;std::getline(std::cin,line) && !line.empty();)
			grid.push_back(line);

		const auto height = grid.size();
		const auto width = grid[0].size();

		rotated_grid.resize(width,std::string(height,'x'));
		for(std::size_t y=0;y<height;++y)
		{
			for(std::size_t x=0;x<width;++x)
				rotated_grid[x][height-1-y] = grid[y][x];
		}

		const auto is_reflected_at = [&](const auto& g, std::size_t row)
		{
			const auto dist_to_check = std::min(row,g.size()-2-row);
			for(std::size_t i=0;i<=dist_to_check;++i)
			{
				if(g[row-i]!=g[row+i+1])
					return false;
			}

			return true;
		};
		
		for(std::size_t row=0;row<height-1;++row)
		{
			if(is_reflected_at(grid,row))
				result+=(row+1)*100;
		}
	
		for(std::size_t col=0;col<width-1;++col)
		{
			if(is_reflected_at(rotated_grid,col))
				result+=col+1;
		}
	}
	
	std::cout<<result;

	return 0;
}
