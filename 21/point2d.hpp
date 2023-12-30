#ifndef POINT2D_H
#define POINT2D_H

#include <bit>
#include <functional>

#include <cstddef>

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
			std::size_t result = p.x;
			result = std::rotl(result,1)^p.y;
			return result;
		}
	};
}

#endif
