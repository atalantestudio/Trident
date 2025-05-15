#pragma once

namespace ProjectA {
	static VkFrontFace getVkFrontFace(FrontFace frontFace) {
		static constexpr VkFrontFace vkFrontFaces[] = {
			VK_FRONT_FACE_CLOCKWISE,
			VK_FRONT_FACE_COUNTER_CLOCKWISE,
		};

		return vkFrontFaces[static_cast<uint8>(frontFace)];
	}
}