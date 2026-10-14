// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
		void ForwardRenderer::encodeGIShadingPass(ref<CommandEncoder> commandEncoder) const {
			commandEncoder->bindGraphicsPipeline(giShadingPipeline);
			commandEncoder->bindVertexBuffer(0, giVertexBuffer, 0);
			commandEncoder->bindDescriptorSets(giShadingPipelineLayout, 0, giVertexGenerationDescriptorSet);
			commandEncoder->draw(6, 1, 0, 0);
		}
	#endif
}