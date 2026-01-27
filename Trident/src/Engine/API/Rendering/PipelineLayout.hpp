// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Descriptor/PipelineLayoutDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class PipelineLayout {
		public:
			explicit PipelineLayout(const PipelineLayoutDescriptor& descriptor) {
				DEBUG_DESCRIPTOR_GUARD(descriptor);
			}

			virtual ~PipelineLayout() = default;
	};
}