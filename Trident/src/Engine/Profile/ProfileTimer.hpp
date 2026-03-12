// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Profile/ProfileEvent.hpp"

namespace Trident {
	class ProfileTimer {
		public:
			explicit ProfileTimer(view<char8> name);

			~ProfileTimer();

		private:
			ProfileEvent event;
	};
}