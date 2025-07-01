#pragma once

#include "Engine/API/Rendering/Descriptor/SamplerDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	class Sampler {
		public:
			virtual ~Sampler() = default;

		protected:
			explicit Sampler(const SamplerDescriptor& descriptor) {
				DEBUG_DESCRIPTOR_GUARD(descriptor);
			}
	};
}