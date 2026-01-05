#pragma once

#include "Engine/API/Event/Window/WindowResizeEvent.hpp"
#include "Engine/API/Rendering/CommandEncoder.hpp"
#include "Engine/API/Rendering/Descriptor/PipelineLayoutDescriptor.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/Application/Application.hpp"

namespace ProjectA {
	class Renderer {
		public:
			static GraphicsAPI getGraphicsApi();

			static void initialize(const ProjectDescriptor& projectDescriptor, const Window& window, GraphicsAPI graphicsAPI, bool vSync);

		public:
			virtual void build() = 0;

			virtual void render(const ref<CommandEncoder>& commandEncoder) const = 0;

			virtual void resize(WindowResizeEvent& event);

			virtual void onEvent(Event& event);

		public:
			static ref<Buffer> createBuffer(const BufferDescriptor& descriptor);

			// Creates and returns a staging buffer.
			// `size` is in bytes.
			// If `persistent` is true, the buffer will stay mapped until it is destroyed.
			static ref<Buffer> createStagingBuffer(uint64 size, bool persistent = false);

			static ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor);

			static ref<DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings);

			static ref<Fence> createFence();

			static ref<Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor);

			static ref<Image> createImage(const ImageDescriptor& descriptor);

			static ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor);

			static ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor);

			static ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor);

			static ref<Sampler> createSampler(const SamplerDescriptor& descriptor);

			static ref<Semaphore> createSemaphore();

			static ref<Subpass> createSubpass(const SubpassDescriptor& descriptor);

			static const ref<Semaphore>& getAcquireNextImageSemaphore(uint32 frameInFlightIndex);

			static const ref<CommandEncoder>& getCommandEncoder(uint32 frameInFlightIndex);

			static ref<Device> getDevice();

			static const ref<Semaphore>& getFinishedRenderSemaphore(uint32 frameInFlightIndex);

			static const ref<Fence>& getFlightFence(uint32 frameInFlightIndex);

			static uint32 getImageIndex();

			static ref<Surface> getSurface();

			static const ref<Swapchain>& getSwapchain();

			static ref<ShaderModule> loadShaderModule(const ref<ShaderModuleAsset>& shaderModuleAsset, const ShaderModuleDescriptor& descriptor);

			// TODO: Deprecate.
			static ref<ShaderModule> loadShaderModule(view<char8> path, const ShaderModuleDescriptor& descriptor);

			// Use to create HLSL shader modules.
			// NOTE: This function will be removed once TSL can be transpiled to HLSL (or DXIL).
			static ref<ShaderModule> loadHlslShaderModule(view<char8> code, const ShaderModuleDescriptor& descriptor);

			static void setImageIndex(uint32 imageIndex);

		public:
			void setLayer(const Layer* layer) {
				this->layer = layer;
			}

		protected:
			const Layer* getLayer() const {
				return layer;
			}

		private:
			static GraphicsAPI graphicsApi;
			static scope<GraphicsAdapter> adapter;

			static uint32 imageIndex;

			// TODO: Use scope.
			static ref<Swapchain> swapchain;

			// TODO: Use scope.
			static std::array<ref<CommandEncoder>, Application::MAX_FRAMES_IN_FLIGHT> commandEncoders;

			// TODO: Use scope.
			static std::array<ref<Fence>, Application::MAX_FRAMES_IN_FLIGHT> flightFences;

			// TODO: Use scope.
			static std::array<ref<Semaphore>, Application::MAX_FRAMES_IN_FLIGHT> acquireNextImageSemaphores;

			// TODO: Use scope.
			static std::array<ref<Semaphore>, Application::MAX_FRAMES_IN_FLIGHT> finishedRenderSemaphores;

		private:
			const Layer* layer;
	};
}