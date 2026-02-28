// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::VK {
	void* loadInstanceProcedure(VkInstance instance, view<char8> name);

	void* loadDeviceProcedure(VkDevice device, view<char8> name);

	void bindAdapter(Trident::GraphicsAdapter* adapter);
}