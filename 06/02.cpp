#include <algorithm>
#include <iostream>
#include <string>

#include <cctype>
#include <cmath>

int main(int argc, char* argv[])
{
	std::string time_str, distance_str;
	std::getline(std::cin,time_str);
	std::getline(std::cin,distance_str);
	std::ranges::remove_if(time_str,[](auto c) { return !std::isdigit(c); });
	std::ranges::remove_if(distance_str,[](auto c) { return !std::isdigit(c); });

	long long product = 1;
	long long time = std::stoll(time_str);
	long long distance = std::stoll(distance_str);
	{
		const auto det = time*time - 4*distance;
		const auto det_sqrt = std::sqrt(det);
		const auto start = std::max(1.0,std::ceil((time - det_sqrt)/2));
		const auto end = std::min(time+0.0,std::ceil((time + det_sqrt)/2));

		product*=(end-start);
	}
	std::cout<<product;

	return 0;
}
