// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ClearValue.hpp"
#include "Engine/API/Rendering/Framebuffer.hpp"

namespace Trident {
	struct BeginRenderPassDescriptor {
		ref<Framebuffer> framebuffer;

		// 1 clear value per attachment.
		sequence<ClearValue> clearValues;
	};
}