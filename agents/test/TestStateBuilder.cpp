#include <random>

#include "TestStateBuilder.h"

#include "Cards/Database.h"
#include "state/Configs.h"
#include "decks/Decks.h"

#include "state/targetor/Targets-impl.h"
#include "state/detail/InvokeCallback-impl.h"

class MyRandomGenerator : public engine::FlowControl::IRandomGenerator
{
public:
	MyRandomGenerator(std::mt19937 & random) : random_(random) {}

	int Get(int exclusive_max)
	{
		return random_() % exclusive_max;
	}

	size_t Get(size_t exclusive_max) { return (size_t)Get((int)exclusive_max); }

	int Get(int min, int max)
	{
		return min + Get(max - min + 1);
	}

public:
	std::mt19937 & random_;
};

#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable: 4505 )
#endif
static void PushBackDeckCard(Cards::CardId id, engine::FlowControl::IRandomGenerator & random, state::State & state, state::PlayerIdentifier player)
{
	int deck_count = (int)state.GetBoard().Get(player).deck_.Size();

	state.GetBoard().Get(player).deck_.ShuffleAdd(id, [&](int exclusive_max) {
		return random.Get(exclusive_max);
	});

	++deck_count;

	assert(state.GetBoard().Get(player).deck_.Size() == deck_count);
}

static state::Cards::Card CreateHandCard(Cards::CardId id, state::State & state, state::PlayerIdentifier player)
{
	state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(id);

	raw_card.enchanted_states.player = player;
	raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);
	raw_card.zone = state::kCardZoneNewlyCreated;

	return state::Cards::Card(raw_card);
}

static state::CardRef AddHandCard(Cards::CardId id, state::State & state, state::PlayerIdentifier player)
{
	int hand_count = (int)state.GetBoard().Get(player).hand_.Size();

	auto ref = state.AddCard(CreateHandCard(id, state, player));
	state.GetZoneChanger<state::kCardZoneNewlyCreated>(ref)
		.ChangeTo<state::kCardZoneHand>(player);

	assert(state.GetCardsManager().Get(ref).GetCardId() == id);
	assert(state.GetCardsManager().Get(ref).GetPlayerIdentifier() == player);
	if (hand_count == 10) {
		assert(state.GetBoard().Get(player).hand_.Size() == 10);
		assert(state.GetCardsManager().Get(ref).GetZone() == state::kCardZoneGraveyard);
	}
	else {
		++hand_count;
		assert((int)state.GetBoard().Get(player).hand_.Size() == hand_count);
		assert(state.GetCardsManager().Get(ref).GetZone() == state::kCardZoneHand);
		if constexpr (!state::kOrderHandCardsByCardId) {
			assert(state.GetBoard().Get(player).hand_.Get(hand_count - 1) == ref);
			assert(state.GetCardsManager().Get(ref).GetZonePosition() == (hand_count - 1));
		}
	}

	return ref;
}

[[maybe_unused]]
static void MakeHand(state::State & state, state::PlayerIdentifier player)
{
	//AddHandCard(Cards::ID_CS2_141, state, player);
	//AddHandCard(Cards::ID_EX1_320, state, player);
	//AddHandCard(Cards::ID_EX1_009, state, player);
	AddHandCard(Cards::ID_NEW1_007, state, player);
	//AddHandCard(Cards::ID_CFM_940, state, player);
}

static void MakeHero(state::State & state, state::PlayerIdentifier player, Cards::CardId card_id)
{
	state::Cards::CardData raw_card;
	raw_card.card_id = card_id;
	raw_card.card_type = state::kCardTypeHero;
	raw_card.zone = state::kCardZoneNewlyCreated;
	raw_card.enchanted_states.max_hp = 30;
	raw_card.enchanted_states.player = player;
	raw_card.enchanted_states.attack = 0;
	raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);

	state::CardRef ref = state.AddCard(state::Cards::Card(raw_card));

	state.GetZoneChanger<state::kCardTypeHero, state::kCardZoneNewlyCreated>(ref)
		.ChangeTo<state::kCardZonePlay>(player);


	auto hero_power = Cards::CardDispatcher::CreateInstance(Cards::ID_CS2_034);
	assert(hero_power.card_type == state::kCardTypeHeroPower);
	hero_power.zone = state::kCardZoneNewlyCreated;
	ref = state.AddCard(state::Cards::Card(hero_power));
	state.GetZoneChanger<state::kCardTypeHeroPower, state::kCardZoneNewlyCreated>(ref)
		.ChangeTo<state::kCardZonePlay>(player);
}
#ifdef _MSC_VER
#pragma warning( pop )
#endif

void PrepareDeck(std::unordered_multiset<std::string> const& cards, engine::FlowControl::IRandomGenerator & random, state::State & state, state::PlayerIdentifier player)
{
	for (auto const& card_name : cards) {
		Cards::CardId card_id = (Cards::CardId)Cards::Database::GetInstance().GetIdByCardName(card_name);
		PushBackDeckCard(card_id, random, state, player);
	}
}

void MoveFromDeckToHand(std::unordered_multiset<std::string> & cards, std::string const& card_name, state::State & state, state::PlayerIdentifier player)
{
	auto it = cards.find(card_name);
	if (it == cards.end()) throw std::runtime_error("card not exists in deck");
	cards.erase(it);
	Cards::CardId card_id = (Cards::CardId)Cards::Database::GetInstance().GetIdByCardName(card_name);
	AddHandCard(card_id, state, player);
}

void RandomlyMoveFromDeckToHand(
	std::mt19937 & rand,
	std::unordered_multiset<std::string> & cards, state::State & state, state::PlayerIdentifier player)
{
	size_t idx = rand() % cards.size();
	auto it = std::next(cards.begin(), idx);
	std::string card_name = *it;

	cards.erase(it);
	Cards::CardId card_id = (Cards::CardId)Cards::Database::GetInstance().GetIdByCardName(card_name);
	AddHandCard(card_id, state, player);
}

state::State TestStateBuilder::GetStateWithRandomStartCard(int start_card_seed, std::mt19937 & random)
{
	std::mt19937 start_card_rand(start_card_seed);
	state::State state;
	MyRandomGenerator my_random(random);

	MakeHero(state, state::PlayerIdentifier::First(), Cards::ID_HERO_07);
	auto deck1 = decks::Decks::GetDeck("InnKeeperExpertWarlock");
	RandomlyMoveFromDeckToHand(start_card_rand, deck1, state, state::PlayerIdentifier::First());
	RandomlyMoveFromDeckToHand(start_card_rand, deck1, state, state::PlayerIdentifier::First());
	RandomlyMoveFromDeckToHand(start_card_rand, deck1, state, state::PlayerIdentifier::First());
	PrepareDeck(deck1, my_random, state, state::PlayerIdentifier::First());

	MakeHero(state, state::PlayerIdentifier::Second(), Cards::ID_HERO_07);
	auto deck2 = decks::Decks::GetDeck("InnKeeperExpertWarlock");
	RandomlyMoveFromDeckToHand(start_card_rand, deck2, state, state::PlayerIdentifier::Second());
	RandomlyMoveFromDeckToHand(start_card_rand, deck2, state, state::PlayerIdentifier::Second());
	RandomlyMoveFromDeckToHand(start_card_rand, deck2, state, state::PlayerIdentifier::Second());
	RandomlyMoveFromDeckToHand(start_card_rand, deck2, state, state::PlayerIdentifier::Second());
	AddHandCard(Cards::ID_GAME_005, state, state::PlayerIdentifier::Second());
	PrepareDeck(deck2, my_random, state, state::PlayerIdentifier::Second());

	state.GetMutableCurrentPlayerId().SetFirst();
	state.GetBoard().GetFirst().GetResource().SetTotal(1);
	state.GetBoard().GetFirst().GetResource().Refill();
	state.GetBoard().GetSecond().GetResource().SetTotal(0);
	state.GetBoard().GetSecond().GetResource().Refill();

	return state;
}

state::State TestStateBuilder::GetState(std::mt19937 & random)
{
	state::State state;
	MyRandomGenerator my_random(random);

	MakeHero(state, state::PlayerIdentifier::First(), Cards::ID_HERO_08);
	auto deck1 = decks::Decks::GetDeck("InnKeeperBasicMage");
	MoveFromDeckToHand(deck1, "Arcane Missiles", state, state::PlayerIdentifier::First());
	//MoveFromDeckToHand(deck1, "Murloc Raider", state, state::PlayerIdentifier::First());
	MoveFromDeckToHand(deck1, "Bloodfen Raptor", state, state::PlayerIdentifier::First());
	MoveFromDeckToHand(deck1, "Wolfrider", state, state::PlayerIdentifier::First());
	AddHandCard(Cards::ID_GAME_005, state, state::PlayerIdentifier::First());
	//AddHandCard(Cards::ID_NEW1_007, state, state::PlayerIdentifier::First());
	PrepareDeck(deck1, my_random, state, state::PlayerIdentifier::First());

	MakeHero(state, state::PlayerIdentifier::Second(), Cards::ID_HERO_08);
	auto deck2 = decks::Decks::GetDeck("InnKeeperBasicMage");
	MoveFromDeckToHand(deck2, "Arcane Missiles", state, state::PlayerIdentifier::Second());
	MoveFromDeckToHand(deck2, "Bloodfen Raptor", state, state::PlayerIdentifier::Second());
	MoveFromDeckToHand(deck2, "River Crocolisk", state, state::PlayerIdentifier::Second());
	MoveFromDeckToHand(deck2, "Arcane Explosion", state, state::PlayerIdentifier::Second());
	PrepareDeck(deck2, my_random, state, state::PlayerIdentifier::Second());

	state.GetMutableCurrentPlayerId().SetFirst();
	state.GetBoard().GetFirst().GetResource().SetTotal(1);
	state.GetBoard().GetFirst().GetResource().Refill();
	state.GetBoard().GetSecond().GetResource().SetTotal(0);
	state.GetBoard().GetSecond().GetResource().Refill();

	return state;
}
