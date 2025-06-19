#pragma once

#include "Engine/API/Rendering/Descriptor/ShaderModuleDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	class ShaderModule {
		public:
			// ShaderModule() = default;

			virtual ~ShaderModule() = default;

		/* protected:
			explicit ShaderModule(const ShaderModuleDescriptor& descriptor) {
				DEBUG_VALIDATE_DESCRIPTOR(descriptor);
			} */
	};
}