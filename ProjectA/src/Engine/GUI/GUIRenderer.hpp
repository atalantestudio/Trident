#pragma once

#include "Engine/API/Event/WindowEvent.hpp"
#include "Engine/ECS/Component/GUIComponent.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class GUIRenderer : public Renderer {
		public:
			GUIRenderer();

			//ref<Image> loadImage(const std::string& path);

			//ref<Image> loadImage(const ref<Buffer>& buffer, uint32 width, uint32 height);

			//ref<Image> loadDefaultImage();

			//void registerEntityPool(const ComponentPool<GUIComponent>& componentPool);

			void addQuad(const mat3x3<float32>& world);

			//void registerColors(const chain<vec4<float32>>& colors) const;

			void render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;

			void onEvent(Event& event) override;

		private:
			void setupVertexBuffer();
			void setupIndexBuffer();
			void setupWorldBuffer();
			//void setupColorBuffer();
			//void setupTextureCoordinateBuffer();
			void setupCameraBuffers();
			void setupRenderPipeline();

			void updateCamera() const;

			void copyWorldBuffer() const;
			void clearWorldBuffer();

		private:
			struct Vertex {
				vec3<float32> position;
				vec2<float32> textureCoordinate;
			};

		private:
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;
			static constexpr uint64 MAX_INSTANCES = 2;
			//static constexpr uint32 MAX_COLORS = 32;
			//static constexpr uint32 MAX_TEXTURES = 32;

		private:
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexStagingBuffer;
			ref<Buffer> vertexBuffer;

			ref<Buffer> indexBuffer;

			ref<Buffer> worldBuffer;
			ref<Buffer> worldStagingBuffer;
			mat3x3<float32>* worldMappedRange = nullptr;

			//ref<Buffer> textureCoordinateVertexBuffer;

			ref<Sampler> sampler;

			chain<ref<Buffer>> cameraBuffers;
			chain<mat3x3<float32>*> cameraMappedRanges;

			//ref<Buffer> colorStorageBuffer;

			uint32 instanceCount = 0;
			uint32 frameIndex = 0;

			ref<Image> defaultImage;
			ref<ImageView> defaultImageView;
			ref<Image> fontImage;
			ref<ImageView> fontImageView;
	};
}