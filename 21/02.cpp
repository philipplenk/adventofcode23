#include <array>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_set>
#include <vector>

struct point2d
{
	long long x, y;
	friend auto operator<=>(const point2d&, const point2d&) = default;
};

namespace std
{
	template <>
	struct hash<point2d>
	{
		auto operator()(const point2d& p) const
		{
			return (p.y+300)*600+p.x+300;
		}
	};
}

struct vec2d
{
	long long x, y;
};

point2d operator+(const point2d& p, const vec2d& v)
{
	return {p.x+v.x,p.y+v.y};
}

auto determinant(const std::array<std::array<long long,3>,3>& mat)
{
	return 
		mat[0][0]*(mat[1][1]*mat[2][2]-mat[1][2]*mat[2][1]) - 
		mat[0][1]*(mat[1][0]*mat[2][2]-mat[1][2]*mat[2][0]) +
		mat[0][2]*(mat[1][0]*mat[2][1]-mat[1][1]*mat[2][0]);
}

// Fits a quadratic equation via cramers rule
auto quadratic_coefficients(point2d a, point2d b, point2d c)
{
	std::array<point2d,3> points{a,b,c};
	const auto coeff_mat = std::array<std::array<long long,3>,3>
	{{
		{{a.x*a.x,a.x,1}},
		{{b.x*b.x,b.x,1}},
		{{c.x*c.x,c.x,1}},
	}};

	const auto det = determinant(coeff_mat);

	const auto det_for = [&](int col)
	{
		auto mat = coeff_mat;
		for(int y=0;y<3;++y)
			mat[y][col] = points[y].y;

		return determinant(mat);
	};

	struct
	{
		long long a,b,c;
	} result {det_for(0)/det,det_for(1)/det,det_for(2)/det};

	return result;
}

/*
	Quite a horrid solution today :)
	I would apologize, but I am not sure this torture of a puzzle deserves any better...
	As rather frequently this year, we had to make a bunch of assumptions about out
	input files and this time they didn't even hold true for the example input!

	Anyway, due to some properties of the given mazes(a straight, uninterrupted line
	toward the borders) the repeated ones turned out to be nicely diamond shaped and
	(largely) independent of each other.
	The peculiar number of steps - 26501365 - turned out to be congruent to half the input size
	modulo the input size. (i.e. it was 131*n + 65).

	As it takes exactly half the input size many steps to fill the first, inner diamond
	(because we started exactly in the middle and were not interrupted), this implies
	we get a nice, completely filled diamond shape and can even compute of how gardens they consist.

	There are a few different types at the end and with alternating parities, all of which
	could be counted once using code similar to part1. We could then compute the exact number
	of each type, multiply and add them and get our nice, accurate result.

	Once I had figured that out, however, a few hours of madness had passed and I had
	neither the patience nor mental capacity to complete that reasoning and calculation "properly". 

	Which is why I "cheated": 
	Knowing that the total number - if looked at in fixed intervals of input_size iterations starting at
	iteration input_size/2 - will follow some pattern and guessing(correctly as it turned out) that it
	will be quadratic, I simply sampled 3 of those points, fitted a quadratic equation onto them and
	called it a night.
*/

int main(int argc, char* argv[])
{
	std::vector<std::string> grid{std::istream_iterator<std::string>{std::cin},{}};
	const int height = grid.size();
	const int width = grid[0].size();

	const auto normalize = [&](point2d p)
	{
		p.x%=width;
		p.y%=height;
		p.x = (p.x+width)%width;
		p.y = (p.y+height)%height;
		return p;
	};

	const auto valid = [&](point2d p)
	{
		p = normalize(p);
		return grid[p.y][p.x]!='#';
	};

	point2d start{};
	for(int y=0;y<height;++y)
	{
		for(int x=0;x<width;++x)
		{
			if(grid[y][x]=='S')
				start = {x,y};
		}
	}

	std::vector<point2d> sample_points;
	const auto sample_steps = 2;
	
	std::vector<point2d> to_visit;
	to_visit.push_back(start);
	for(int i=0;i<sample_steps*width+width/2;++i)
	{	
		std::unordered_set<point2d> seen;
		std::vector<point2d> next_to_visit;
		for(auto p: to_visit)
		{
			for(auto d: {vec2d{1,0},vec2d{-1,0},vec2d{0,1},vec2d{0,-1}})
			{
				const auto n = p + d;
				if(valid(n) && !seen.contains(n))
				{
					seen.insert(n);
					next_to_visit.push_back(n);
				}
			}
		}

		to_visit = std::move(next_to_visit);

		if((i+1)%width==width/2)
			sample_points.push_back(point2d(sample_points.size(),to_visit.size()));
	}	

	const auto steps = 26501365LL;
	const auto x = steps/width;
	const auto coeff = quadratic_coefficients(sample_points.front(), sample_points[1], sample_points[2]); 
	const auto result = (coeff.a*x*x+coeff.b*x+coeff.c);
	std::cerr<<coeff.a<<"*x^2+"<<coeff.b<<"*x+"<<coeff.c<<" = "<<result<<'\n';

	std::cout<<result;

	return 0;
}
