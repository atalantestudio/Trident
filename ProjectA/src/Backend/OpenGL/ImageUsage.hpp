#pragma once

namespace ProjectA {
	static GLenum getGlImageUsage(ImageUsage imageUsage) {
		static constexpr GLenum glImageUsages[] = {
			GL_COLOR_ATTACHMENT0,
			GL_DEPTH_ATTACHMENT,
			// TODO: Add GL_DEPTH_STENCIL_ATTACHMENT.
			// TODO: Add GL_STENCIL_ATTACHMENT.
		};

		return glImageUsages[static_cast<uint8>(imageUsage)];
	}
}