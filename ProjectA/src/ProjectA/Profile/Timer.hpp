#pragma once

// TODO: Profile-only PCH header?
#include <chrono>

#include "ProjectA/Profile/ProfileEvent.hpp"

namespace ProjectA {
	class Timer {
		private:
			ProfileEvent event;

		public:
			Timer(const std::string& name);

			~Timer();
	};
}