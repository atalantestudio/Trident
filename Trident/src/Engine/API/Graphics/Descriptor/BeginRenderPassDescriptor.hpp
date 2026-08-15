// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ClearValue.hpp"
#include "Engine/API/Graphics/Framebuffer.hpp"

namespace Trident {
	struct BeginRenderPassDescriptor {
		ref<Framebuffer> framebuffer;

		// 1 clear value per attachment.
		// TODO(graphics/command-encoder/begin-render-pass): Replace with a view.
		sequence<ClearValue> clearValues;
	};
}