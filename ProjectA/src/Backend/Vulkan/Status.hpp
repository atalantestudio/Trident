#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/Status.hpp"

namespace ProjectA {
	static Status getStatus(VkResult result) {
		if (result == VK_SUCCESS) {
			return Status::SUCCESS;
		}

		if (result == VK_SUBOPTIMAL_KHR) {
			return Status::SUBOPTIMAL;
		}

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			return Status::SUBOPTIMAL;
		}

		return Status::ERROR_UNHANDLED;
	}
}