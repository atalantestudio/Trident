// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	#if defined(FORWARD_ENABLE_VERTEX_COLOR_GI)
		void ForwardRenderer::encodeGIVertexGenerationPass(ref<CommandEncoder> commandEncoder) const {
			commandEncoder->bindComputePipeline(giVertexGenerationPipeline);
			commandEncoder->bindDescriptorSets(giVertexGenerationPipelineLayout, 0, giVertexGenerationDescriptorSet);

			// TODO: Assuming 1 point light for now, each thread projects a single triangle for each light.
			commandEncoder->dispatch(4, 1, 1);
		}
	#endif
}