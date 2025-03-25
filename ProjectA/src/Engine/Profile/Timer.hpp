#pragma once

#include "Engine/Profile/ProfileEvent.hpp"

namespace ProjectA {
	class Timer {
		public:
			Timer(const std::string& name);

			~Timer();

		private:
			ProfileEvent event;
	};
}