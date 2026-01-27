// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Profile/ProfileEvent.hpp"

namespace Trident {
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