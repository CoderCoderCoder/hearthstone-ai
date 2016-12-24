#pragma once

#include "State/Types.h"
#include "State/State.h"
#include "State/Cards/Card.h"
#include "State/Cards/Manager.h"
#include "FlowControl/Manipulators/CardManipulator.h"
#include "FlowControl/Manipulators/Helpers/DamageHelper.h"
#include "FlowControl/Manipulators/Helpers/HealHelper.h"

namespace FlowControl
{
	namespace Helpers { class DamageDealer; }

	namespace Manipulators
	{
		class CharacterManipulator : public CardManipulator
		{
		public:
			CharacterManipulator(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
				: CardManipulator(state, flow_context, card_ref, card)
			{
				assert(card.GetCardType() == state::kCardTypeMinion ||
					card.GetCardType() == state::kCardTypeHero);
			}

			void Taunt(bool v) { card_.SetTaunt(v); }
			void Shield(bool v) { card_.SetShield(v); }
			void Charge(bool v) { card_.SetCharge(v); }

			Helpers::DamageHelper Damage(int amount) { return Helpers::DamageHelper(state_, flow_context_, card_ref_, card_, amount); }
			Helpers::HealHelper Heal(int amount) { return Helpers::HealHelper(state_, flow_context_, card_ref_, card_, amount); }

			detail::DamageSetter Internal_SetDamage() { return detail::DamageSetter(card_); }

			void AfterAttack()
			{
				card_.IncreaseNumAttacksThisTurn();
			}
		};
	}
}