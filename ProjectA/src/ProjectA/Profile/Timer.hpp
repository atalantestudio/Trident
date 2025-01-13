#pragma once

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