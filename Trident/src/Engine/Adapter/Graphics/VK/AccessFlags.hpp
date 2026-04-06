// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Access.hpp"

namespace Trident::VK {
	inline VkAccessFlags getAccessFlags(Access access) {
		// TODO: Use VK_ACCESS_NONE.
		VkAccessFlags accessFlags{};

		if (access & Access::SHADER_READ) {
			accessFlags |= VK_ACCESS_SHADER_READ_BIT;
		}

		if (access & Access::SHADER_WRITE) {
			accessFlags |= VK_ACCESS_SHADER_WRITE_BIT;
		}

		if (access & Access::TRANSFER_READ) {
			accessFlags |= VK_ACCESS_TRANSFER_READ_BIT;
		}

		if (access & Access::TRANSFER_WRITE) {
			accessFlags |= VK_ACCESS_TRANSFER_WRITE_BIT;
		}

		return accessFlags;
	}
}