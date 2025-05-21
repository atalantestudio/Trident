#pragma once

namespace ProjectA {
	VkImageAspectFlags getVkImageAspectFlags(ImageAspect imageAspect) {
		return static_cast<VkImageAspectFlags>(imageAspect);
	}
}