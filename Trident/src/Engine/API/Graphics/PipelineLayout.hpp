// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Descriptor/PipelineLayoutDescriptor.hpp"

namespace Trident {
	class PipelineLayout {
		public:
			explicit PipelineLayout(const Device* device);

			~PipelineLayout();

		public:
			const Device* device;
	};
}