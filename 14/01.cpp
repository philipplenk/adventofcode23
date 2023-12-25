#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
	std::vector<std::string> grid(std::istream_iterator<std::string>{std::cin},{});
	const int height = grid.size();
	const int width = grid[0].size();

	for(bool changed = true; changed;)
	{
		changed = false;
		auto changed_grid = grid;
		for(int y=1; y<height; ++y)
		{
			for(int x=0; x<width; ++x)
			{
				if(grid[y][x]=='O' && changed_grid[y-1][x]=='.')
				{
					changed_grid[y-1][x] = 'O';
					changed_grid[y][x] = '.';
					changed = true;
				}
			}
		}
		grid = changed_grid;
	}

	int sum = 0;	
	for(int y=0;y<height;++y)
		sum+=std::count(grid[y].begin(),grid[y].end(),'O')*(height-y);
	
	std::cout<<sum;
	
	return 0;
}
