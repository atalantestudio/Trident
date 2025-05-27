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

			void copyMappedBuffer() const;

			void render(const ref<CommandEncoder>& commandEncoder) const final override;

			void resize(WindowResizeEvent& event) final override;

			void resetSubcomponentCount();

		private:
			void initializeVertexBuffer();
			void initializeIndexBuffer();
			void initializeSubcomponentBuffer();
			void initializeCameraBuffer();
			void initializeSampler();
			void initializeRenderPipeline();

			void updateCamera() const;

			void registerImage(const ref<Buffer>& buffer, uint32 width, uint32 height);

		private:
			struct Vertex {
				// TODO: Remove padding.
				vec4<float32> position;

				svec2<float32> imageCoordinate;
			};

			struct Subcomponent {
				mat3x3<float32> world;
				mat3x3<float32> imageWorld;
				uint32 imageIndex;
				vec4<float32> color;
			};

		private:
			static constexpr uint32 MAX_SUBCOMPONENTS = 32;
			static constexpr uint32 MAX_IMAGES = 4;

		private:
			ref<RenderPass> renderPass;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;

			ref<Buffer> subcomponentBuffer;
			ref<Buffer> subcomponentStagingBuffer;
			Subcomponent* subcomponentMappedRange;

			ref<Buffer> cameraBuffer;
			mat3x3<float32>* cameraMappedRange;

			std::vector<ref<Image>> images;
			std::vector<ref<ImageView>> imageViews;

			ref<Sampler> sampler;

			uint32 subcomponentCount = 0;
	};
}