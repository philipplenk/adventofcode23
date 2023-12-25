#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

struct point2d
{
	int x, y;
};

struct vec2d
{
	int x, y;
};

point2d& operator+=(point2d& p, const vec2d& v)
{
	p.x+=v.x;
	p.y+=v.y;
	return p;
}

point2d operator+(point2d p, const vec2d& v)
{
	return p+=v;
}

int main(int argc, char* argv[])
{
	std::vector<std::string> grid(std::istream_iterator<std::string>{std::cin},{});
	const int height = grid.size();
	const int width = grid[0].size();

	enum class direction
	{
		north,
		west,
		south,
		east
	};

	const auto tilt = [&](direction tilt_direction)
	{
		const auto valid = [&](const point2d p)
		{
			return p.x>=0 && p.y>=0 && p.x<width && p.y<height;
		};

		const std::array<vec2d,4> iteration_directions
		{{
			{0,1},
			{1,0},
			{0,-1},
			{-1,0}
		}};

		const auto inner_step = iteration_directions[static_cast<int>(tilt_direction)];
		const auto outer_step = inner_step.x==0?vec2d{1,0}:vec2d{0,1};
		const auto iteration_start_x = inner_step.x<0?width-1:0;
		const auto iteration_start_y = inner_step.y<0?height-1:0;
		const auto iteration_start = point2d{iteration_start_x,iteration_start_y};

		for(point2d start=iteration_start; valid(start); start+=outer_step)
		{
			for(point2d p = start, move_to = start; valid(p); p+=inner_step)
			{
				if(grid[p.y][p.x]=='O')
				{
					grid[p.y][p.x] = '.';
					grid[move_to.y][move_to.x] = 'O';
					move_to+=inner_step;
				}
				else if(grid[p.y][p.x]=='#')
					move_to = p + inner_step;
			}
		}
	};

	std::map<std::vector<std::string>,int> seen;
	std::vector<std::vector<std::string>> grids;

	constexpr auto number_of_tilt_cycles = 1000000000;
	for(int cycle = 0; cycle<number_of_tilt_cycles; ++cycle)
	{
		if(seen.contains(grid))
		{
			const auto cycle_start = seen[grid];
			const auto cycle_length = cycle-cycle_start;
			const auto remaining = (number_of_tilt_cycles-cycle)%cycle_length;
			
			grid = grids[cycle_start+remaining];
			break;
		}
		grids.push_back(grid);
		seen[grid] = cycle;

		tilt(direction::north);
		tilt(direction::west);
		tilt(direction::south);
		tilt(direction::east);
	}

	int sum = 0;
	for(int y=0;y<height;++y)
		sum+=std::count(grid[y].begin(),grid[y].end(),'O')*(height-y);
	
	std::cout<<sum;

	return 0;
}
