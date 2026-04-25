// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/MemoryAllocation.hpp"

namespace Trident {
	class MemoryAllocator {
		private:
			static constexpr uint64 DYNAMIC_MEMORY_ALLOCATION_SIZE = 16000000;

		private:
			std::vector<MemoryAllocation> allocations;
	};
}