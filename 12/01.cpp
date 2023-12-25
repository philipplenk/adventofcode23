#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

#include <cctype>
#include <cstddef>

struct disgusting_bitset
{
	__uint128_t bits{};
	std::size_t size{};
};

struct spring_record
{
	disgusting_bitset damaged;
	__uint128_t unknown{};
	
	std::vector<disgusting_bitset> groups;
};

std::istream& operator>>(std::istream& in, spring_record& r)
{
	r = {};

	for(char c; !std::isspace(in.peek()) && in>>c; ++r.damaged.size)
	{
		if(c=='#')
			r.damaged.bits|=(__uint128_t{1}<<r.damaged.size);
		if(c=='?')
			r.unknown|=(__uint128_t{1}<<r.damaged.size);
	}
	
	for(in>>std::ws;std::isdigit(in.peek());)
	{
		disgusting_bitset group;	
		in>>group.size;
		group.bits = (__uint128_t{1}<<group.size)-1;

		r.groups.push_back(group);
		
		char skip;
		if(in.peek()==',') in>>skip;
	}

	return in>>std::ws;
}

int main(int argc, char* argv[])
{
	const auto is_compatible = [&](const auto& spring, disgusting_bitset value, std::size_t pos)
	{
		const auto followup_position = pos+value.size;
		const auto followup_bit = (spring.damaged.bits>>followup_position)&1;

		if(followup_bit==1)
			return false;

		const auto positioned_bits = value.bits<<pos;
		const auto different = (positioned_bits & ~spring.damaged.bits);
		const auto ok = (different & ~spring.unknown)==0;
		
		return ok;
	};

	const auto count_options_rec = [&](const auto& spring, std::size_t start, std::size_t current_group_idx, auto rec)
	{
		if(current_group_idx>=spring.groups.size())
		{
			const auto remaining_bits = spring.damaged.bits>>start;
			return remaining_bits==0?1ll:0ll;
		}

		if(start>=spring.damaged.size)
			return 0ll;

		const auto current_group = spring.groups[current_group_idx];

		long long sum = 0;
		for(std::size_t pos = start; pos+current_group.size<=spring.damaged.size; ++pos)
		{
			if(is_compatible(spring, current_group, pos))
				sum+=rec(spring, pos+current_group.size+1,current_group_idx+1,rec);

			if((spring.damaged.bits>>pos) & 1) break;
		}
		
		return sum;
	};

	const auto count_options = [&](const auto& spring)
	{
		return count_options_rec(spring,0,0,count_options_rec);
	};
	
	std::cout<<std::transform_reduce(std::istream_iterator<spring_record>{std::cin},{},0ll,std::plus<>{},count_options);
	
	return 0;
}
