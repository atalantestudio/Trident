// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/QueueFamilyCapability.hpp"

namespace Trident::DX12 {
	inline D3D12_COMMAND_LIST_TYPE getCommandListType(QueueFamilyCapability queueFamilyCapability) {
		static constexpr D3D12_COMMAND_LIST_TYPE COMMAND_LIST_TYPES[] = {
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			D3D12_COMMAND_LIST_TYPE_COMPUTE,
			D3D12_COMMAND_LIST_TYPE_COPY,
		};

		ADAPTER_ASSERT(queueFamilyCapability < QueueFamilyCapability::count, "Invalid queue family capability.");

		return COMMAND_LIST_TYPES[static_cast<uint32>(queueFamilyCapability)];
	}
}