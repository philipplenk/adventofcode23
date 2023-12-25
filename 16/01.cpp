#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <cstdint>

struct point2d
{
	int x, y;
};

struct vec2d
{
	int x, y;
};

point2d operator+(const point2d& p, const vec2d v)
{
	return {p.x+v.x,p.y+v.y};
}

enum class direction: std::uint8_t
{
	left = 1,
	right = 1<<1,
	up = 1<<2,
	down = 1<<3
};

vec2d to_vec(direction d)
{
	switch(d)
	{
		case direction::left: return {-1,0};
		case direction::right: return {1,0};
		case direction::up: return {0,-1};
		case direction::down: return {0,1};
	}
	return {};
}

int main(int argc, char* argv[])
{
	std::vector<std::string> grid{std::istream_iterator<std::string>{std::cin},{}};
	const int height = grid.size();
	const int width = grid[0].size();

	const auto valid = [&](const point2d& p)
	{
		return p.x>=0 && p.y>=0 && p.x<width && p.y<height;
	};

	struct beam
	{
		point2d pos;
		direction dir;
	};

	std::vector<beam> beams;
	beams.push_back({{0,0},direction::right});

	std::vector<std::uint8_t> seen(width*height);
	
	while(!beams.empty())
	{
		auto next = beams.back();
		beams.pop_back();

		if(seen[next.pos.y*width+next.pos.x]&static_cast<std::uint8_t>(next.dir))
			continue;
		
		seen[next.pos.y*width+next.pos.x]|=static_cast<std::uint8_t>(next.dir);

		switch(grid[next.pos.y][next.pos.x])
		{
			case '.':
			{
				break;
			}
			case '/':
			{
				using enum direction;
				switch(next.dir)
				{
					case right: next.dir = up; break;
					case left: next.dir = down; break;
					case down: next.dir = left; break;
					case up: next.dir = right; break;
				}
				break;
			}
			case '\\':
			{
				using enum direction;
				switch(next.dir)
				{
					case right: next.dir = down; break;
					case left: next.dir = up; break;
					case down: next.dir = right; break;
					case up: next.dir = left; break;
				}
				break;
			}
			case '-':
			{
				if(next.dir==direction::up || next.dir==direction::down)
				{
					beams.push_back({next.pos,direction::left});
					next.dir = direction::right;
				}
				break;
			}
			case '|':
			{
				if(next.dir==direction::left || next.dir==direction::right)
				{
					beams.push_back({next.pos,direction::up});
					next.dir = direction::down;
				}
				break;
			}
		}

		next.pos = next.pos+to_vec(next.dir);
		if(valid(next.pos))
			beams.push_back(next);
	}

	std::cout<<std::ranges::count_if(seen,[](auto v){ return v>0; });

	return 0;
}
