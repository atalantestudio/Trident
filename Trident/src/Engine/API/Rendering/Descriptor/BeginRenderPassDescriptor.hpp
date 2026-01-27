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