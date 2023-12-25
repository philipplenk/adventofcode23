#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <cmath>

int main(int argc, char* argv[])
{
	std::string time_str, distance_str;
	std::getline(std::cin,time_str);
	std::getline(std::cin,distance_str);

	std::stringstream time_strm{time_str}, distance_strm{distance_str};
	std::string skip;
	time_strm>>skip;
	distance_strm>>skip;
	std::vector<int> times{std::istream_iterator<int>{time_strm},{}};
	std::vector<int> distances{std::istream_iterator<int>{distance_strm},{}};

	long product = 1;
	for(std::size_t i=0;i<times.size();++i)
	{
		const auto det = times[i]*times[i] - 4*distances[i];
		const auto det_sqrt = std::sqrt(det);
		
		const auto start = std::max(1.0,std::ceil((times[i] - det_sqrt)/2));
		const auto end = std::min(times[i]+0.0,std::ceil((times[i] + det_sqrt)/2));

		product*=(end-start);
	}
	std::cout<<product;
	
	return 0;
}
