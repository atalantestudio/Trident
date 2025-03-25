#pragma once

#include "Engine/ECS/ECS.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class GUIRenderer : public Renderer {
		public:
			GUIRenderer();

			~GUIRenderer();

			void registerEntities();

			void registerColors(const std::vector<vec4<float32>>& colors) const;

			void render() override;

			void resize();

			void onEvent(Event& event) override;

			bool onResizeEvent(WindowResizeEvent& event);

		private:
			void updateScriptComponents() const;

			void destroyScriptComponents() const;

			void updateCamera() const;

		private:
			struct UniformBufferObject {
				mat3x3<float32> projection;
			};

		private:
			static constexpr uint32 MAX_QUADS = 64;
			static constexpr uint32 MAX_COLORS = 32;
			static constexpr uint32 MAX_TEXTURES = 32;
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 1;

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
			chain<BufferMappedRange> cameraUniformBufferMappedRanges;
			ref<CommandEncoder> commandEncoder;
			chain<ref<Semaphore>> availableImageSemaphores;
			chain<ref<Semaphore>> finishedRenderSemaphores;
			chain<ref<Fence>> flightFences;
			uint32 instanceCount = 0;
			uint32 frameIndex = 0;
			bool needsResize = false;

		public:
			ECS ecs;
	};
}