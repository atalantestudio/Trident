#pragma once

namespace ProjectA {
	class Fence {
		public:
			virtual ~Fence() = default;

			// Returns true if the fence is signaled, and false otherwise.
			virtual bool isSignaled() const = 0;
	};
}