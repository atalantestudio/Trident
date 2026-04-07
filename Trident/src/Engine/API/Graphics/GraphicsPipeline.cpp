// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/GraphicsPipeline.hpp"

namespace Trident {
	GraphicsPipeline::GraphicsPipeline(const Device* device) :
		device(device)
	{}

	GraphicsPipeline::~GraphicsPipeline() {
		device->adapter->destroyGraphicsPipelineImplementation(device, this);
	}
}