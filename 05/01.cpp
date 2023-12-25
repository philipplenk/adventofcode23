#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>
#include <optional>
#include <string>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <vector>

struct mapping_range
{
	unsigned long source_start, destination_start, length;
	friend auto operator<=>(const mapping_range&, const mapping_range&) = default;
};

std::istream& operator>>(std::istream& in, mapping_range& range)
{
	return in>>range.destination_start>>range.source_start>>range.length;
}

struct mapping
{
	std::string from, to;
	std::vector<mapping_range> ranges;
};

std::istream& operator>>(std::istream& in, mapping& m)
{
	m.ranges.clear();

	std::string heading;
	if(!std::getline(in,heading))
		return in;

	const auto end_first = heading.find('-');
	const auto end_second = heading.find(' ');
	m.from = heading.substr(0,end_first);
	m.to = heading.substr(end_first+4,end_second-end_first-4);

	for(mapping_range rng; std::isdigit(in.peek()) && in>>rng; in.ignore(1))
		m.ranges.push_back(rng);
	
	std::sort(m.ranges.begin(),m.ranges.end());

	return in;
}

int main(int argc, char* argv[])
{
	std::string seed_string, skip;
	std::getline(std::cin,seed_string);
	std::stringstream seed_strm{seed_string};
	seed_strm>>skip;
	std::vector<unsigned long> seeds{std::istream_iterator<unsigned long>{seed_strm},{}};

	std::unordered_map<std::string,mapping> mappings;
	for(mapping m; std::cin>>std::ws>>m;)
		mappings[m.from] = m;

	const auto find_containing_range = [&](const std::vector<mapping_range>& ranges, unsigned long value) -> std::optional<mapping_range>
	{
		auto it = std::lower_bound(ranges.begin(),ranges.end(),value+1,[](const auto& lhs, const auto& rhs)
		{
			return lhs.source_start<rhs;
		});

		if(it==ranges.begin())
			return std::nullopt;

		it = std::prev(it);
		if(it->source_start+it->length<=value)
			return std::nullopt;

		return *it;
	};

	auto min = std::numeric_limits<unsigned long>::max(); 
	for(auto id: seeds)
	{
		std::string type = "seed";
		while(type!="location")
		{
			const auto& mapping = mappings[type];
		
			if(auto map = find_containing_range(mapping.ranges,id); map)
				id = map->destination_start + (id - map->source_start);

			type = mapping.to;
		}
		min = std::min(id,min);
	}
	std::cout<<min;
	
	return 0;
}
