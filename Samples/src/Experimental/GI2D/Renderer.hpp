// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../../Graphics/GraphicsLayer.hpp"

namespace Trident {
	class GI2DExperimentalRenderer : public Renderer {
		public:
			void build() override;

			void render(ref<CommandEncoder> commandEncoder) const override;

		private:
			ref<PipelineLayout> createPipelineLayout() const;

			ref<ComputePipeline> createVertexGenerationPipeline() const;
			ref<GraphicsPipeline> createGlobalIlluminationPipeline() const;

		private:
			ref<PipelineLayout> pipelineLayout;
			ref<ComputePipeline> vertexGenerationPipeline;
			ref<GraphicsPipeline> globalIlluminationPipeline;
	};

	DEFINE_SAMPLE_LAYER(GI2DExperimental);
}