#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include <cctype>

enum class module_type
{
	broadcast,
	flip_flop,
	conjunction
};

enum class pulse_type
{
	low, high
};

std::ostream& operator<<(std::ostream& out, const pulse_type& p)
{
	return out<<(p==pulse_type::low?"low":"high");
}

struct connection
{
	std::string connected_to;
	pulse_type stored_pulse = pulse_type::low;
};

struct module
{
	module_type type;
	std::string name;

	bool is_on = false;
	std::vector<connection> outgoing, incoming;
};

struct pulse
{
	std::string source, destination;
	pulse_type type;
};

std::istream& operator>>(std::istream& in, module& m)
{
	if(!std::isalpha(in.peek()))
	{
		char type;
		in>>type;
		m.type = type=='%'?module_type::flip_flop:module_type::conjunction;
	}
	else
		m.type = module_type::broadcast;

	in>>m.name;

	in.ignore(3);
	m.outgoing.clear();
	m.incoming.clear();
	m.is_on = false;
	do
	{
		std::string connected_to;
		in>>connected_to;
		if(connected_to.back()==',')
			connected_to.pop_back();

		m.outgoing.push_back({connected_to,pulse_type::low});
	}
	while(in && in.peek()!='\n');

	return in;
}

int main(int argc, char* argv[])
{
	std::unordered_map<std::string,module> modules;
	for(module m; std::cin>>m>>std::ws;)
		modules[m.name] = m;

	for(const auto& [source,m]: modules)
	{
		for(const auto& con: m.outgoing)
			modules[con.connected_to].incoming.push_back({source,pulse_type::low});
	}

	int low = 0, high = 0;
	for(int i=0;i<1000;++i)
	{
		std::deque<pulse> to_process;
		to_process.push_back({"button","broadcaster",pulse_type::low});

		while(!to_process.empty())
		{
			const auto next = to_process.front();
			to_process.pop_front();

			low+=next.type==pulse_type::low?1:0;
			high+=next.type==pulse_type::high?1:0;

			auto& current_module = modules[next.destination]; 
			switch(current_module.type)
			{
				case module_type::broadcast:
				{
					for(const auto& con: current_module.outgoing)
					{
						to_process.push_back({next.destination,con.connected_to,next.type});
					}
					
					break;
				}
				case module_type::flip_flop:
				{
					if(next.type==pulse_type::low)
					{
						current_module.is_on = !current_module.is_on;

						for(const auto& con: current_module.outgoing)
							to_process.push_back({next.destination,con.connected_to,current_module.is_on?pulse_type::high:pulse_type::low});
					}
					break;
				}
				case module_type::conjunction:
				{
					bool all = true;
					for(auto& con: current_module.incoming)
					{
						if(con.connected_to==next.source)
							con.stored_pulse = next.type;

						all&=con.stored_pulse==pulse_type::high;
					}

					for(const auto& con: current_module.outgoing)
						to_process.push_back({next.destination,con.connected_to,all?pulse_type::low:pulse_type::high});
					break;
				}
			}
		}
	}
	std::cout<<low*high;
	
	return 0;
}
