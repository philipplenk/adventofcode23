#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <cstddef>

struct point2d
{
	long long x, y;
};

int main(int argc, char* argv[])
{
	std::vector<std::string> grid{std::istream_iterator<std::string>{std::cin},{}};
	const int height = grid.size();
	const int width = grid[0].size();

	std::vector<bool> row_contains_galaxy(height,false), col_contains_galaxy(width,false);
	for(int y=0; y<height; ++y)
	{
		for(int x=0; x<width; ++x)
		{
			const auto contains_galaxy = grid[y][x]=='#';
			row_contains_galaxy[y] = row_contains_galaxy[y] | contains_galaxy;
			col_contains_galaxy[x] = col_contains_galaxy[x] | contains_galaxy;
		}
	}

	const long long expansion = 2;
	std::vector<point2d> galaxies;

	for(long long y=0, real_y=0; y<height; ++y)
	{
		for(long long x=0, real_x = 0;x<width;++x)
		{
			if(grid[y][x]=='#')
				galaxies.push_back({real_x,real_y});
			
			real_x+=col_contains_galaxy[x]?1:expansion;
		}
		real_y+=row_contains_galaxy[y]?1:expansion;
	}

	long long ago = 0;
	for(std::size_t i=0; i<galaxies.size(); ++i)
	{
		for(std::size_t j=i+1; j<galaxies.size(); ++j)
		{
			const auto a = galaxies[i];
			const auto galaxy = galaxies[j];
			const auto far_far = std::abs(a.x-galaxy.x);
			const auto away = std::abs(a.y-galaxy.y);
			ago+=far_far+away;
		}
	}
	std::cout<<ago;
	
	return 0;
}
