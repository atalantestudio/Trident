#pragma once

#include "Engine/Profile/ProfileEvent.hpp"

namespace ProjectA {
	class Profiler {
		public:
			static void writeEvent(const ProfileEvent& profile);

		public:
			explicit Profiler(view<char8> path);

			~Profiler();

		private:
			static std::ofstream stream;
			static bool firstEvent;
	};
}