// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/DeviceExtensionName.hpp"

namespace Trident {
	template<DeviceExtensionName Name, typename Device>
	class DeviceExtension;

	template<DeviceExtensionName Name, typename CommandEncoder>
	class CommandEncoderDeviceExtension;
}