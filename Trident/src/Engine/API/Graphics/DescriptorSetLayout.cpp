// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/DescriptorSetLayout.hpp"
#include "Engine/API/Graphics/Device.hpp"

namespace Trident {
	DescriptorSetLayout::DescriptorSetLayout(const Device* device, const sequence<DescriptorSetLayoutBinding>& bindings) :
		device(device),
		bindings(bindings)
	{}

	DescriptorSetLayout::~DescriptorSetLayout() {
		device->adapter->destroyDescriptorSetLayoutImplementation(device, this);
	}
}