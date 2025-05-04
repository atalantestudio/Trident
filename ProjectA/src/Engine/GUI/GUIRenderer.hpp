#pragma once

#include "Engine/API/Event/WindowEvent.hpp"
#include "Engine/ECS/Component/GUIComponent.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class GUIRenderer : public Renderer {
		public:
			GUIRenderer();

			void build();

			// Creates a 1x1 white image.
			uint32 registerDefaultImage();

			// Registers a view of the provided image, and returns its index in the image view array.
			uint32 registerImage(const ref<Image>& image);
			uint32 registerImage(std::string_view path);

			void registerSubcomponent(const mat3x3<float32>& world, const mat3x3<float32>& imageWorld, uint32 imageIndex, const vec4<float32>& color);

			void render(const ref<CommandEncoder>& commandEncoder) override;

			void onEvent(Event& event) override;

		private:
			void setupVertexBuffer();
			void setupIndexBuffer();
			void setupSubcomponentBuffer();
			void setupCameraBuffers();
			void setupSampler();
			void setupRenderPipeline();

			void updateCamera() const;

			void registerImage(const ref<Buffer>& buffer, uint32 width, uint32 height);

		private:
			struct Vertex {
				vec3<float32> position;
				vec2<float32> imageCoordinate;
			};

			struct Subcomponent {
				mat3x3<float32> world;
				mat3x3<float32> imageWorld;
				uint32 imageIndex;
				vec4<float32> color;
			};

		private:
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;
			static constexpr uint32 MAX_SUBCOMPONENTS = 32;
			static constexpr uint32 MAX_IMAGES = 4;

		private:
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;

			ref<Buffer> subcomponentBuffer;
			ref<Buffer> subcomponentStagingBuffer;
			Subcomponent* subcomponentMappedRange = nullptr;

			chain<ref<Buffer>> cameraBuffers;
			chain<mat3x3<float32>*> cameraMappedRanges;

			chain<ref<Image>> images;
			chain<ref<ImageView>> imageViews;

			ref<Sampler> sampler;

			uint32 frameIndex = 0;
			uint32 subcomponentCount = 0;
	};
}