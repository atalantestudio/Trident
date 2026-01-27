// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Descriptor/ShaderModuleDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class ShaderModule {
		public:
			virtual ~ShaderModule() = default;

		protected:
			explicit ShaderModule(const ShaderModuleDescriptor& descriptor) {
				DEBUG_DESCRIPTOR_GUARD(descriptor);
			}
	};
}