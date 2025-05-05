#include "pch.hpp"

#ifdef ENABLE_OPENGL
	#include "Backend/OpenGL/OpenGLGraphicsAdapter.hpp"
#endif

#ifdef ENABLE_VULKAN
	#include "Backend/Vulkan/VulkanGraphicsAdapter.hpp"
#endif

#include "Engine/Application.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	GraphicsAPI Renderer::graphicsApi = GraphicsAPI::NONE;
	scope<GraphicsAdapter> Renderer::adapter = nullptr;
	ref<RenderPass> Renderer::mainRenderPass = nullptr;
	ref<Swapchain> Renderer::swapchain = nullptr;
	ref<CommandEncoder> Renderer::commandEncoder = nullptr;
	std::array<ref<Semaphore>, Renderer::MAX_FRAMES_IN_FLIGHT> Renderer::availableImageSemaphores;
	std::array<ref<Semaphore>, Renderer::MAX_FRAMES_IN_FLIGHT> Renderer::finishedRenderSemaphores;
	std::array<ref<Fence>, Renderer::MAX_FRAMES_IN_FLIGHT> Renderer::flightFences;

	GraphicsAPI Renderer::getGraphicsApi() {
		return graphicsApi;
	}

	const scope<GraphicsAdapter>& Renderer::getBackend() {
		return adapter;
	}

	void Renderer::build(const Window& window, GraphicsAPI graphicsApi) {
		Renderer::graphicsApi = graphicsApi;

		switch (Renderer::graphicsApi) {
			case GraphicsAPI::NONE:
				ENGINE_WARN("Headless graphics are not supported.");

				break;
			#ifdef ENABLE_OPENGL
				case GraphicsAPI::OPENGL:
					ENGINE_INFO("Using OpenGL backend.");

					// TODO: Make compatible with new Window API.
					adapter = createScope<OpenGLGraphicsAdapter>(window);

					break;
			#endif
			#ifdef ENABLE_VULKAN
				case GraphicsAPI::VULKAN:
					ENGINE_INFO("Using Vulkan backend.");

					adapter = createScope<VulkanGraphicsAdapter>(window);

					break;
			#endif
		}

		ENGINE_ASSERT(adapter != nullptr, "Failed to initialize the graphics adapter.");

		RenderPassDescriptor renderPassDescriptor{};
		renderPassDescriptor.loadOperation = LoadOperation::CLEAR;
		renderPassDescriptor.storeOperation = StoreOperation::STORE;

		mainRenderPass = createRenderPass(renderPassDescriptor);

		SwapchainDescriptor swapchainDescriptor;
		swapchainDescriptor.renderPass = mainRenderPass;
		swapchainDescriptor.extentWidth = window.getSize()[0];
		swapchainDescriptor.extentHeight = window.getSize()[1];

		swapchain = adapter->createSwapchain(swapchainDescriptor);

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

		commandEncoder = Renderer::getDevice()->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);

		for (uint32 frameIndex = 0; frameIndex < MAX_FRAMES_IN_FLIGHT; frameIndex += 1) {
			availableImageSemaphores[frameIndex] = Renderer::createSemaphore();
			finishedRenderSemaphores[frameIndex] = Renderer::createSemaphore();
			flightFences[frameIndex] = Renderer::createFence();
		}
	}

	void Renderer::getWindowExtent(sint32& extentWidth, sint32& extentHeight) {
		const sint32* size = Application::get().getWindow().getSize();

		extentWidth = size[0];
		extentHeight = size[1];
	}

	ref<Device> Renderer::getDevice() {
		return adapter->getDevice();
	}

	const ref<Swapchain>& Renderer::getSwapchain() {
		return swapchain;
	}

	const ref<RenderPass>& Renderer::getMainRenderPass() {
		return mainRenderPass;
	}

	const ref<CommandEncoder>& Renderer::getCommandEncoder() {
		return commandEncoder;
	}

	const ref<Semaphore>& Renderer::getAvailableImageSemaphore(uint32 frameInFlightIndex) {
		return availableImageSemaphores[frameInFlightIndex];
	}

	const ref<Semaphore>& Renderer::getFinishedRenderSemaphore(uint32 frameInFlightIndex) {
		return finishedRenderSemaphores[frameInFlightIndex];
	}

	const ref<Fence>& Renderer::getFlightFence(uint32 frameInFlightIndex) {
		return flightFences[frameInFlightIndex];
	}

	ref<Buffer> Renderer::createBuffer(const BufferDescriptor& descriptor) {
		return adapter->createBuffer(descriptor);
	}

	ref<Buffer> Renderer::createStagingBuffer(uint64 size, bool persistent) {
		BufferDescriptor stagingBufferDescriptor{};
		stagingBufferDescriptor.size = size;
		stagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;
		stagingBufferDescriptor.persistent = persistent;

		return adapter->createBuffer(stagingBufferDescriptor);
	}

	ref<ComputePipeline> Renderer::createComputePipeline(const ComputePipelineDescriptor& descriptor) {
		return adapter->createComputePipeline(descriptor);
	}

	ref<Fence> Renderer::createFence() {
		return adapter->createFence();
	}

	ref<Image> Renderer::createImage(const ImageDescriptor& descriptor) {
		return adapter->createImage(descriptor);
	}

	ref<PipelineLayout> Renderer::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) {
		return adapter->createPipelineLayout(descriptor);
	}

	ref<RenderPass> Renderer::createRenderPass(const RenderPassDescriptor& descriptor) {
		return adapter->createRenderPass(descriptor);
	}

	ref<RenderPipeline> Renderer::createRenderPipeline(const RenderPipelineDescriptor& descriptor) {
		return adapter->createRenderPipeline(descriptor);
	}

	ref<Sampler> Renderer::createSampler(const SamplerDescriptor& descriptor) {
		return adapter->createSampler(descriptor);
	}

	ref<Semaphore> Renderer::createSemaphore() {
		return adapter->createSemaphore();
	}

	ref<ShaderModule> Renderer::loadShaderModule(const std::string& path, const ShaderModuleDescriptor& descriptor) {
		const chain<uint8> code = Filesystem::loadByteBuffer(path);

		return adapter->createShaderModule(code, descriptor);
	}
}