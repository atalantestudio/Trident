#pragma once

#include "Engine/Profile/ProfileEvent.hpp"

namespace ProjectA {
	class Profiler {
		public:
			static void writeEvent(const ProfileEvent& profile);

		public:
			Profiler(std::string_view path);

			~Profiler();

		private:
			static std::ofstream stream;
			static std::size_t eventCount;
	};
}