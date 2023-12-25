#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <cstddef>

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

auto compress(const std::vector<std::string>& grid)
{
	const int height = grid.size();
	const int width = grid[0].size();

	const auto valid = [&](const auto& p)
	{
		return p.x>=0 && p.y>=0 && p.x<width && p.y<height && grid[p.y][p.x]!='#';
	};

	struct node
	{
		std::unordered_map<std::size_t,int> distance;
	};

	struct waypoint
	{
		point2d from, to;
		int len = 0;
	};

	const point2d start{0,0};
	
	std::vector<waypoint> to_visit;
	to_visit.push_back({start,start});

	std::vector<std::size_t> node_ids(width*height,0);
	std::vector<bool> seen(width*height,false);
	std::vector<node> nodes(1);
	std::size_t final_id = 0;
	
	while(!to_visit.empty())
	{
		auto [from,to, len] = to_visit.back();
		to_visit.pop_back();

		if(seen[to.y*width+to.x])
		{
			const auto from_id = node_ids[from.y*width+from.x];
			const auto to_id = node_ids[to.y*width+to.x];
			nodes[from_id].distance[to_id] = std::max(nodes[from_id].distance[to_id],len);
			nodes[to_id].distance[from_id] = std::max(nodes[to_id].distance[from_id],len);
			
			continue;
		}
		seen[to.y*width+to.x] = true;

		int number_of_neighbors = 0;
		for(auto d: {vec2d{1,0},vec2d{-1,0},vec2d{0,1},vec2d{0,-1}})
		{
			const auto n = to + d;
			if(valid(n) && n!=from)
				++number_of_neighbors;
		}

		if(number_of_neighbors>1 || to.y==height-1)
		{
			const auto from_id = node_ids[from.y*width+from.x];
			const auto to_id = nodes.size();
			if(to.y==height-1)
				final_id = to_id;
			
			nodes.push_back({});
			nodes[from_id].distance[to_id] = std::max(nodes[from_id].distance[to_id],len);
			nodes[to_id].distance[from_id] = std::max(nodes[to_id].distance[from_id],len);
			node_ids[to.y*width+to.x] = to_id;
			len = 0;
		}
		else
			node_ids[to.y*width+to.x] = node_ids[from.y*width+from.x];
		
		for(auto d: {vec2d{1,0},vec2d{-1,0},vec2d{0,1},vec2d{0,-1}})
		{
			const auto n = to + d;
			if(valid(n) && n!=from)
				to_visit.push_back({to,n,len+1});
		}
	}

	struct result_type
	{
		std::vector<node> nodes;
		std::size_t final_id;
	};
	return result_type{nodes,final_id};
}

int main(int argc, char* argv[])
{
	std::vector<std::string> grid{std::istream_iterator<std::string>{std::cin},{}};
	const auto [compressed,final_id] = compress(grid);

	std::vector<bool> visited(compressed.size(),false);
	const auto find_rec = [&](std::size_t n, auto rec)
	{
		if(n==final_id) return 0;
		
		visited[n] = true;
		int max = 0;
		for(auto [target,length]: compressed[n].distance)
		{
			if(!visited[target])
				max = std::max(max,length+rec(target,rec));
		}
		visited[n] = false;

		return max;
	};

	std::cout<<find_rec(0,find_rec)-1;

	return 0;
}
