// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/DescriptorSetLayout.hpp"
#include "Engine/API/Graphics/Device.hpp"

namespace Trident {
	DescriptorSetLayout::DescriptorSetLayout(const Device* device, view<DescriptorSetLayoutBinding> bindings) :
		device(device),
		bindings(bindings.count())
	{
		std::copy(bindings.begin(), bindings.end(), &this->bindings[0]);
	}

	DescriptorSetLayout::~DescriptorSetLayout() {
		device->adapter->destroyDescriptorSetLayoutImplementation(device, this);
	}
}