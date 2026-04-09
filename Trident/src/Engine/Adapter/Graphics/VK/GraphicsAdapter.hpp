// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::VK {
	template<typename Procedure>
	Procedure loadInstanceProcedure(VkInstance instance, view<char8> name) {
		static const PlatformLibrary library = Platform::openLibrary("vulkan-1.dll");
		static PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = library.loadProcedure<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");

		const PFN_vkVoidFunction procedure = vkGetInstanceProcAddr(instance, &name[0]);

		ADAPTER_ASSERT(procedure != nullptr, "Cannot find instance procedure \"[]\".", name);

		return function_cast<Procedure>(procedure);
	}

	template<typename Procedure>
	Procedure loadDeviceProcedure(VkDevice device, view<char8> name) {
		static const PlatformLibrary library = Platform::openLibrary("vulkan-1.dll");
		static PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = library.loadProcedure<PFN_vkGetDeviceProcAddr>("vkGetDeviceProcAddr");

		const PFN_vkVoidFunction procedure = vkGetDeviceProcAddr(device, &name[0]);

		ADAPTER_ASSERT(procedure != nullptr, "Cannot find device procedure \"[]\".", name);

		return function_cast<Procedure>(procedure);
	}

	void bindAdapter(Trident::GraphicsAdapter* adapter);
}