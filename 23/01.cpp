#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

struct point2d
{
	int x, y;
	friend auto operator<=>(const point2d&, const point2d&) = default;
};

struct vec2d
{
	int x, y;
	friend auto operator<=>(const vec2d&, const vec2d&) = default;
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

	const auto valid = [&](const auto& p)
	{
		return p.x>=0 && p.y>=0 && p.x<width && p.y<height && grid[p.y][p.x]!='#';
	};
	
	point2d start{0,0};

	std::vector<int> length(width*height,0);

	struct waypoint
	{
		point2d from, to;
		int len = 0;
	};
	
	std::vector<waypoint> to_visit;
	to_visit.push_back({start,start});

	int max = 0;
	while(!to_visit.empty())
	{
		const auto [from,to, len] = to_visit.back();
		to_visit.pop_back();

		if(to.y==height-1)
			max = std::max(len,max);

		std::optional<vec2d> forced = std::nullopt;
		if(grid[to.y][to.x]=='<') forced = vec2d{-1,0};
		if(grid[to.y][to.x]=='>') forced = vec2d{1,0};
		if(grid[to.y][to.x]=='v') forced = vec2d{0,1};
		
		for(auto d: {vec2d{1,0},vec2d{-1,0},vec2d{0,1},vec2d{0,-1}})
		{
			if(forced && d!=*forced) continue;

			const auto n = to + d;
			if(valid(n) && n!=from)
				to_visit.push_back({to,n,len+1});
		}
	}

	std::cout<<max-1;
	
	return 0;
}
