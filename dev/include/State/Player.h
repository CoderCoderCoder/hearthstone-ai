#pragma once

#include "State/Deck.h"
#include "State/Graveyard.h"
#include "State/Hand.h"
#include "State/Minions.h"
#include "State/Secrets.h"
#include "State/Weapon.h"
#include "State/PlayerResource.h"
#include "State/PlayerState.h"

namespace State
{
	class Player
	{
	public:
		Player() : fatigue_damage_(0) {}

		Deck deck_;
		Hand hand_;
		Minions minions_;
		Weapon weapon_;
		Secrets secrets_;
		Graveyard graveyard_;

		PlayerResource resource_;
		PlayerState state_;

		int fatigue_damage_;
	};
}