#pragma once

#include "FlowControl/Manipulators/HeroManipulator.h"
#include "FlowControl/Helpers/DamageDealer.h"

namespace FlowControl
{
	namespace Manipulators
	{
		template <typename RandomGenerator>
		void HeroManipulator::DrawCard(RandomGenerator&& random)
		{
			state::board::Player & player = state_.board.Get(player_id_);

			if (player.deck_.Empty())
			{
				int damage = ++player.fatigue_damage_;
				FlowControl::Helpers::DamageDealer(state_, flow_context_).DealDamage(player.hero_ref_, damage);
				return;
			}

			int deck_count = (int)player.deck_.Size();
			int deck_idx = 0;
			if (deck_count > 1) deck_idx = random.Get(deck_count);

			state::CardRef card_ref = player.deck_.Get(deck_idx);

			if (player.hand_.Full()) {
				Manipulate(state_, flow_context_).Card(card_ref).Zone().ChangeTo<state::kCardZoneGraveyard>(state_.current_player);
			}
			else {
				Manipulate(state_, flow_context_).Card(card_ref).Zone().ChangeTo<state::kCardZoneHand>(state_.current_player);
			}

			// TODO: trigger on-draw event (parameter: card_ref)
		}
	}
}