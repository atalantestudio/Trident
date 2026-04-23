// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Descriptor/ShaderModuleDescriptor.hpp"
#include "Engine/API/Graphics/Device.hpp"

namespace Trident {
	class ShaderModule {
		public:
			~ShaderModule();

		protected:
			explicit ShaderModule(const Device* device);

		public:
			const Device* device;
	};
}