// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/Status.hpp"

namespace Trident::VK {
	inline AcquireNextImageStatus acquireNextImageStatusFromResult(VkResult result) {
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

	inline PresentStatus presentStatusFromResult(VkResult result) {
		switch (result) {
			case VK_SUCCESS: {
				return PresentStatus::SUCCESS;
			}
			case VK_SUBOPTIMAL_KHR: {
				return PresentStatus::SUBOPTIMAL;
			}
			case VK_ERROR_OUT_OF_DATE_KHR: {
				return PresentStatus::ERROR_OUT_OF_DATE;
			}
			default: {
				ADAPTER_ASSERT(false, "Unhandled status.");

				return PresentStatus::ERROR_UNHANDLED;
			}
		}
	}
}