#pragma once

#include "Engine/Acceleration/seq.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class GUIRenderer : public Renderer {
		public:
			GUIRenderer();

			void registerEntities(const ECS& ecs);

			void render() override;

		private:
			void updateUniformBufferObjects() const;

		private:
			struct UniformBufferObject {
				mat3x3<float32> projection;
			};

		private:
			static constexpr uint32 MAX_QUADS = 64;
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;

		private:
			ref<RenderPass> renderPass;
			ref<Swapchain> swapchain;
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> positionVertexBuffer;
			ref<Buffer> transformVertexBuffer;
			ref<Buffer> indexBuffer;
			seq<ref<Buffer>> uniformBuffers;
			seq<void*> uniformBufferMappedMemory;
			ref<CommandEncoder> commandEncoder;
			seq<ref<Semaphore>> availableImageSemaphores;
			seq<ref<Semaphore>> finishedRenderSemaphores;
			seq<ref<Fence>> flightFences;

			uint32 instanceCount = 0;
			uint32 frameIndex = 0;
	};
}