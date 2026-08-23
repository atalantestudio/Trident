// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/Descriptor/DescriptorSetLayoutDescriptor.hpp"

namespace Trident {
	class DescriptorSetLayout {
		public:
			explicit DescriptorSetLayout(const Device* device, const DescriptorSetLayoutDescriptor& descriptor);

			~DescriptorSetLayout();

		public:
			const Device* device;

			sequence<DescriptorSetLayoutBinding> bindings;
	};
}