// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/PhysicalDevice.hpp"

namespace Trident {
	view<char8> PhysicalDevice::getName() const {
		return adapter->getPhysicalDeviceName(this);
	}
}