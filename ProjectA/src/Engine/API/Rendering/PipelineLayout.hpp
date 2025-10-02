#pragma once

#include "Engine/API/Rendering/Descriptor/PipelineLayoutDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	class PipelineLayout {
		public:
			explicit PipelineLayout(const PipelineLayoutDescriptor& descriptor) {
				DEBUG_DESCRIPTOR_GUARD(descriptor);
			}

			virtual ~PipelineLayout() = default;
	};
}