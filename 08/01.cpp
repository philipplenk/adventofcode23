#include <iostream>
#include <string>
#include <unordered_map>

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

	std::unordered_map<std::string,node> nodes;
	for(node n; std::cin>>n;)
		nodes[n.name] = n;

	int steps = 0;
	for(std::string current = "AAA"; current!="ZZZ";)
	{
		for(auto d: directions)
		{
			current = d=='L'?nodes[current].left:nodes[current].right;
			++steps;
			if(current=="ZZZ") break;
		}
	}
	std::cout<<steps;

	return 0;
}
