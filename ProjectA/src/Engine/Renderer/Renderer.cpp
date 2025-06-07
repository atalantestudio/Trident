#include "pch.hpp"

#ifdef ENABLE_OPENGL
	#include "Backend/OpenGL/OpenGLGraphicsAdapter.hpp"
#endif

#ifdef ENABLE_VULKAN
	#include "Backend/Vulkan/VulkanGraphicsAdapter.hpp"
#endif

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	GraphicsAPI Renderer::graphicsApi = GraphicsAPI::NONE;
	scope<GraphicsAdapter> Renderer::adapter = nullptr;
	uint32 Renderer::imageIndex = 0;
	ref<Swapchain> Renderer::swapchain = nullptr;
	std::array<ref<CommandEncoder>, Application::MAX_FRAMES_IN_FLIGHT> Renderer::commandEncoders;
	std::array<ref<Fence>, Application::MAX_FRAMES_IN_FLIGHT> Renderer::flightFences;
	std::array<ref<Semaphore>, Application::MAX_FRAMES_IN_FLIGHT> Renderer::acquireNextImageSemaphores;
	std::array<ref<Semaphore>, Application::MAX_FRAMES_IN_FLIGHT> Renderer::finishedRenderSemaphores;

	GraphicsAPI Renderer::getGraphicsApi() {
		return graphicsApi;
	}

	const scope<GraphicsAdapter>& Renderer::getBackend() {
		return adapter;
	}

	void Renderer::build(const Window& window, GraphicsAPI graphicsApi, bool vSync) {
		Renderer::graphicsApi = graphicsApi;

		switch (Renderer::graphicsApi) {
			case GraphicsAPI::NONE:
				ENGINE_WARN("Headless graphics are not supported.");

				break;
			#ifdef ENABLE_OPENGL
				case GraphicsAPI::OPENGL:
					ENGINE_INFO("Using OpenGL backend.");

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

		DEBUG_ASSERT(adapter != nullptr);

		SwapchainDescriptor swapchainDescriptor{};
		swapchainDescriptor.extentWidth = window.getSize()[0];
		swapchainDescriptor.extentHeight = window.getSize()[1];
		swapchainDescriptor.vSync = vSync;

		swapchain = adapter->createSwapchain(swapchainDescriptor);

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

		for (uint32 frameIndex = 0; frameIndex < Application::MAX_FRAMES_IN_FLIGHT; frameIndex += 1) {
			commandEncoders[frameIndex] = adapter->getDevice()->getGraphicsQueue()->createCommandEncoder(commandBufferDescriptor);
			flightFences[frameIndex] = adapter->createFence();
			acquireNextImageSemaphores[frameIndex] = adapter->createSemaphore();
			finishedRenderSemaphores[frameIndex] = adapter->createSemaphore();
		}
	}

	void Renderer::resize(WindowResizeEvent& event) {}

	void Renderer::onEvent(Event& event) {}

	ref<Buffer> Renderer::createBuffer(const BufferDescriptor& descriptor) {
		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		return adapter->createBuffer(descriptor);
	}

	ref<Buffer> Renderer::createStagingBuffer(uint64 size, bool persistent) {
		BufferDescriptor stagingBufferDescriptor;
		stagingBufferDescriptor.size = size;
		stagingBufferDescriptor.usage = BufferUsage::TRANSFER_SRC;
		stagingBufferDescriptor.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT;
		stagingBufferDescriptor.persistent = persistent;

		return createBuffer(stagingBufferDescriptor);
	}

	ref<ComputePipeline> Renderer::createComputePipeline(const ComputePipelineDescriptor& descriptor) {
		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		return adapter->createComputePipeline(descriptor);
	}

	ref<Fence> Renderer::createFence() {
		return adapter->createFence();
	}

	ref<Image> Renderer::createImage(const ImageDescriptor& descriptor) {
		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		return adapter->createImage(descriptor);
	}

	ref<PipelineLayout> Renderer::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) {
		return adapter->createPipelineLayout(descriptor);
	}

	ref<RenderPass> Renderer::createRenderPass(const RenderPassDescriptor& descriptor) {
		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		return adapter->createRenderPass(descriptor);
	}

	ref<RenderPipeline> Renderer::createRenderPipeline(const RenderPipelineDescriptor& descriptor) {
		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		return adapter->createRenderPipeline(descriptor);
	}

	ref<Sampler> Renderer::createSampler(const SamplerDescriptor& descriptor) {
		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		return adapter->createSampler(descriptor);
	}

	ref<Semaphore> Renderer::createSemaphore() {
		return adapter->createSemaphore();
	}

	const ref<Semaphore>& Renderer::getAcquireNextImageSemaphore(uint32 frameInFlightIndex) {
		return acquireNextImageSemaphores[frameInFlightIndex];
	}

	const ref<CommandEncoder>& Renderer::getCommandEncoder(uint32 frameInFlightIndex) {
		return commandEncoders[frameInFlightIndex];
	}

	ref<Device> Renderer::getDevice() {
		return adapter->getDevice();
	}

	const ref<Semaphore>& Renderer::getFinishedRenderSemaphore(uint32 frameInFlightIndex) {
		return finishedRenderSemaphores[frameInFlightIndex];
	}

	const ref<Fence>& Renderer::getFlightFence(uint32 frameInFlightIndex) {
		return flightFences[frameInFlightIndex];
	}

	uint32 Renderer::getImageIndex() {
		return imageIndex;
	}

	ref<Surface> Renderer::getSurface() {
		return adapter->getSurface();
	}

	const ref<Swapchain>& Renderer::getSwapchain() {
		return swapchain;
	}

	ref<ShaderModule> Renderer::loadShaderModule(std::string_view path, const ShaderModuleDescriptor& descriptor) {
		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		const sequence<uint8> code = Filesystem::loadByteBuffer(path);

		return adapter->createShaderModule(code, descriptor);
	}

	void Renderer::setImageIndex(uint32 imageIndex) {
		Renderer::imageIndex = imageIndex;
	}
}