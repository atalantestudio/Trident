#pragma once

#include "Engine/API/Event/Window/WindowResizeEvent.hpp"
#include "Engine/API/Rendering/CommandEncoder.hpp"
#include "Engine/API/Rendering/Descriptor/PipelineLayoutDescriptor.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/Application/Application.hpp"
#include "Engine/GraphicsAdapter.hpp"
#include "Engine/Renderer/GraphicsAPI.hpp"

namespace ProjectA {
	class Renderer {
		public:
			static GraphicsAPI getGraphicsApi();

			static const scope<GraphicsAdapter>& getBackend();

			static void initialize(const ApplicationManifest& applicationManifest, const Window& window, GraphicsAPI graphicsAPI, bool vSync);

		public:
			virtual void build() = 0;

			virtual void render(const ref<CommandEncoder>& commandEncoder) const = 0;

			virtual void resize(WindowResizeEvent& event);

			virtual void onEvent(Event& event);

		public:
			static ref<Buffer> createBuffer(const BufferDescriptor& descriptor);

			// Creates and returns a staging buffer.
			// `size` is in bytes.
			// If `persistent` is true, then the buffer is configured to be mapped permanently.
			static ref<Buffer> createStagingBuffer(uint64 size, bool persistent = false);

			static ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor);

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

			// Loads and returns a SPIR-V shader module.
			static ref<ShaderModule> loadShaderModule(std::string_view path, const ShaderModuleDescriptor& descriptor);

			static void setImageIndex(uint32 imageIndex);

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
	};
}