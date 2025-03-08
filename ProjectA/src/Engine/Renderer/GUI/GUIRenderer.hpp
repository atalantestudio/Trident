#pragma once

#include "Engine/ECS/ECS.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class GUIRenderer : public Renderer {
		public:
			GUIRenderer();

			void registerEntities(const ECS& ecs);

			void registerColors(const std::vector<vec4<float32>>& colors);

			void render() override;

			void onEvent(Event& event) override;

		private:
			void updateCameraUniform() const;

		private:
			struct UniformBufferObject {
				mat3x3<float32> projection;
			};

		private:
			static constexpr uint32 MAX_QUADS = 64;
			static constexpr uint32 MAX_COLORS = 32;
			static constexpr uint32 MAX_TEXTURES = 32;
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;

		private:
			ref<RenderPass> renderPass;
			ref<Swapchain> swapchain;
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> positionVertexBuffer;
			ref<Buffer> transformVertexBuffer;
			ref<Buffer> indexBuffer;
			chain<ref<Buffer>> cameraUniformBuffers;
			ref<Buffer> colorStorageBuffer;

			// TODO: Remove?
			chain<BufferMappedRange> cameraUniformBufferMappedRanges;

			ref<CommandEncoder> commandEncoder;
			chain<ref<Semaphore>> availableImageSemaphores;
			chain<ref<Semaphore>> finishedRenderSemaphores;
			chain<ref<Fence>> flightFences;
			uint32 instanceCount = 0;
			uint32 frameIndex = 0;
	};
}