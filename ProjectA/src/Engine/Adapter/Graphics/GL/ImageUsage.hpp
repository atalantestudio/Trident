#pragma once

#include "Engine/API/Rendering/ImageUsage.hpp"

namespace ProjectA {
	static GLenum getGlImageUsage(ImageUsage imageUsage) {
		if (imageUsage & ImageUsage::COLOR_ATTACHMENT) {
			return GL_COLOR_ATTACHMENT0;
		}

		if (imageUsage & ImageUsage::DEPTH_STENCIL_ATTACHMENT) {
			return GL_DEPTH_STENCIL_ATTACHMENT;
		}

		// TODO: Handle GL_DEPTH_ATTACHMENT.
		// TODO: Handle GL_STENCIL_ATTACHMENT.

		ENGINE_ASSERT(false, "Unhandled image usage.");

		return 0;
	}
}