#include <iostream>
#include <iterator>
#include <queue>
#include <string>
#include <vector>

#include <cstdint>

struct point2d
{
	int x,y;
	friend bool operator<=>(const point2d&, const point2d&) = default;
};

struct vec2d
{
	int x,y;
};

point2d operator+(const point2d& p, const vec2d& v)
{
	return {p.x+v.x,p.y+v.y};
};

point2d operator-(const point2d& p, const vec2d& v)
{
	return {p.x-v.x,p.y-v.y};
};

vec2d operator*(const vec2d& v, int factor)
{
	return {v.x*factor,v.y*factor};
}

int main(int argc, char* argv[])
{
	std::vector<std::string> grid(std::istream_iterator<std::string>{std::cin},{});
	const int height = grid.size();
	const int width = grid[0].size();

	const auto valid = [&](const point2d p)
	{
		return p.x>=0 && p.y>=0 && p.x<width && p.y<height;
	};

	struct waypoint
	{
		point2d pos;
		vec2d last_direction;
		int cost;

		bool operator<(const waypoint& other) const
		{
			return other.cost<cost;
		}
	};

	std::priority_queue<waypoint> to_visit;
	to_visit.push({{0,0},{0,1},0});
	to_visit.push({{0,0},{1,0},0});

	const point2d target{width-1,height-1};

	std::vector<std::uint8_t> visited(width*height,0);
	while(!to_visit.empty())
	{
		const auto next = to_visit.top();
		to_visit.pop();

		if(next.pos==target)
		{
			std::cout<<next.cost;
			break;
		}

		const auto visited_bit = next.last_direction.x==0?1:2;

		if(visited[next.pos.y*width+next.pos.x]&visited_bit)
			continue;

		visited[next.pos.y*width+next.pos.x] |= visited_bit;

		auto new_dir = next.last_direction.x==0?vec2d{1,0}:vec2d{0,1};
		auto forward = next.pos;
		auto backward = next.pos;
		int forward_cost = 0, backward_cost = 0;
		for(int i=0;i<10;++i)
		{
			forward = forward+new_dir;
			backward = backward-new_dir;
			if(valid(forward))
			{
				forward_cost+=grid[forward.y][forward.x]-'0';
				if(i>=3)
					to_visit.push({forward,new_dir,next.cost+forward_cost});
			}
			if(valid(backward))
			{
				backward_cost+=grid[backward.y][backward.x]-'0';
				if(i>=3)
					to_visit.push({backward,new_dir*-1,next.cost+backward_cost});
			}
		}
	}

	return 0;
}
