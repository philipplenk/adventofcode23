#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
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
			std::unordered_set<int> blocked_by;
			for(int y=min_y;y<=max_y;++y)
			{
				for(int x=min_x;x<=max_x;++x)
				{
					if(resting.contains({x,y,next_z}))
					{
						blocked_by.insert(resting[{x,y,next_z}]);
						blocked = true;
					}
				}
			}

			if(blocked_by.size()==1)
				essential.insert(*blocked_by.begin());

			if(blocked)
				break;

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

	std::cout<<bricks.size() - essential.size();

	return 0;
}
