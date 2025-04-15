#pragma once

#include "Engine/API/CommandEncoder.hpp"
#include "Engine/API/Event/Event.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/GraphicsAdapter.hpp"
#include "Engine/Renderer/GraphicsAPI.hpp"

namespace ProjectA {
	class Renderer {
		public:
			static GraphicsAPI getGraphicsApi();

			static const scope<GraphicsAdapter>& getBackend();

			static void recreate(const Window& window, GraphicsAPI graphicsAPI);

			static void getWindowExtent(sint32& extentWidth, sint32& extentHeight);

		public:
			virtual void render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) = 0;

			virtual void onEvent(Event& event) {}

		public:
			static ref<Device> getDevice();

			static ref<Buffer> createBuffer(const BufferDescriptor& descriptor);

			static ref<Buffer> createStagingBuffer(uint64 size);

			static ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor);

			static ref<Fence> createFence();

			static ref<Image> createImage(const ImageDescriptor& descriptor);

			static ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor);

			static ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor);

			static ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor);

			static ref<Sampler> createSampler(const SamplerDescriptor& descriptor);

			static ref<Semaphore> createSemaphore();

			// Loads and returns a SPIR-V shader module.
			static ref<ShaderModule> loadShaderModule(const std::string& path, const ShaderModuleDescriptor& descriptor);

		private:
			static GraphicsAPI graphicsApi;
			static scope<GraphicsAdapter> adapter;
	};
}