#pragma once

namespace ProjectA {
	static GLbitfield getGlBarrier(ImageLayout imageLayout) {
		ENGINE_ASSERT(imageLayout != ImageLayout::PRESENT_SRC, "ImageLayout::PRESENT_SRC cannot be used with access flags.");

		static constexpr GLbitfield glBarriers[] = {
			0,
			0,
			0,
			0,
			0,
			GL_SHADER_IMAGE_ACCESS_BARRIER_BIT,
			0,
			GL_TEXTURE_UPDATE_BARRIER_BIT,
			0,
		};

		return glBarriers[static_cast<uint8>(imageLayout)];
	}
}