// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/ShaderModule.hpp"

namespace Trident {
	ShaderModule::ShaderModule(const Device* device) :
		device(device)
	{}

	ShaderModule::~ShaderModule() {
		device->adapter->destroyShaderModuleImplementation(device, this);
	}
}