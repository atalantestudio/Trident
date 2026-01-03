#include "pch.hpp"

#ifdef ENABLE_DX12_ADAPTER
	#include "Engine/Adapter/Graphics/DX12/GraphicsAdapter.hpp"
#endif

#ifdef ENABLE_GL_ADAPTER
	#include "Engine/Adapter/Graphics/GL/GraphicsAdapter.hpp"
#endif

#ifdef ENABLE_VK_ADAPTER
	#include "Engine/Adapter/Graphics/VK/GraphicsAdapter.hpp"
#endif

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	GraphicsAPI Renderer::graphicsApi = GraphicsAPI::CPU;
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

	void Renderer::initialize(const ProjectDescriptor& projectDescriptor, const Window& window, GraphicsAPI graphicsApi, bool vSync) {
		Renderer::graphicsApi = graphicsApi;

		switch (Renderer::graphicsApi) {
			case GraphicsAPI::CPU: {
				ENGINE_ERROR_UNSUPPORTED();

				break;
			}
			#ifdef ENABLE_DX12_ADAPTER
				case GraphicsAPI::DX12: {
					ENGINE_INFO("Using DirectX 12 graphics adapter.");

					adapter = createScope<DX12::GraphicsAdapter>(projectDescriptor, window);

					break;
				}
			#endif
			#ifdef ENABLE_GL_ADAPTER
				case GraphicsAPI::GL: {
					ENGINE_INFO("Using OpenGL graphics adapter.");

					adapter = createScope<OpenGLGraphicsAdapter>(projectDescriptor, window);

					break;
				}
			#endif
			#ifdef ENABLE_VK_ADAPTER
				case GraphicsAPI::VK: {
					ENGINE_INFO("Using Vulkan graphics adapter.");

					adapter = createScope<VulkanGraphicsAdapter>(projectDescriptor, window);

					break;
				}
			#endif
		}

		DEBUG_ASSERT(adapter != nullptr);

		SwapchainDescriptor swapchainDescriptor{};
		swapchainDescriptor.extent.width = window.getSize()[0];
		swapchainDescriptor.extent.height = window.getSize()[1];
		swapchainDescriptor.vSync = vSync;

		swapchain = adapter->createSwapchain(swapchainDescriptor);

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

		const ref<Queue> graphicsQueue = adapter->getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);

		for (uint32 frameIndex = 0; frameIndex < Application::MAX_FRAMES_IN_FLIGHT; frameIndex += 1) {
			commandEncoders[frameIndex] = graphicsQueue->createCommandEncoder(commandBufferDescriptor);
			flightFences[frameIndex] = adapter->createFence();
			acquireNextImageSemaphores[frameIndex] = adapter->createSemaphore();
			finishedRenderSemaphores[frameIndex] = adapter->createSemaphore();
		}
	}

	void Renderer::resize(WindowResizeEvent& event) {}

	void Renderer::onEvent(Event& event) {}

	ref<Buffer> Renderer::createBuffer(const BufferDescriptor& descriptor) {
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
		return adapter->createComputePipeline(descriptor);
	}

	ref<DescriptorSetLayout> Renderer::createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) {
		return adapter->createDescriptorSetLayout(bindings);
	}

	ref<Fence> Renderer::createFence() {
		return adapter->createFence();
	}

	ref<Framebuffer> Renderer::createFramebuffer(const FramebufferDescriptor& descriptor) {
		return adapter->createFramebuffer(descriptor);
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

	ref<Subpass> Renderer::createSubpass(const SubpassDescriptor& descriptor) {
		return createRef<Subpass>(descriptor);
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

	ref<ShaderModule> Renderer::loadShaderModule(const ref<ShaderModuleAsset>& shaderModuleAsset, const ShaderModuleDescriptor& descriptor) {
		return adapter->createShaderModule(shaderModuleAsset, descriptor);
	}

	ref<ShaderModule> Renderer::loadShaderModule(std::string_view path, const ShaderModuleDescriptor& descriptor) {
		std::string pathString = "Asset/Shader/";
		pathString += path;

		if (
			graphicsApi == GraphicsAPI::VK
			#ifdef GL_SOURCE_FROM_SPIRV
				|| graphicsApi == GraphicsAPI::GL
			#endif
		) {
			pathString += ".spv";
		}

		path = pathString;

		const sequence<uint8> code = Filesystem::openByteBuffer(path.data());

		return adapter->createShaderModule(code, descriptor);
	}

	void Renderer::setImageIndex(uint32 imageIndex) {
		Renderer::imageIndex = imageIndex;
	}
}