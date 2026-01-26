// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Descriptor/FenceDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	class Fence {
		public:
			virtual ~Fence() = default;

		protected:
			explicit Fence(const FenceDescriptor& descriptor) {
				DEBUG_DESCRIPTOR_GUARD(descriptor);
			}
	};
}