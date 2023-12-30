#ifndef HASHLIFE_H
#define HASHLIFE_H

#include "point2d.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <iostream>
#include <numeric>
#include <span>
#include <unordered_map>
#include <vector>

#include <cmath>

namespace hashlife
{
	struct node
	{
		std::size_t level;
		std::array<std::size_t, 4> children_ids;
		std::size_t living_cells{0};

		friend auto operator<=>(const node&, const node&) = default;
	};
}

namespace std
{
	template <>
	struct hash<hashlife::node>
	{
		auto operator()(const hashlife::node& n) const
		{
			std::size_t result = n.level;
			for(auto c: n.children_ids)
				result = std::rotl(result,1)^c;
			return result;
		}
	};
}

namespace hashlife
{
	template <typename T>
	concept automaton_ruleset = requires(const T const_rules, std::span<std::size_t,16> neighborhood)
	{
		{ const_rules.is_alive(std::size_t{}) } -> std::convertible_to<bool>;
		{ const_rules.normalize(point2d{}) } -> std::convertible_to<point2d>;
		{ const_rules.background_square_at(point2d{}) } -> std::convertible_to<std::array<std::size_t, 4>>;
		{ const_rules.evolved_center(neighborhood) } -> std::convertible_to<std::array<std::size_t,4>>;
	};

	template <automaton_ruleset automaton_rules, std::size_t max_levels = 64>
	class tree
	{
		public:
		tree(automaton_rules rules, const point2d& center_quad_position):
			rules_{rules},
			top_left_{center_quad_position},
			root_id_{empty_at(1,top_left_)}
		{}

		void set(point2d position, std::size_t value)
		{
			const auto contains = [&]()
			{
				const auto shift = 1<<(nodes_[root_id_].level);
				const auto bottom_right = point2d{top_left_.x+shift,top_left_.y+shift};
				return position.x>=top_left_.x && position.x<bottom_right.x && position.y>=top_left_.y && position.y<bottom_right.y;
			};

			while(!contains())
				expand();

			position.x-=top_left_.x;
			position.y-=top_left_.y;
			root_id_ = replace_in(nodes_[root_id_], position, value);
		}

		void step(std::size_t steps)
		{
			for(std::size_t i=0;i<std::log2(steps)+1;++i)
				expand();

			while(steps>0)
			{
				std::cerr<<"Remaining steps: "<<steps<<'\n';
				std::size_t hyper_power = std::log2(steps);
				std::size_t hyper_level = hyper_power+2;

				const auto old_start = top_left_;
				expand();
				root_id_ = evolve(root_id_,hyper_level);
				top_left_ = old_start;
				steps-=(1<<hyper_power);
			}
		}

		auto number_of_living_cells() const
		{
			return nodes_[root_id_].living_cells;
		}

		private:
		std::vector<node> nodes_;
		std::unordered_map<node,std::size_t> node_to_id_;
		
		std::vector<std::unordered_map<point2d,std::size_t>> empty_at_cache_{max_levels};
		std::vector<std::unordered_map<std::size_t,std::size_t>> evolved_child_cache_{max_levels};

		automaton_rules rules_;	
		point2d top_left_;
		std::size_t root_id_;

		std::size_t replace_in(node n, point2d to_replace, std::size_t new_value)
		{
			if(n.level==1)
			{
				n.children_ids[to_replace.y*2+to_replace.x] = new_value;
				return find_or_create(n);
			}

			const auto inner_shift = 1<<(n.level-1);
			const auto replace_in_x = to_replace.x/inner_shift;
			const auto replace_in_y = to_replace.y/inner_shift;
			const auto replace_in_id = replace_in_y*2+replace_in_x;
			const auto offset_x = replace_in_x*inner_shift;
			const auto offset_y = replace_in_y*inner_shift;
			const auto relative_position = point2d{to_replace.x-offset_x,to_replace.y-offset_y};

			auto modified_children = n.children_ids;
			const auto node_to_modify = nodes_[modified_children[replace_in_id]];
			modified_children[replace_in_id] = replace_in(node_to_modify,relative_position,new_value);

			return find_or_create({n.level,modified_children}); 
		}

		void expand()
		{
			const auto root_node = nodes_[root_id_];
			const auto shift = 1<<(root_node.level-1);
			top_left_.x-=shift;
			top_left_.y-=shift;

			root_id_ = replace_center(empty_at(root_node.level+1,top_left_),root_id_);
		}

		std::size_t find_or_create(node new_node)
		{
			const auto is_alive = [&](auto v) { return rules_.is_alive(v); };
			const auto living_in = [&](auto id) { return nodes_[id].living_cells; };
			const auto& children = new_node.children_ids;

			new_node.living_cells = new_node.level==1?
				std::ranges::count_if(children,is_alive):
				std::transform_reduce(children.begin(),children.end(),0ull,std::plus<>{},living_in);

			if(!node_to_id_.contains(new_node))
			{
				node_to_id_[new_node] = nodes_.size();
				nodes_.push_back(new_node);
			}
			return node_to_id_[new_node];
		}

		std::size_t empty_at(std::size_t level, point2d top_left)
		{
			top_left = rules_.normalize(top_left);

			if(empty_at_cache_[level].contains(top_left))
				return empty_at_cache_[level][top_left];

			if(level==1)
			{
				const auto bg = rules_.background_square_at(top_left);
				return empty_at_cache_[level][top_left] = find_or_create({level,bg});
			}

			const auto stride = 1<<(level-1);
			const auto tl = empty_at(level-1,top_left);
			const auto tr = empty_at(level-1,{top_left.x+stride,top_left.y});
			const auto bl = empty_at(level-1,{top_left.x,top_left.y+stride});
			const auto br = empty_at(level-1,{top_left.x+stride,top_left.y+stride});

			return empty_at_cache_[level][top_left] = find_or_create({level,{tl,tr,bl,br}});
		}

		std::size_t evolve(std::size_t node_id, std::size_t hyper_level)
		{
			if(evolved_child_cache_[hyper_level].contains(node_id))
				return evolved_child_cache_[hyper_level][node_id];

			const auto parent_node = nodes_[node_id];
			if(parent_node.living_cells==0) return center(node_id);

			if(parent_node.level==2)
			{
				const auto neighborhood = collect_grandchildren(node_id); 
				const auto result = find_or_create({1,rules_.evolved_center(neighborhood)});
				return evolved_child_cache_[hyper_level][node_id] = result;
			}

			const auto n00 = evolve(parent_node.children_ids[0],hyper_level);
			const auto n01 = evolve(find_or_create({parent_node.level-1,
			{
				nodes_[parent_node.children_ids[0]].children_ids[1],
				nodes_[parent_node.children_ids[1]].children_ids[0],
				nodes_[parent_node.children_ids[0]].children_ids[3],
				nodes_[parent_node.children_ids[1]].children_ids[2]
			}}),hyper_level);
			const auto n02 = evolve(parent_node.children_ids[1],hyper_level);
			
			const auto n10 = evolve(find_or_create({parent_node.level-1,
			{
				nodes_[parent_node.children_ids[0]].children_ids[2],
				nodes_[parent_node.children_ids[0]].children_ids[3],
				nodes_[parent_node.children_ids[2]].children_ids[0],
				nodes_[parent_node.children_ids[2]].children_ids[1]
			}}),hyper_level);
			const auto n11 = evolve(center(node_id),hyper_level);
			const auto n12 = evolve(find_or_create({parent_node.level-1,
			{
				nodes_[parent_node.children_ids[1]].children_ids[2],
				nodes_[parent_node.children_ids[1]].children_ids[3],
				nodes_[parent_node.children_ids[3]].children_ids[0],
				nodes_[parent_node.children_ids[3]].children_ids[1]
			}}),hyper_level);
			
			const auto n20 = evolve(parent_node.children_ids[2],hyper_level);
			const auto n21 = evolve(find_or_create({parent_node.level-1,
			{
				nodes_[parent_node.children_ids[2]].children_ids[1],
				nodes_[parent_node.children_ids[3]].children_ids[0],
				nodes_[parent_node.children_ids[2]].children_ids[3],
				nodes_[parent_node.children_ids[3]].children_ids[2]
			}}),hyper_level);
			const auto n22 = evolve(parent_node.children_ids[3],hyper_level);

			std::array<std::size_t,4> parts
			{
				find_or_create({parent_node.level-1,{n00,n01,n10,n11}}),
				find_or_create({parent_node.level-1,{n01,n02,n11,n12}}), 
				find_or_create({parent_node.level-1,{n10,n11,n20,n21}}), 
				find_or_create({parent_node.level-1,{n11,n12,n21,n22}}), 
			};

			if(hyper_level>=parent_node.level)
			{
				for(auto& p: parts)
					p = evolve(p,hyper_level);

				const auto result = find_or_create({nodes_[parts[0]].level+1,parts});
				return evolved_child_cache_[hyper_level][node_id] = result;
			}
			
			return evolved_child_cache_[hyper_level][node_id] = find_or_create({parent_node.level-1,
			{
				center(parts[0]),
				center(parts[1]),
				center(parts[2]),
				center(parts[3])
			}});

		}

		std::array<std::size_t, 16> collect_grandchildren(std::size_t node_id) const
		{
			const auto grandparent = nodes_[node_id];
			std::array<std::size_t,16> result;
			for(std::size_t y=0;y<4;++y)
			{
				for(std::size_t x=0;x<4;++x)
				{
					const auto child_y = y/2;
					const auto child_x = x/2;
					const auto child_idx = child_y*2+child_x;
					const auto& child = nodes_[grandparent.children_ids[child_idx]];

					const auto grandchild_y = y%2;
					const auto grandchild_x = x%2;
					const auto grandchild_idx = grandchild_y*2 + grandchild_x;
					result[y*4+x] = child.children_ids[grandchild_idx];
				}
			}

			return result;
		}

		std::size_t replace_center(std::size_t node_id, std::size_t center_id)
		{
			const auto original = nodes_[node_id];
			const auto center = nodes_[center_id];
			
			std::array<std::size_t,4> new_children;
			for(std::size_t i=0;i<new_children.size();++i)
			{
				auto modified_children = nodes_[original.children_ids[i]].children_ids;
				modified_children[3-i] = center.children_ids[i];
				
				new_children[i] = find_or_create({center.level,modified_children});
			}

			return find_or_create({original.level, new_children});
		}

		std::size_t center(std::size_t node_id)
		{
			const auto parent = nodes_[node_id];
			return find_or_create({parent.level-1,
			{
				nodes_[parent.children_ids[0]].children_ids[3],
				nodes_[parent.children_ids[1]].children_ids[2],
				nodes_[parent.children_ids[2]].children_ids[1],
				nodes_[parent.children_ids[3]].children_ids[0]
			}});
		}
	};
}

#endif
