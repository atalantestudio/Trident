// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Descriptor/FenceDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class Fence {
		public:
			~Fence();

		protected:
			explicit Fence(const Device* device);

		protected:
			const Device* device;
	};
}