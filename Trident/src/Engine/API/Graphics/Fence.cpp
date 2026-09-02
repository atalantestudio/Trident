// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/Fence.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	Fence::Fence(const Device* device) :
		device(device)
	{}

	Fence::~Fence() {
		device->adapter->destroyFenceImplementation(device, this);
	}
}