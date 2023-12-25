#include <iostream>
#include <numeric>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
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

/*
	This solution has to make a bunch of assumptions which happend to hold true for my input.
	As I have no clue if they are universal or if the stars just fortuitously aligned
	in my favour - however unlikely that may be - I opted to at least verify a few of them.
	
	- First of all, I assume rx(the output node) is fed by a single conjunction-node.
	- Further, I assume all subgraphs producing an input for this conjuction-node are disjunct, i.e. independent.
	- Finally, I assume that if all of those inputs produce a high pulse in the same button-press-cycle, they do so
	  without producing a low pulse in between and they do so again after the same number of cycles.

	The first two are confirmed below.	
	I could not think of an easy way to verify the final ones, so instead of doing so, I just silently assume them
	and will simply break down and cry after receiving a wrong result ;_;

	If, however, all of this holds, I can find a solution by determining the cycle length to a high pulse for each
	input independently. The least common multiple of those cycle lengths will then be the first iteration where all
	of them produce a high pulse at the same time, yielding a low pulse on the output node :)
*/
bool assumptions_hold(const std::unordered_map<std::string, module>& modules)
{
	if(modules.contains("rx") && modules.find("rx")->second.incoming.size()>1)
	{
		std::cerr<<"rx is connected to more than one input\n";
		return false;
	}
	
	const auto final_module = modules.find("rx")->second.incoming.front().connected_to;

	if(modules.contains(final_module) && modules.find(final_module)->second.type!=module_type::conjunction)
	{
		std::cerr<<"The final module is not a conjunction!\n";
		return false;
	}

	std::unordered_set<std::string> seen;
	for(const auto& con: modules.find(final_module)->second.incoming)
	{
		std::unordered_set<std::string> to_visit, seen_here;
		to_visit.insert(con.connected_to);
		while(!to_visit.empty())
		{
			std::unordered_set<std::string> to_visit_next;
			for(const auto& m: to_visit)
			{
				if(m=="broadcaster")
					continue;

				if(seen_here.contains(m))
					continue;
				
				if(seen.contains(m))
				{
					std::cerr<<"Inputs to final module are not disjunct/overlapping\n";
					std::cerr<<"Input "<<m<<" appeared in more than one\n";
					return false;
				}
				
				seen_here.insert(m);
				seen.insert(m);

				if(!modules.contains(m))
					continue;
				
				for(const auto& [c,_]: modules.find(m)->second.incoming)
				{
					if(!seen_here.contains(c))
						to_visit_next.insert(c);
				}
			}
			to_visit = std::move(to_visit_next);
		}
	}

	return true;
}

void propagate(const pulse& signal, std::unordered_map<std::string,module>& modules, std::deque<pulse>& to_process)
{
	auto& current_module = modules[signal.destination]; 
	switch(current_module.type)
	{
		case module_type::broadcast:
		{
			for(const auto& con: current_module.outgoing)
			{
				to_process.push_back({signal.destination,con.connected_to,signal.type});
			}	
			break;
		}
		case module_type::flip_flop:
		{
			if(signal.type==pulse_type::low)
			{
				current_module.is_on = !current_module.is_on;
				const auto output_signal = current_module.is_on?pulse_type::high:pulse_type::low;

				for(const auto& con: current_module.outgoing)
					to_process.push_back({signal.destination,con.connected_to,output_signal});
			}
			break;
		}
		case module_type::conjunction:
		{
			bool all = true;
			for(auto& con: current_module.incoming)
			{
				if(con.connected_to==signal.source)
					con.stored_pulse = signal.type;

				all&=con.stored_pulse==pulse_type::high;
			}

			const auto output_signal = all?pulse_type::low:pulse_type::high;
			for(const auto& con: current_module.outgoing)
				to_process.push_back({signal.destination,con.connected_to,output_signal});
			
			break;
		}
	}
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

	if(!assumptions_hold(modules))
	{
		std::cerr<<";_;\n";
		return 1;
	}

	const auto final_module = modules["rx"].incoming.front().connected_to;
	std::unordered_set<std::string> relevant_modules;
	for(const auto& con: modules[final_module].incoming)
		relevant_modules.insert(con.connected_to);

	bool done = false;
	std::unordered_map<std::string,long long> high_impulse_after;
	for(long long i=0;!done;++i)
	{
		std::deque<pulse> to_process;
		to_process.push_back({"button","broadcaster",pulse_type::low});

		while(!to_process.empty())
		{
			const auto next = to_process.front();
			to_process.pop_front();

			if(relevant_modules.contains(next.source) && !high_impulse_after.contains(next.source) && next.type==pulse_type::high)
			{
				high_impulse_after[next.source] = i+1;
				if(relevant_modules.size()==high_impulse_after.size())
				{
					done = true;
					break;
				}
			}

			propagate(next,modules,to_process);
		}
	}

	long long result = 1;
	for(const auto& [_,period]: high_impulse_after)
		result = std::lcm(result,period);

	std::cout<<result;

	return 0;
}
