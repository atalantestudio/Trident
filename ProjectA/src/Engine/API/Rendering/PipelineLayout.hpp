#pragma once

#include "Engine/API/Rendering/Descriptor/PipelineLayoutDescriptor.hpp"

namespace ProjectA {
	class PipelineLayout {
		public:
			virtual ~PipelineLayout() = default;

		protected:
			explicit PipelineLayout(const PipelineLayoutDescriptor& descriptor) {
				DEBUG_VALIDATE_DESCRIPTOR(descriptor);
			}
	};
}