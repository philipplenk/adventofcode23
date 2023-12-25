#include <iostream>
#include <iterator>
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

point2d operator+(const point2d& p, const vec2d& v)
{
	return {p.x+v.x,p.y+v.y};
}

int main(int argc, char* argv[])
{
	std::vector<std::string> grid{std::istream_iterator<std::string>{std::cin},{}};
	const int height = grid.size();
	const int width = grid[0].size();

	const auto valid = [&](const point2d& p)
	{
		return p.x>=0 && p.y>=0 && p.x<width && p.y<height && grid[p.y][p.x]!='#';
	};

	point2d start;
	for(int y=0;y<height;++y)
	{
		for(int x=0;x<width;++x)
		{
			if(grid[y][x]=='S')
				start = {x,y};
		}
	}

	std::vector<point2d> to_visit;
	to_visit.push_back(start);
	for(int i=0;i<64;++i)
	{	
		std::vector<bool> seen(width*height,false);
		std::vector<point2d> next_to_visit;
		for(auto p: to_visit)
		{
			for(auto d: {vec2d{1,0},vec2d{-1,0},vec2d{0,1},vec2d{0,-1}})
			{
				const auto n = p + d;
				if(valid(n) && !seen[n.y*width+n.x])
				{
					seen[n.y*width+n.x] = true;
					next_to_visit.push_back(n);
				}
			}
		}

		to_visit = std::move(next_to_visit);
	}

	std::cout<<to_visit.size();
	return 0;
}
