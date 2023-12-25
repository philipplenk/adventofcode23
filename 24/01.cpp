#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <vector>

#include <cstddef>

struct point3d
{
	long double x, y, z;
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

struct vec3d
{
	long double x, y, z;
	friend auto operator<=>(const vec3d& lhs, const vec3d& rhs) = default;
};

vec3d operator-(const vec3d& lhs, const vec3d& rhs)
{
	return {lhs.x-rhs.x,lhs.y-rhs.y,lhs.z-rhs.z};
}

point3d operator+(const point3d& lhs, const vec3d& rhs)
{
	return {lhs.x+rhs.x,lhs.y+rhs.y,lhs.z+rhs.z};
}

vec3d operator-(const point3d& lhs, const point3d& rhs)
{
	return {lhs.x-rhs.x,lhs.y-rhs.y,lhs.z-rhs.z};
}

template <typename T>
vec3d& operator/=(vec3d& v, T numeric)
{
	v.x/=numeric;
	v.y/=numeric;
	v.z/=numeric;
	return v;
}

template <typename T>
vec3d operator/(vec3d v, T numeric)
{
	return v/=numeric;
}

std::istream& operator>>(std::istream& in, vec3d& v)
{
	in>>v.x;
	in.ignore(1);
	in>>v.y;
	in.ignore(1);
	in>>v.z;

	return in;
}

struct hailstone
{
	point3d position;
	vec3d velocity;
};

std::istream& operator>>(std::istream& in, hailstone& h)
{
	char skip;
	return in>>h.position>>skip>>h.velocity;
}

std::optional<point3d> intersection(const hailstone& lhs, const hailstone& rhs)
{
	const auto p0 = lhs.position;
	const auto p1 = lhs.position+lhs.velocity;
	const auto p2 = rhs.position;
	const auto p3 = rhs.position+rhs.velocity;
	
	const double t0_num = (p0.x-p2.x)*(p2.y-p3.y) - (p0.y-p2.y)*(p2.x-p3.x);
	const double den = (p0.x-p1.x)*(p2.y-p3.y) - (p0.y-p1.y)*(p2.x-p3.x);

	if(den==0)
		return std::nullopt;
	
	const double t1_num = (p0.x-p2.x)*(p0.y-p1.y) - (p0.y-p2.y)*(p0.x-p1.x);

	const auto t0 = t0_num/den;
	const auto t1 = t1_num/den;

	if(t0<0 || t1<0)
		return std::nullopt;

	return point3d(lhs.position.x + lhs.velocity.x*t0, lhs.position.y+lhs.velocity.y*t0,0);
}

int main(int argc, char* argv[])
{
	std::vector<hailstone> hailstones{std::istream_iterator<hailstone>{std::cin},{}};

	const auto low = 200000000000000;
	const auto high = 400000000000000;
	int sum = 0;
	for(std::size_t i=0;i<hailstones.size();++i)
	{
		for(std::size_t j=i+1;j<hailstones.size();++j)
		{
			if(auto ipos = intersection(hailstones[i],hailstones[j]); ipos)
				sum+=ipos->x>=low && ipos->y>=low && ipos->x<=high && ipos->y<=high;
		}
	}
	std::cout<<sum;
	
	return 0;
}
