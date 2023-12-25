#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <limits>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <cstddef>

struct node
{
	std::vector<std::size_t> connected_to;
};

auto stoer_wagner_cut(const std::vector<node>& nodes)
{
	struct cut_info
	{
		std::size_t group_size, cost;
		std::size_t node0, node1;
	};

	std::vector<std::vector<std::size_t>> connection_weights(nodes.size(),std::vector<std::size_t>(nodes.size(),0));
	for(std::size_t n=0;n<nodes.size();++n)
		for(const auto& c: nodes[n].connected_to)
			connection_weights[n][c] = connection_weights[c][n] = 1;

	std::vector<std::size_t> contracted_into(nodes.size(),1);
	
	const auto merge = [&](std::size_t node0_id, std::size_t node1_id)
	{
		for(std::size_t i=0;i<nodes.size();++i)
			connection_weights[node0_id][i] = (connection_weights[i][node0_id]+=connection_weights[node1_id][i]);

		for(std::size_t i=0;i<nodes.size();++i)
			connection_weights[node1_id][i] = connection_weights[i][node1_id] = 0;
		
		contracted_into[node0_id]+=contracted_into[node1_id];
		contracted_into[node1_id] = 0;
	};
	
	const auto min_cut_attempt = [&](std::size_t start_with)
	{
		const auto remaining_nodes = nodes.size() - start_with;
		
		std::vector<std::size_t> connections = connection_weights[0];
		std::vector<bool> selected(nodes.size(),false);
		selected[0] = true;
		std::size_t last_selected_node = 0;

		for(std::size_t selected_nodes=1; selected_nodes+1<remaining_nodes; ++selected_nodes)
		{
			const auto max_connected_id = std::ranges::max_element(connections) - std::begin(connections);
			for(std::size_t i=0;i<nodes.size();++i)
			{
				if(!selected[i])
					connections[i]+=connection_weights[max_connected_id][i];
			}
			last_selected_node = max_connected_id;
			connections[last_selected_node] = 0;
			selected[last_selected_node] = true;
		}

		const std::size_t remaining_node = std::ranges::max_element(connections) - std::begin(connections);
		const auto cost = std::accumulate(connections.begin(),connections.end(),std::size_t{0});

		return cut_info{contracted_into[remaining_node],cost,last_selected_node,remaining_node};
	};

	std::size_t best_groupsize = 0, lowest_cost = std::numeric_limits<std::size_t>::max();
	for(std::size_t i=0;i<nodes.size();++i)
	{
		const auto& cut = min_cut_attempt(i);
		if(cut.cost<lowest_cost)
		{
			best_groupsize = cut.group_size;
			lowest_cost = cut.cost;
		}
		merge(cut.node0, cut.node1);
	}

	struct result_type
	{
		std::size_t group_size, cost;
	};
	
	return result_type{best_groupsize, lowest_cost};
}

int main(int argc, char* argv[])
{
	std::vector<node> nodes;
	std::unordered_map<std::string,std::size_t> node_ids;

	const auto node_id_for = [&](const std::string& name)
	{
		if(!node_ids.contains(name))
		{
			node_ids[name] = nodes.size();
			nodes.push_back({});
		}
		return node_ids[name];
	};

	const auto add_connection = [&](const std::string& node0, const std::string& node1)
	{
		const auto id0 = node_id_for(node0); 
		const auto id1 = node_id_for(node1); 
		nodes[id0].connected_to.push_back(id1);
		nodes[id1].connected_to.push_back(id0);
	};

	for(std::string line; std::getline(std::cin,line);)
	{
		std::stringstream strm{line};
		std::string name;
		std::getline(strm,name,':');
		for(std::string connected_to; strm>>connected_to;)
			add_connection(name,connected_to);
	}

	const auto cut = stoer_wagner_cut(nodes);
	std::cout<<cut.group_size * (nodes.size()-cut.group_size);
	
	return 0;
}
