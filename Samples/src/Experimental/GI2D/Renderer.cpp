// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Experimental/GI2D/GlobalIlluminationPipeline.hpp"
#include "Experimental/GI2D/PipelineLayout.hpp"
#include "Experimental/GI2D/Renderer.hpp"
#include "Experimental/GI2D/VertexGenerationPipeline.hpp"

namespace Trident {
	void GI2DExperimentalRenderer::build() {
		pipelineLayout = createPipelineLayout();
		vertexGenerationPipeline = createVertexGenerationPipeline();
		globalIlluminationPipeline = createGlobalIlluminationPipeline();
	}

	void GI2DExperimentalRenderer::render(ref<CommandEncoder> commandEncoder) const {
		const Extent2D extent = getSwapchain()->getExtent();
		const float32 width = static_cast<float32>(extent.x);
		const float32 height = static_cast<float32>(extent.y);

		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, extent);

		commandEncoder->bindGraphicsPipeline(globalIlluminationPipeline);
		commandEncoder->draw(36, 1, 0, 0);
	}
}