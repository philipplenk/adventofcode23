#include <iostream>
#include <vector>

#include <cstddef>
#include <cstdint>

struct point2d
{
	long long x, y;
};

struct vec2d
{
	long long x, y;
};

vec2d operator*(const vec2d& v, long long f)
{
	return {v.x*f,v.y*f};
}

point2d operator+(const point2d& p, const vec2d& v)
{
	return {p.x+v.x,p.y+v.y};
}

enum class direction
{
	left, right, up, down
};

std::istream& operator>>(std::istream& in, direction& d)
{
	char c;
	in>>c;

	switch(c)
	{
		case 'L': d = direction::left; break;
		case 'R': d = direction::right; break;
		case 'U': d = direction::up; break;
		case 'D': d = direction::down; break;
	}

	return in;
}

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

struct dig_instruction
{
	direction dir;
	int depth;
	std::uint32_t color;
};

std::istream& operator>>(std::istream& in, dig_instruction& d)
{
	char skip;
	return in>>d.dir>>std::dec>>d.depth>>skip>>skip>>std::hex>>d.color>>skip;
}

int main(int argc, char* argv[])
{
	point2d current{0,0};

	std::vector<point2d> boundary_points;
	long long points_on_bounary = 0;
	for(dig_instruction d;std::cin>>d;)
	{
		const auto dir = to_vec(d.dir);
		current = current + dir*d.depth;
		points_on_bounary+=d.depth;
		boundary_points.push_back(current);
	}

	// Shoelace formula to compute the area
	// 2*A = (x_0*y_1)+(x1*y_2)...-(y0*x1)-(y1*x2)...
	long long total_area_2 = 0;
	for(std::size_t i=0;i<boundary_points.size();++i)
	{
		total_area_2+=boundary_points[i].x*boundary_points[(i+1)%boundary_points.size()].y;
		total_area_2-=boundary_points[i].y*boundary_points[(i+1)%boundary_points.size()].x;
	}
	total_area_2 = std::abs(total_area_2);

	// Picks theorem to compute number of integer points:
	// A = interior_points + boundary_points/2 - 1
	// => 2*A = 2*interior_points + boundary_points - 2
	// => Number of integer points = (2*A - boundary_points + 2)/2 + boundary_points

	std::cout<<(total_area_2 - points_on_bounary + 2)/2 + points_on_bounary;

	return 0;
}
