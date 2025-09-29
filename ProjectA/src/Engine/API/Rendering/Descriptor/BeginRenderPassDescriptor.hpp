#pragma once

#include "Engine/API/Rendering/ClearValue.hpp"
#include "Engine/API/Rendering/Framebuffer.hpp"

namespace ProjectA {
	struct BeginRenderPassDescriptor {
		ref<Framebuffer> framebuffer;

		// TODO: Use sequence<ClearValue>.
		// 1 clear value per attachment.
		std::vector<ClearValue> clearValues;
	};
}