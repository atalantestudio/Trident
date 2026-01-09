// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/ImageUsage.hpp"

namespace ProjectA::GL {
	inline GLenum getImageUsage(ImageUsage imageUsage) {
		if (imageUsage & ImageUsage::COLOR_ATTACHMENT) {
			return GL_COLOR_ATTACHMENT0;
		}

		if (imageUsage & ImageUsage::DEPTH_STENCIL_ATTACHMENT) {
			return GL_DEPTH_STENCIL_ATTACHMENT;
		}

		// TODO: Handle GL_DEPTH_ATTACHMENT.
		// TODO: Handle GL_STENCIL_ATTACHMENT.

		ADAPTER_ASSERT(false, "Unsupported image usage.");

		return 0;
	}
}