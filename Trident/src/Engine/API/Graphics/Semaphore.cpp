// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/Semaphore.hpp"

namespace Trident {
	Semaphore::Semaphore(const Device* device) :
		device(device)
	{}

	Semaphore::~Semaphore() {
		device->adapter->destroySemaphoreImplementation(device, this);
	}
}