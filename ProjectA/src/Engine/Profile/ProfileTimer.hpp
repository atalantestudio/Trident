#pragma once

#include "Engine/Profile/ProfileEvent.hpp"

namespace ProjectA {
	class ProfileTimer {
		public:
			explicit ProfileTimer(const std::string& name);

			~ProfileTimer();

		private:
			ProfileEvent event;
	};
}