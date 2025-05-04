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

			void addQuad(const mat3x3<float32>& world, const vec4<float32>& color);

			//void registerColors(const chain<vec4<float32>>& colors) const;

			void render(const ref<CommandEncoder>& commandEncoder) override;

			void onEvent(Event& event) override;

		private:
			void setupVertexBuffer();
			void setupIndexBuffer();
			void setupInstanceBuffer();
			//void setupTextureCoordinateBuffer();
			void setupCameraBuffers();
			void setupRenderPipeline();

			void updateCamera() const;

			void copyBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, uint64 instanceSize) const;
			void resetInstanceCount();

		private:
			struct Vertex {
				vec3<float32> position;
				vec2<float32> textureCoordinate;
			};

			struct Instance {
				mat3x3<float32> world;
				vec4<float32> color;
			};

		private:
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;
			static constexpr uint64 MAX_INSTANCES = 32;
			//static constexpr uint32 MAX_COLORS = 32;
			//static constexpr uint32 MAX_TEXTURES = 32;

		private:
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexStagingBuffer;
			ref<Buffer> vertexBuffer;

			//ref<Buffer> textureCoordinateVertexBuffer;

			ref<Buffer> indexBuffer;

			ref<Buffer> instanceBuffer;
			ref<Buffer> instanceStagingBuffer;
			Instance* instanceMappedRange = nullptr;

			//ref<Sampler> sampler;

			chain<ref<Buffer>> cameraBuffers;
			chain<mat3x3<float32>*> cameraMappedRanges;

			uint32 instanceCount = 0;
			uint32 frameIndex = 0;

			//ref<Image> defaultImage;
			//ref<ImageView> defaultImageView;
			//ref<Image> fontImage;
			//ref<ImageView> fontImageView;
	};
}