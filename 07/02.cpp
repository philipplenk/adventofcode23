#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <tuple>
#include <vector>

#include <cctype>
#include <cstddef>

enum class card_type
{
	joker,
	numbered_2,
	numbered_3,
	numbered_4,
	numbered_5,
	numbered_6,
	numbered_7,
	numbered_8,
	numbered_9,
	numbered_10,
	queen,
	king,
	ace
};

std::istream& operator>>(std::istream& in, card_type& t)
{
	char c;
	if(!(in>>c))
	   return in;
	
	if(std::isdigit(c))
		t = static_cast<card_type>(c-'2'+1);
	else
	{
		switch(c)
		{
			case 'T': t = card_type::numbered_10; break;
			case 'J': t = card_type::joker; break;
			case 'Q': t = card_type::queen; break;
			case 'K': t = card_type::king; break;
			case 'A': t = card_type::ace; break;
		}
	}

	return in;
}

constexpr auto number_of_cards_in_hand = 5;

enum class hand_type
{
	high_card,
	one_pair,
	two_pairs,
	three_of_a_kind,
	full_house,
	four_of_a_kind,
	five_of_a_kind
};

hand_type determine_type(const std::array<card_type,number_of_cards_in_hand >& cards)
{
	constexpr auto number_of_cards = static_cast<int>(card_type::ace)+1;

	std::array<int,number_of_cards> counts{};
	for(auto c: cards)
		++counts[static_cast<int>(c)];

	std::array<int,number_of_cards_in_hand+1> frequency{};
	for(std::size_t i=1;i<counts.size();++i)
		++frequency[counts[i]];

	const auto number_of_jokers = counts[0];

	if(frequency[5-number_of_jokers]>0) return hand_type::five_of_a_kind;
	if(frequency[4-number_of_jokers]>0) return hand_type::four_of_a_kind;
	
	if(frequency[3]==1 && frequency[2]==1) return hand_type::full_house;
	if(frequency[2]==2 && number_of_jokers==1) return hand_type::full_house;

	if(frequency[3-number_of_jokers]>0) return hand_type::three_of_a_kind;
	
	if(frequency[2]>1) return hand_type::two_pairs;
	if(frequency[2-number_of_jokers]>0) return hand_type::one_pair;

	return hand_type::high_card;
}

struct bidding_hand
{
	hand_type type;
	std::array<card_type,number_of_cards_in_hand> cards;
	int bid;
};

std::istream& operator>>(std::istream& in, bidding_hand& hand)
{
	for(auto& c: hand.cards)
		in>>c;

	if(in>>hand.bid)
		hand.type = determine_type(hand.cards);

	return in;
}

int main(int argc, char* argv[])
{
	std::vector<bidding_hand> hands{std::istream_iterator<bidding_hand>{std::cin},{}};

	std::sort(hands.begin(),hands.end(),[&](const auto& lhs, const auto& rhs)
	{
		return std::tie(lhs.type,lhs.cards)<std::tie(rhs.type,rhs.cards);
	});

	int winnings = 0;
	for(int i=1;const auto& hand: hands)
	{
		winnings+=i*hand.bid;
		++i;
	}
	std::cout<<winnings;

	return 0;
}
