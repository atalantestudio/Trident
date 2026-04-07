// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/PipelineLayout.hpp"

namespace Trident {
	PipelineLayout::PipelineLayout(const Device* device) :
		device(device)
	{}

	PipelineLayout::~PipelineLayout() {
		device->adapter->destroyPipelineLayoutImplementation(device, this);
	}
}