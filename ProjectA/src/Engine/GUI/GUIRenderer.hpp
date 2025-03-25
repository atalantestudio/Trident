#pragma once

#include "Engine/ECS/ECS.hpp"
#include "Engine/Event/WindowEvent.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class GUIRenderer : public Renderer {
		public:
			GUIRenderer();

			~GUIRenderer();

			void registerEntities();

			void registerColors(const chain<vec4<float32>>& colors) const;

			void render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;

			void onEvent(Event& event) override;

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
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;

		private:
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> positionVertexBuffer;
			ref<Buffer> transformVertexBuffer;
			ref<Buffer> indexBuffer;
			chain<ref<Buffer>> cameraUniformBuffers;
			ref<Buffer> colorStorageBuffer;
			chain<BufferMappedRange> cameraUniformBufferMappedRanges;
			ref<CommandEncoder> commandEncoder;
			uint32 instanceCount = 0;
			uint32 frameIndex = 0;

		public:
			ECS ecs;
	};
}