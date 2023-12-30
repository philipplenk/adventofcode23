/*
	Generic, input independent solution based on the amazing hashlife algorithm.
	
	Based on and inspired by this truly excellent solution: https://gist.github.com/codekitchen/196a5309f1467c7f7ddaed2978be04a3
	Which I found linked on reddit here: https://www.reddit.com/r/adventofcode/comments/18oy5bi/2023_day_21ruby_alternative_solution_that_works/

	This explanation was incredibly helpful and illuminating: https://www.dev-mind.blog/hashlife/
*/

#include "hashlife.hpp"
#include "point2d.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <numeric>
#include <span>
#include <vector>

class advent_garden_plot
{
	public:
	explicit advent_garden_plot(std::vector<std::string> grid):
		grid_{std::move(grid)}
	{}

	auto pattern_width() const { return std::ssize(grid_[0]); }
	auto pattern_height() const { return std::ssize(grid_); }

	bool is_alive(std::size_t value) const { return value==1; }

	point2d normalize(point2d p) const
	{
		p.x = (p.x%pattern_width()+pattern_width())%pattern_width();
		p.y = (p.y%pattern_height()+pattern_height())%pattern_height();
		
		return p;
	}

	std::array<std::size_t,4> evolved_center(std::span<const std::size_t,16> values) const
	{
		std::array<std::size_t,4> result;
		for(std::size_t y=1;y<3;++y)
		{
			for(std::size_t x=1;x<3;++x)
			{
				result[(y-1)*2+(x-1)] = values[y*4+x]==2?2:
					std::ranges::any_of(std::initializer_list
					{
						values[(y-1)*4+x],values[(y+1)*4+x],values[y*4+x-1],values[y*4+x+1]
					},[](auto v){ return v==1; })?1:0;
			}
		}
		return result;
	}

	std::array<std::size_t,4> background_square_at(point2d top_left) const
	{
		top_left = normalize(top_left);

		return
		{
			grid_[top_left.y][top_left.x]=='#'?2u:0u,
			grid_[top_left.y][(top_left.x+1)%pattern_width()]=='#'?2u:0u,
			grid_[(top_left.y+1)%pattern_height()][top_left.x]=='#'?2u:0u,
			grid_[(top_left.y+1)%pattern_height()][(top_left.x+1)%pattern_width()]=='#'?2u:0u
		};
	}


	private:
	std::vector<std::string> grid_;
};

int main()
{
	constexpr auto steps = 26501365;

	std::vector<std::string> grid{std::istream_iterator<std::string>{std::cin},{}};
	const int height = grid.size();
	const int width = grid[0].size();

	point2d start;
	for(int y=0;y<height;++y)
	{
		for(int x=0;x<width;++x)
		{
			if(grid[y][x]=='S')
				start = {x,y};
		}
	}

	hashlife::tree life{advent_garden_plot{grid},start};
	life.set(start,1);
	life.step(steps);

	std::cout<<life.number_of_living_cells();
	
	return 0;
}
