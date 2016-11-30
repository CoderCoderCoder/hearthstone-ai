#pragma once

#include <stdexcept>
#include <sstream>

#include "weapon.h"
#include "hero-power.h"
#include "object-base.h"
#include "game-engine/enchantments/enchantments.h"

namespace GameEngine {

class Hero;

// POD
class HeroData
{
	friend std::hash<HeroData>;
	friend class Hero;

public:
	HeroData() : attacked_times(0), freezed(false) {}

	bool operator==(HeroData const& rhs) const;
	bool operator!=(HeroData const& rhs) const;

public:
	std::string GetDebugString() const;

public:
	int hp;
	int max_hp;
	int armor;
	int attack;

	Weapon weapon;
	HeroPower hero_power;

	int attacked_times;
	bool freezed;
};

} // GameEngine

inline bool GameEngine::HeroData::operator==(HeroData const & rhs) const
{
	if (this->hp != rhs.hp) return false;
	if (this->max_hp != rhs.max_hp) return false;
	if (this->armor != rhs.armor) return false;
	if (this->attack != rhs.attack) return false;
	if (this->weapon != rhs.weapon) return false;
	if (this->hero_power != rhs.hero_power) return false;
	if (this->attacked_times != rhs.attacked_times) return false;
	if (this->freezed != rhs.freezed) return false;

	return true;
}

inline bool GameEngine::HeroData::operator!=(HeroData const & rhs) const
{
	return !(*this == rhs);
}

inline std::string GameEngine::HeroData::GetDebugString() const
{
	std::ostringstream oss;
	oss << "HP: " << this->hp << " + " << this->armor << " / " << this->max_hp;
	if (this->freezed) oss << " [FREEZED]";
	oss << std::endl;
	if (this->attack > 0) oss << "Attack: " << this->attack << std::endl;

	if (this->weapon.IsValid()) {
		oss << "Weapon: [" << this->weapon.card_id << "] " << this->weapon.attack << " " << this->weapon.durability;
		if (this->weapon.freeze_attack) oss << " [FREEZE]";
		if (this->weapon.forgetful > 0) oss << " [FORGETFUL:" << this->weapon.forgetful << "]";

		oss << std::endl;
	}

	return oss.str();
}

namespace std {
	template <> struct hash<GameEngine::HeroData> {
		typedef GameEngine::HeroData argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.hp);
			GameEngine::hash_combine(result, s.max_hp);
			GameEngine::hash_combine(result, s.armor);
			GameEngine::hash_combine(result, s.attack);
			GameEngine::hash_combine(result, s.weapon);
			GameEngine::hash_combine(result, s.hero_power);
			GameEngine::hash_combine(result, s.attacked_times);
			GameEngine::hash_combine(result, s.freezed);

			return result;
		}
	};
}