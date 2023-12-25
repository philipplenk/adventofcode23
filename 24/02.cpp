#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <ranges>
#include <unordered_set>
#include <vector>

#include <cstddef>
#include <cstdint>
#include <cmath>

struct point3d
{
	long long x, y, z;
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

struct vec3d
{
	long long x, y, z;
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
vec3d& operator*=(vec3d& v, T numeric)
{
	v.x*=numeric;
	v.y*=numeric;
	v.z*=numeric;
	return v;
}

template <typename T>
vec3d operator*(vec3d v, T numeric)
{
	return v*=numeric;
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

bool do_intersect(const hailstone& lhs, const hailstone& rhs)
{
	if(lhs.velocity==rhs.velocity) return false;
	
	const auto t = (rhs.position.x-lhs.position.x)/(lhs.velocity.x-rhs.velocity.x);
	if(t<0) return false;

	const auto real = lhs.position + lhs.velocity * t;
	const auto target = rhs.position + rhs.velocity * t;

	return real==target;
};

/*
	In order to find a unique solution, I once again had to make a bunch of assumptions based
	on my input. At the time of this writing, I have not yet checked reddit or others solutions,
	so I do not know if such assumptions were generally needed or if I am just to stupid to find a
	more general approach. 

	Anyway: After some inspection, soul searching and no small amount of despair, I gazed upon the
	chaos that is my input file and finally noticed something slightly peculiar: Many velocities
	shared one coordinate! That seemed like something I could abuse!
	After some pondering, I devised a cunning plan, based on the following observations:
		
		- If two stones always travel at the same speed (on one axis), their distance (on that axis)
		  never changes.
		  
		- Therefore, when hitting the first one of them, I know exactly what additional distance I
		  will have to cross. Namely: The distance between their starting points.
		  
		- Furthermore, the components of a few velocity vectors I checked seemed to be co-prime,
		  i.e. they shared no common factors except for 1. From that, the wording about *exactly*
		  the same position and the requirement for integer starting points and velocities, I jumped
		  to the - most likely erroneous but exceedingly useful - conclusion that all of our hits
		  will themselves be on positive integer coordinates. So we'll restrict ourselves to those. 
		  Every alternative, be it fractional, real, transcendental or otherwise supernatural
		  will be ignored.

		- Provided this assumption holds, we can draw an incredible conclusion from the first
		  two observations above: The distance our murderous rock has to cross(or rather make up for)
		  for a second misdeed after one innocent hailstone has been brutally obliterated *must*
		  *always* be divisible by its relative velocity.
		  Evenly divisible, i.e. without a remainder. I use the term "relative velocity" here
		  to refer to velocity relative to the target, i.e. rock_velocity - target_velocity, as we
		  have to "catch up".

		- This implies the relative velocity (still only talking about a single axis) has to be
		  *one of the prime factors* of the distance between the two targets on first impact.
		  In most cases we do not know that distance, as it would depend on the exact time of
		  contact if our two hapless victims happen to move away from each other. *If*...

		- Recall the first observation. Two stones travelling at *the same speed* never move away
		  from each other. Their distance is constant, independent of time. They are forced to
		  follow each other for all eternity, always moving, never touching, perpetually out of reach.
		  Luckily for us, the beautiful tragedy of their forlorn existence aids in their destruction.
		  
		  We know their distance. We know their velocity. We have reduced the search space for our
		  own velocity to a finite, reasonably small number of possible alternatives. 
		  For each factor of the distance, we obtain one possible velocity:
			  rock_velocity - target_velocity = factor
			  => rock_velocity = factor + target_velocity
		
		- Repeating that process for another set of stones yields another set of factors and potential
		  velocities. Our real velocity, however, has to satisfy the conditions for *all* pairs so,
		  so we can eliminate all other options, keeping only the common ones that are shared between
		  all pairs.
		  It turns out doing this for my input produces a very small number of alternatives. *Very* small.
		  Tiny even. Just 1!

		- Repeating that process for all other axes, all 2 of them, yields exactly one number for each.

		With that, we have figured out our velocity! Yeahy! Cheers and celebrations to you! Half of our
		work here is done! And it only took ... oh... several... hours.... I am very sane, trust me!

		Anywaaaaay...
		We still need to compute our starting point, which, fortuitously is comparatively trivial. Observe
		the dubious results of my far away high school math education:
			Let *S* be our starting point and *v* be our velocity. 
			Let *S0* be the starting point of an arbitrarily selected stone and *v0* be its velocity.

			Our position at any given time *t* can then be described as:

				S + v*t

			Almost the same can be said about the target stone. Its position is computed as:

				S0 + v0*t

			These two snuggle up really close upon hitting each other. So close they become one:

				S + v*t0 = S0 + v0*t0

			With *t0* being the time of impact.
			The same remains true for any other given stone at the time of its impact. Let's call
			the relevant variables S1, v1 and t1

				S + v*t1 = S1 + v1*t1

			Beautiful! Now why is this useful again? Well, we have 2 equations with 3 unknowns and
			as we all know that... oh. OH! That is one unknown too many. Alright, that's it. Let's
			give up, pack up and return to the oblivion from whence we came...

			But wait! There's more! We know t0 and t1 are not exactly independent. For some stones
			we even know a surpring lot about their relationship! Intimate details!
			Earlier, we used the distance in space to figure out our velocity. Now that we know our
			velocity, we can use it to compute the distance in time. 
			For that, let us again choose our target stones wisely and select two who share one component.
			Without loss of generality, let us assume they share their x-velocity.
			We know their distance. We know their velocity. We know our velocity. Therefore:
				
				t1 = t0 + time_it_takes_to_make_up_for_remaining_distance
				t1 = t0 + distance/relative_velocity

			distance is simply the distance between their starting points, i.e. S1 - S0.
			relative velocity is the difference between our velocity and that of the second
			target, i.e. velocity - v1

			For better readability we shall call this summand delta_t, as it describes the time passed
			between the two impacts.

			We can now rewrite our previous equations, yielding two equations with only two unknowns:

				I:  S + t0*v = S0 + t0*v0
				II: S + (t0 + delta_t)*v = S1 + (t0 + delta_t)*v1

			Great! That is something we can actually solve. First expand the products:

				I:  S + t0*v = S0 + t0*v0
				II: S + t0*v + delta_t*v = S1 + t0*v1 + delta_t*v1

			Now subtract I from II:

				II - I: (S + t0*v + delta_t*v) - (S + t0*v) = (S1 + t0*v1 + delta_t*v1) - (S0 + t0*v0)

			Rearrange a bit:

				(S-S) + (t0*v - t0*v) + delta_t*v = (S1 - S0) + t0*v1 + delta_t*v1 - t0*v0

			Eliminate the worthless:

				delta_t*v = (S1 - S0) + t0*v1 + delta_t*v1 - t0*v0

			Rearrange some more to get all t0's isolated on one side:

				t0*v0 - t0*v1 = (S1 - S0) + delta_t*v1 - delta_t*v

			Factorize:

				t0 * (v0-v1) = (S1 - S0) + delta_t * (v1-v)

			And divide:

				t0 = ((S1 - S0) + delta_t * (v1-v)) / (v0-v1)

		Alright, very simple, let's plug in some numbers and see what we get:

			*universe explodes*

		Oh shi... I divided by zero. 
		Recall, I specifically selected the two targets to have some with the same velocity!

			v0 - v1 == 0

		That turns out to be a bit of a problem here...

		Except it isn't! 3D to the rescue!
		Up to this point, I always worked with a single axis, completely neglecting the fact that
		we actually have three components per vector!
		Almost all of the math above - all except for the delta_t calculation in fact - still holds
		for the other 2 components. So we can just plug *one of the other components* into the 
		resulting equation, sidestep the division by zero problem and obtain a glorious result!

		Makes sense?
		I sincerely hope it does. I might be ever so slightly tired and not-quite-so-slightly mad
		at this point.

		Nonetheless, this method produced the correct result for my input. Enjoy :)
*/


/*
	First, here is the objectively worst way to factor a number. It is the slowest part of this
	solution and there are hundreds of ways to optimize it, but I found thousands of reasons not
	to care anymore.
*/ 
auto factors(long long value)
{
	std::unordered_set<long long> values;
	
	auto abs_value = std::abs(value);
	for(long long i=1; i*i<=abs_value;++i)
	{
		if(abs_value%i==0)
		{
			values.insert(abs_value/i);
			values.insert(i);
		}
	}

	return values;
}

/*
	Now, for a given component, find all pairs of hailstones whose velocities are equal in that component.
	Find the distance to travel, compute the factors of that distance and add them to the target vectors.
	Filter all those results not present in all pairs and return the resulting possibilities. 	
*/
auto enumerate_possible_velocities(const std::vector<hailstone>& hailstones, auto get_position, auto get_velocity)
{
	bool first = true;
	
	std::unordered_set<long long> possible_velocities;
	for(std::size_t i=0;i<hailstones.size();++i)
	{
		for(std::size_t j=i+1;j<hailstones.size();++j)
		{
			const auto& velocity0 = std::invoke(get_velocity,hailstones[i].velocity);
			const auto& velocity1 = std::invoke(get_velocity,hailstones[j].velocity);
			if(velocity0==velocity1)
			{
				const auto& position0 = std::invoke(get_position,hailstones[i].position);
				const auto& position1 = std::invoke(get_position,hailstones[j].position);
				const auto distance = position0 - position1;
				if(distance==0) continue;
				
				const auto possible_factors = factors(distance);
				std::unordered_set<long long> new_velocities;
				for(auto factor: possible_factors)
				{
					const auto v0 = velocity0-factor;
					const auto v1 = velocity0+factor;
					new_velocities.insert(v0);
					new_velocities.insert(v1);
				}
				
				if(first)
					possible_velocities = new_velocities;
				else
					std::erase_if(possible_velocities,[&](auto v){ return !new_velocities.contains(v); });

				first = false;
			}
		}
	}
	return possible_velocities;
}

auto select_computation_pair(const std::vector<hailstone>& stones)
{
	struct return_type
	{
		hailstone first, second;
	};
	
	for(std::size_t i=0; i<stones.size(); ++i)
	{
		for(std::size_t j=i+1; j<stones.size(); ++j)
		{
			const auto common_x_velocity = stones[i].velocity.x==stones[j].velocity.x;
			const auto different_y_velocity = stones[i].velocity.y!=stones[j].velocity.y;
			const auto different_x_position = stones[i].position.x!=stones[j].position.x;
			if(common_x_velocity && different_y_velocity && different_x_position)
				return std::make_optional(return_type{stones[i],stones[j]});
		}
	}

	return std::optional<return_type>(std::nullopt);
}

int main(int argc, char* argv[])
{
	std::vector<hailstone> hailstones{std::istream_iterator<hailstone>{std::cin},{}};

	const auto intersects_all = [&](const hailstone& thrown)
	{
		return std::ranges::all_of(hailstones,[&](const auto& h)
		{
			return do_intersect(thrown,h);
		});
	};
	
	const auto vxs = enumerate_possible_velocities(hailstones,&point3d::x,&vec3d::x);
	const auto vys = enumerate_possible_velocities(hailstones,&point3d::y,&vec3d::y);
	const auto vzs = enumerate_possible_velocities(hailstones,&point3d::z,&vec3d::z);

	const auto computation_pair = select_computation_pair(hailstones);
	if(!computation_pair)
	{
		std::cerr<<"All hope abandon...\n";
		return 1;
	}
	const auto [first_stone, second_stone] = *computation_pair;

	// In my case, there was only one possibility per component, but I am not sure that is a given
	// Therefore, let's loop, just to be sure:
	for(auto vx: vxs)
	{
		for(auto vy: vys)
		{
			for(auto vz: vzs)
			{
				vec3d velocity{vx,vy,vz};

				const auto delta_t = (second_stone.position.x - first_stone.position.x)/(velocity.x-second_stone.velocity.x);
				const auto start0 = first_stone.position.y;
				const auto start1 = second_stone.position.y;
				const auto delta_s = start1-start0;
				const auto v0 = first_stone.velocity.y;
				const auto v1 = second_stone.velocity.y;
				const auto delta_v = v0 - v1;

				const auto first_t = (delta_s + delta_t * (v1 - velocity.y))/delta_v;
				const auto p0 = first_stone.position + first_stone.velocity * first_t;

				const auto starting_position = p0 + velocity*-first_t;

				if(intersects_all({starting_position,velocity}))
				{
					std::cout<<starting_position.x+starting_position.y+starting_position.z;
					return 0;
				}

			}
		}
	}
	
	return 0;
}

// Aaaaand it turns out I was indeed stupid and more general and efficient solutions are indeed possible. Another learning experience :)
