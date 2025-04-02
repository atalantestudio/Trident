#pragma once

#include "Engine/ECS/Component/GUIComponent.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Event/WindowEvent.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class GUIRenderer : public Renderer {
		public:
			GUIRenderer();

			ref<Image> loadImage(const std::string& path);

			ref<Image> loadImage(const ref<Buffer>& buffer, uint32 width, uint32 height);

			ref<Image> loadDefaultImage();

			void registerEntityPool(const ComponentPool<GUIComponent>& componentPool);

			void registerColors(const chain<vec4<float32>>& colors) const;

			void render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;

			void onEvent(Event& event) override;

		private:
			void updateCamera() const;

		private:
			struct Vertex {
				vec3<float32> position;
				vec2<float32> textureCoordinates;
			};

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
			ref<Buffer> textureCoordinateVertexBuffer;
			ref<Buffer> transformVertexBuffer;
			ref<Buffer> indexBuffer;
			ref<Sampler> sampler;
			chain<ref<Buffer>> cameraUniformBuffers;
			ref<Buffer> colorStorageBuffer;
			chain<UniformBufferObject*> cameraUniformBufferMappedRanges;
			ref<CommandEncoder> commandEncoder;
			uint32 instanceCount = 0;
			uint32 frameIndex = 0;

			ref<Image> defaultImage;
			ref<ImageView> defaultImageView;
			ref<Image> fontImage;
			ref<ImageView> fontImageView;
	};
}