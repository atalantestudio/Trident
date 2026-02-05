// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	#if defined(BUILD_MODE_DEBUG)
		class ScopeTimer {
			public:
				ScopeTimer() :
					start(std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count()),
					end(0)
				{}

				~ScopeTimer() {
					end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count();

					ENGINE_DEBUG("Scope timer took []ms.", (end - start) / 1000);
				}

			private:
				uint64 start;
				uint64 end;
		};
	#endif
}