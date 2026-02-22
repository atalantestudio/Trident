// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Descriptor/PipelineLayoutDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class PipelineLayout {
		public:
			~PipelineLayout();

		protected:
			explicit PipelineLayout(const Device* device);

		public:
			const Device* device;
	};
}