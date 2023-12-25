#include <iostream>
#include <iterator>
#include <numeric>
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

using workflow_map = std::unordered_map<std::string,workflow>;

struct variable_set
{
	int x, m, a, s;

	int operator[](char c) const
	{
		switch(c)
		{
			case 'x': return x;
			case 'm': return m;
			case 'a': return a;
			case 's': return s;
		}
		return 0;
	}
};

bool evaluate(const condition& cond, const variable_set& variables)
{
	if(cond.type==condition_type::less)
		return variables[cond.variable]<cond.value;

	return variables[cond.variable]>cond.value;
}

std::istream& operator>>(std::istream& in, variable_set& vars)
{
	char skip;
	return in
		>>skip>>skip>>skip>>vars.x
		>>skip>>skip>>skip>>vars.m
		>>skip>>skip>>skip>>vars.a
		>>skip>>skip>>skip>>vars.s
		>>skip;
}

action evaluate(const workflow& workflow, const variable_set& variables)
{
	for(const auto& r: workflow.rules)
	{
		if(!r.cond || evaluate(*r.cond,variables))
			return r.result;
	}
	return {action_type::reject,""};
}


int main(int argc, char* argv[])
{
	workflow_map workflows;
	for(workflow w; std::cin.peek()!='{' && std::cin>>w>>std::ws;)
		workflows[w.name] = w;

	const auto sum_if_accepted = [&](const variable_set& variables)
	{
		auto next_action = action{action_type::delegate, "in"};
		while(next_action.type==action_type::delegate)
			next_action = evaluate(workflows[next_action.delegate_to],variables);

		if(next_action.type==action_type::reject)
			return 0;

		return variables.x+variables.m+variables.a+variables.s;
	};

	std::cout<<std::transform_reduce(std::istream_iterator<variable_set>{std::cin},{},0,std::plus<>{},sum_if_accepted);

	return 0;
}
