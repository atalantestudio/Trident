// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/ComputePipeline.hpp"
#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	ComputePipeline::ComputePipeline(const Device* device) :
		device(device)
	{}

	ComputePipeline::~ComputePipeline() {
		device->adapter->destroyComputePipelineImplementation(device, this);
	}
}