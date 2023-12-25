#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <cctype>
#include <cstddef>

enum class action_type
{
	accept, reject, delegate
};

struct action
{
	action_type type;
	std::string delegate_to;
};

std::istream& operator>>(std::istream& in, action& a)
{
	if(std::islower(in.peek()))
	{
		a.type = action_type::delegate;
		a.delegate_to.clear();
		for(char c; std::islower(in.peek()) && in>>c; )
			a.delegate_to+=c;
		
		return in;
	}
	
	char type;
	in>>type;
	a.type = type=='R'?action_type::reject:action_type::accept;

	return in;
}

enum class condition_type
{
	less, greater, none
};

struct condition
{
	condition_type type;
	char variable;
	int value;
};

std::istream& operator>>(std::istream& in, std::optional<condition>& c)
{
	condition result;
	in>>result.variable;
	
	if(std::isupper(result.variable) || std::islower(in.peek()))
	{
		in.putback(result.variable);
		c = std::nullopt;
		return in;
	}

	char operation, skip;
	in>>operation>>result.value>>skip;
	result.type = operation=='<'?condition_type::less:condition_type::greater;
	c = result;

	return in;
}

struct rule
{
	std::optional<condition> cond;
	action result;
};

std::istream& operator>>(std::istream& in, rule& r)
{
	return in>>r.cond>>r.result;
}

struct workflow
{
	std::string name;
	std::vector<rule> rules;
};

std::istream& operator>>(std::istream& in, workflow& w)
{
	w = {};
	for(char c; in.peek()!='{' && in>>c;)
		w.name+=c;

	char skip;
	for(rule c; in>>skip && skip!='}' && in>>c;)
		w.rules.push_back(c);
	
	return in;
}

struct range
{
	int min = 1, max = 4000;
};

using workflow_map = std::unordered_map<std::string,workflow>;
using variable_ranges = std::unordered_map<char,range>;

std::size_t combinations_in(const variable_ranges& ranges)
{
	long long result = 1;
	for(const auto& [_,r]: ranges)
		result*=std::max(0,r.max+1-r.min);
	
	return result;
}

auto split(const condition& cond, const variable_ranges& ranges)
{
	struct result_type
	{
		variable_ranges if_true, if_false;
	};

	result_type result{ranges,ranges};
	if(cond.type==condition_type::less)
	{
		result.if_true[cond.variable].max = std::min(result.if_true[cond.variable].max,cond.value-1);
		result.if_false[cond.variable].min = std::max(result.if_false[cond.variable].min,cond.value);
	}
	else
	{
		result.if_true[cond.variable].min = std::max(result.if_true[cond.variable].min,cond.value+1);
		result.if_false[cond.variable].max = std::min(result.if_false[cond.variable].max,cond.value);
	}

	return result;
}

std::size_t count_accepting_ranges(const workflow_map& workflows, const workflow& workflow, const variable_ranges& ranges, std::size_t rule_idx)
{
	if(rule_idx>workflow.rules.size())
		return {};
	
	const auto& current_rule = workflow.rules[rule_idx];

	const auto count_remaining = [&](const auto& remaining_ranges)
	{
		if(current_rule.result.type==action_type::delegate)
		{
			const auto& next_workflow = workflows.find(current_rule.result.delegate_to)->second;
			return count_accepting_ranges(workflows, next_workflow, remaining_ranges, 0); 
		}
		return current_rule.result.type==action_type::accept?combinations_in(remaining_ranges):0;
	};

	if(!current_rule.cond)
		return count_remaining(ranges);

	const auto remaining = split(*current_rule.cond,ranges);
	const auto if_false = count_accepting_ranges(workflows, workflow,remaining.if_false, rule_idx+1);

	return if_false+count_remaining(remaining.if_true);
}


int main(int argc, char* argv[])
{
	workflow_map workflows;
	for(workflow w; std::cin.peek()!='{' && std::cin>>w>>std::ws;)
		workflows[w.name] = w;

	variable_ranges default_ranges;
	for(auto c: {'x','m','a','s'})
		default_ranges[c] = {1,4000};

	std::cout<<count_accepting_ranges(workflows,workflows["in"],default_ranges,0);

	return 0;
}
