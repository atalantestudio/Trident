// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/Descriptor/FenceDescriptor.hpp"

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