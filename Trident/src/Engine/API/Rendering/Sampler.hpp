// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Descriptor/SamplerDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class Sampler {
		public:
			virtual ~Sampler() = default;

		protected:
			explicit Sampler(const SamplerDescriptor& descriptor) {
				DEBUG_DESCRIPTOR_GUARD(descriptor);
			}
	};
}