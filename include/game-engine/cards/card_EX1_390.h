#pragma once

#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/stages/helper.h"
#include "game-engine/board-objects/minion-manipulator.h"
#include "game-engine/board-objects/aura-enrage.h"

namespace GameEngine {
	namespace Cards {

		class Card_EX1_390
		{
		public:
			static constexpr int card_id = CARD_ID_EX1_390;

			// Tauren Warrior

			class Aura : public GameEngine::BoardObjects::AuraEnrage
			{
			private:
				void AddEnrageEnchantment(GameEngine::BoardObjects::Minion & aura_owner)
				{
					constexpr int attack_boost = 3;
					constexpr int hp_boost = 0;

					auto enchantment = std::make_unique<BoardObjects::Enchantment_BuffMinion_C<attack_boost, hp_boost, 0, false>>();

					aura_owner.AddEnchantment(std::move(enchantment), &this->enchantments_manager);
				}

			private: // for comparison
				bool EqualsTo(GameEngine::BoardObjects::Aura const& rhs_base) const
				{
					auto rhs = dynamic_cast<decltype(this)>(&rhs_base);
					if (!rhs) return false;

					return true;
				}

				std::size_t GetHash() const
				{
					std::size_t result = std::hash<int>()(card_id);

					return result;
				}
			};

			static void AfterSummoned(GameEngine::BoardObjects::Minion & summoned_minion)
			{
				summoned_minion.AddAura(std::make_unique<Aura>());
			}
		};

	} // namespace Cards
} // namespace GameEngine
