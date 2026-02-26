// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/Status.hpp"

namespace Trident::VK {
	inline AcquireNextImageStatus fromResult(VkResult result) {
		switch (result) {
			case VK_SUCCESS: {
				return AcquireNextImageStatus::SUCCESS;
			}
			case VK_SUBOPTIMAL_KHR: {
				return AcquireNextImageStatus::SUBOPTIMAL;
			}
			case VK_ERROR_OUT_OF_DATE_KHR: {
				return AcquireNextImageStatus::ERROR_OUT_OF_DATE;
			}
			default: {
				ADAPTER_ASSERT(false, "Unhandled status.");

				return AcquireNextImageStatus::ERROR_UNHANDLED;
			}
		}
	}
}