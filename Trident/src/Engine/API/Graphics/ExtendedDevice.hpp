// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/CommandEncoder.hpp"
#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/DeviceExtension/DeviceExtension.hpp"

namespace Trident {
	template<DeviceExtensionName... Extensions>
	class ExtendedDevice : public Device, public DeviceExtension<Extensions, ExtendedDevice<Extensions...>>... {};

	template<DeviceExtensionName... Extensions>
	class ExtendedCommandEncoder : public CommandEncoder, public CommandEncoderDeviceExtension<Extensions, ExtendedCommandEncoder<Extensions...>>... {};
}