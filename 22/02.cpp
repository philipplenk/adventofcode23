#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct point3d
{
	int x, y, z;
	friend auto operator<=>(const point3d& lhs, const point3d& rhs) = default;
};

std::istream& operator>>(std::istream& in, point3d& p)
{
	in>>p.x;
	in.ignore(1);
	in>>p.y;
	in.ignore(1);
	in>>p.z;

	return in;
}

struct brick
{
	point3d start, end;
};

std::istream& operator>>(std::istream& in, brick& b)
{
	in>>b.start;
	in.ignore(1);
	return in>>b.end;
}

int main(int argc, char* argv[])
{
	std::vector<brick> bricks(std::istream_iterator<brick>{std::cin},{});

	std::sort(bricks.begin(),bricks.end(),[&](const auto& lhs, const auto& rhs)
	{
		return std::min(lhs.start.z,lhs.end.z)<std::min(rhs.start.z,rhs.end.z);
	});

	std::map<point3d,int> resting;
	std::unordered_set<int> essential;
	std::unordered_map<int,std::unordered_set<int>> blocking;
	std::unordered_map<int,std::unordered_set<int>> blocked_by;
	for(int id=0; auto& b: bricks)
	{
		while(std::min(b.start.z,b.end.z)>1)
		{
			int next_z = std::min(b.start.z,b.end.z)-1;

			const auto min_x = std::min(b.start.x,b.end.x);
			const auto max_x = std::max(b.start.x,b.end.x);
			const auto min_y = std::min(b.start.y,b.end.y);
			const auto max_y = std::max(b.start.y,b.end.y);

			bool blocked = false;
			std::unordered_set<int> blocked_by_this;
			for(int y=min_y;y<=max_y;++y)
			{
				for(int x=min_x;x<=max_x;++x)
				{
					if(resting.contains({x,y,next_z}))
					{
						blocked_by_this.insert(resting[{x,y,next_z}]);
						blocked = true;
					}
				}
			}

			if(blocked_by_this.size()==1)
			{
				essential.insert(*blocked_by_this.begin());

			}
			for(auto b: blocked_by_this)
				blocking[b].insert(id);

			if(blocked)
			{
				blocked_by[id] = blocked_by_this;
				break;
			}

			--b.start.z;
			--b.end.z;
		}
		
		const auto min_x = std::min(b.start.x,b.end.x);
		const auto max_x = std::max(b.start.x,b.end.x);
		const auto min_y = std::min(b.start.y,b.end.y);
		const auto max_y = std::max(b.start.y,b.end.y);
		const auto min_z = std::min(b.start.z,b.end.z);
		const auto max_z = std::max(b.start.z,b.end.z);
		for(int y=min_y;y<=max_y;++y)
		{
			for(int x=min_x;x<=max_x;++x)
			{
				for(int z=min_z;z<=max_z;++z)
					resting[{x,y,z}] = id;
			}
		}
		++id;
	}

	const auto count = [&](int id)
	{
		std::size_t result = 0;
		
		auto blocked_by_ = blocked_by;

		std::unordered_set<int> to_remove, removed;
		to_remove.insert(id);
		while(!to_remove.empty())
		{
			std::unordered_set<int> to_remove_next;
			for(auto next: to_remove)
			{
				++result;
				for(auto erase_from: blocking[next])
				{
					blocked_by_[erase_from].erase(next);
					if(blocked_by_[erase_from].empty() && !removed.contains(erase_from))
					{
						removed.insert(erase_from);
						to_remove_next.insert(erase_from);
					}
				}
			}

			to_remove = std::move(to_remove_next);
		}
		return result - 1;
	};

	std::size_t sum = 0;
	for(std::size_t id=0;id<bricks.size();++id)
		sum+=count(id);

	std::cout<<sum;

	return 0;
}
