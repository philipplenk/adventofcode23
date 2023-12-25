#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <optional>
#include <ranges>
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

struct range
{
	unsigned long start, length;
	friend auto operator<=>(const range&, const range&) = default;
};

std::istream& operator>>(std::istream& in, range& r)
{
	return in>>r.start>>r.length;
}

std::optional<range> try_cut_front(range& to_cut, const mapping_range& cut_with)
{
	if(to_cut.length==0)
		return std::nullopt;

	if(to_cut.start>=cut_with.source_start+cut_with.length)
		return std::nullopt;

	if(to_cut.start+to_cut.length<=cut_with.source_start)
		return std::nullopt;

	if(to_cut.start<cut_with.source_start)
	{
		const auto prefix_length = cut_with.source_start - to_cut.start;
		const range new_range{ .start = to_cut.start, .length = prefix_length };

		to_cut.start = cut_with.source_start;
		to_cut.length-=prefix_length;

		return new_range;
	}

	const auto common_end = std::min(to_cut.start+to_cut.length,cut_with.source_start+cut_with.length);
	const auto common_length = common_end - to_cut.start;
	const auto mapped_start = to_cut.start - cut_with.source_start + cut_with.destination_start;
	const range new_range{ .start = mapped_start, .length = common_length };

	to_cut.start+=common_length;
	to_cut.length-=new_range.length;
	
	return new_range;
}

int main(int argc, char* argv[])
{
	std::string seed_string, skip;
	std::getline(std::cin,seed_string);
	std::stringstream seed_strm{seed_string};
	seed_strm>>skip;
	const std::vector<range> seed_ranges{std::istream_iterator<range>{seed_strm},{}};

	std::unordered_map<std::string,mapping> mappings;
	for(mapping m; std::cin>>std::ws>>m;)
		mappings[m.from] = m;

	const auto apply_mapping = [](const std::vector<range>& current_ranges, const std::vector<mapping_range> mapping_ranges)
	{
		std::vector<range> new_ranges;
		for(auto current_range: current_ranges)
		{
			for(const auto& map_range: mapping_ranges)
			{
				for(std::optional<range> new_range; (new_range = try_cut_front(current_range, map_range));)
					new_ranges.push_back(*new_range);

				if(current_range.length==0)
					break;
			}
			if(current_range.length>0)
				new_ranges.push_back(current_range);
		}
		return new_ranges;
	};

	const auto transform_range = [&](range rng)
	{
		std::string current_type = "seed";
		std::vector<range> current_ranges(1,rng);
		
		while(current_type!="location")
		{
			const auto& mapping = mappings[current_type];
			current_ranges = apply_mapping(current_ranges, mapping.ranges);
			current_type = mapping.to;
		}

		return current_ranges;
	};

	const auto min_start = [](const std::vector<range>& ranges)
	{
		return std::min_element(ranges.begin(),ranges.end())->start;
	};

	std::cout<<std::ranges::min(seed_ranges | std::views::transform(transform_range) | std::views::transform(min_start));
	
	return 0;
}
