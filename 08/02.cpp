#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

struct node
{
	std::string name, left, right;
};

std::istream& operator>>(std::istream& in, node& n)
{
	char skip;
	in>>n.name>>skip>>skip>>n.left>>n.right;
	n.left.pop_back();
	n.right.pop_back();

	return in;
}

int main(int argc, char* argv[])
{
	std::string directions;
	std::cin>>directions;

	std::vector<std::string> starting_nodes;
	std::unordered_map<std::string,node> nodes;
	for(node n; std::cin>>n;)
	{
		nodes[n.name] = n;
		if(n.name.back()=='A')
			starting_nodes.push_back(n.name);
	}

	const auto path_length = [&](const auto& start)
	{
		unsigned long steps = 0;
		for(auto current_node = start;;)
		{
			for(auto d: directions)
			{
				current_node=(d=='L'?nodes[current_node].left:nodes[current_node].right);
				++steps;

				if(current_node.back()=='Z')
					return steps;
			}
		}
	};

	const auto lcm = [](auto lhs, auto rhs) { return std::lcm(lhs,rhs); };
	std::cout<<std::transform_reduce(starting_nodes.begin(),starting_nodes.end(),1ul,lcm,path_length);

	return 0;
}
