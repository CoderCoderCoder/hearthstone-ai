#pragma once

#include <assert.h>
#include "state/Cards/Card.h"
#include "FlowControl/Manipulators/CardManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class WeaponManipulator : public CardManipulator
		{
		public:
			WeaponManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card &card)
				: CardManipulator(state, flow_context, card_ref, card)
			{
				assert(card.GetCardType() == state::kCardTypeWeapon);
			}
		};
	}
}