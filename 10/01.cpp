#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

struct point2d
{
	int x, y;
	friend constexpr bool operator<=>(const point2d&, const point2d&) noexcept = default;
};

enum class direction
{
	up, down, left, right
};

std::ostream& operator<<(std::ostream& out, const direction& d)
{
	switch(d)
	{
		case direction::up: return out<<"up";
		case direction::down: return out<<"down";
		case direction::left: return out<<"left";
		case direction::right: return out<<"right";
	}

	return out; 
}

direction invert(direction d)
{
	switch(d)
	{
		case direction::up: return direction::down;
		case direction::down: return direction::up;
		case direction::left: return direction::right;
		case direction::right: return direction::left;
	}

	return d;
}

std::optional<direction> followup_direction(char c, direction incoming)
{
	switch(c)
	{
		case '|':
		{
			if(incoming==direction::down) return direction::up;
			if(incoming==direction::up) return direction::down;
			return std::nullopt;
		}
		case '-':
		{
			if(incoming==direction::left) return direction::right;
			if(incoming==direction::right) return direction::left;
			return std::nullopt;
		}
		case 'L':
		{
			if(incoming==direction::right) return direction::up;
			if(incoming==direction::up) return direction::right;
			return std::nullopt;
		}
		case 'J':
		{
			if(incoming==direction::left) return direction::up;
			if(incoming==direction::up) return direction::left;
			return std::nullopt;
		}
		case 'F':
		{
			if(incoming==direction::down) return direction::right;
			if(incoming==direction::right) return direction::down;
			return std::nullopt;
		}
		case '7':
		{
			if(incoming==direction::down) return direction::left;
			if(incoming==direction::left) return direction::down;
			return std::nullopt;
		}
	}
	return std::nullopt;
}

point2d move(point2d p, direction d)
{
	switch(d)
	{
		case direction::up: return {p.x,p.y-1};
		case direction::down: return {p.x,p.y+1};
		case direction::left: return {p.x-1,p.y};
		case direction::right: return {p.x+1,p.y};
	}

	return p; 
}

int main(int argc, char* argv[])
{
	std::vector<std::string> grid{std::istream_iterator<std::string>{std::cin},{}};
	const int height = grid.size();
	const int width = grid[0].size();

	point2d start{};
	for(int y=0;y<height;++y)
	{
		for(int x=0;x<width;++x)
		{
			if(grid[y][x]=='S')
				start = {x,y};
		}
	}

	const auto valid = [&](const point2d& p)
	{
		return p.x>=0 && p.y>=0 && p.x<width && p.y<height;
	};

	const auto walk = [&](auto start, direction from)
	{
		auto first_target = move(start,from);
		from = invert(from);

		int steps = 1;
		for(auto current = first_target; valid(current) && current!=start;++steps)
		{
			auto move_to = followup_direction(grid[current.y][current.x],from);
			if(move_to)
			{
				current = move(current,*move_to);
				from = invert(*move_to);
			}
			else
				return 0;
		}
		return steps;
	};

	int length = 0;
	for(auto d: {direction::up,direction::left,direction::down,direction::right})
		length = std::max(length,walk(start,d));

	std::cout<<length/2;

	return 0;
}
