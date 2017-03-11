#pragma once

#include <exception>
#include "State/Types.h"
#include "State/board/Player.h"

namespace state
{
	namespace board
	{
		class Board
		{
		public:
			Player & Get(PlayerIdentifier identifier)
			{
				if (identifier.IsFirst()) return first_;
				else return second_;
			}

			const Player & Get(PlayerIdentifier identifier) const
			{
				if (identifier.IsFirst()) return first_;
				else return second_;
			}

			Player & GetAnother(PlayerIdentifier identifier)
			{
				if (identifier.IsFirst()) return second_;
				else return first_;
			}

			const Player & GetAnother(PlayerIdentifier identifier) const
			{
				if (identifier.IsFirst()) return second_;
				else return first_;
			}

			Player & GetFirst() { return first_; }
			Player const& GetFirst() const { return first_; }

			Player & GetSecond() { return second_; }
			Player const& GetSecond() const { return second_; }

		private:
			Player first_;
			Player second_;
		};
	}
}