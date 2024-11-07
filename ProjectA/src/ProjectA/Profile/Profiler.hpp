#pragma once

// TODO: Profile-only PCH header?
#include <fstream>

#include "ProjectA/Profile/ProfileEvent.hpp"

namespace ProjectA {
	class Profiler {
		private:
			static std::ofstream stream;
			static std::size_t eventCount;

		public:
			static void writeEvent(const ProfileEvent& profile);

		public:
			Profiler(const std::string& path);

			~Profiler();
	};
}