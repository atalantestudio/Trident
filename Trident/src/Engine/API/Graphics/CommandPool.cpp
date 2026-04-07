// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/CommandPool.hpp"
#include "Engine/API/Graphics/Device.hpp"

namespace Trident {
	CommandPool::CommandPool(const Device* device) :
		device(device)
	{}

	CommandPool::~CommandPool() {
		device->adapter->destroyCommandPoolImplementation(device, this);
	}
}