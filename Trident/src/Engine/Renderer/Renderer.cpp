// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

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

namespace Trident {
	GraphicsAPI Renderer::graphicsApi = GraphicsAPI::CPU;
	scope<GraphicsAdapter> Renderer::adapter = nullptr;
	uint32 Renderer::imageIndex = 0;
	ref<Swapchain> Renderer::swapchain = nullptr;
	sequence<ref<CommandEncoder>> Renderer::commandEncoders;
	sequence<ref<Fence>> Renderer::flightFences;
	sequence<ref<Semaphore>> Renderer::acquireNextImageSemaphores;
	sequence<ref<Semaphore>> Renderer::finishedRenderSemaphores;

	GraphicsAPI Renderer::getGraphicsApi() {
		return graphicsApi;
	}

	void Renderer::initialize(const ProjectDescriptor& projectDescriptor, const Window& window, const ApplicationProperties& properties) {
		switch (properties.GraphicsAPI) {
			case GraphicsAPI::CPU: {
				ENGINE_ERROR_UNSUPPORTED();

				break;
			}
			#ifdef ENABLE_DX12_ADAPTER
				case GraphicsAPI::DX12: {
					ENGINE_INFO("Using DirectX 12 graphics adapter.");

					adapter = createScope<DX12::GraphicsAdapter>(projectDescriptor, window, properties);

					break;
				}
			#endif
			#ifdef ENABLE_GL_ADAPTER
				case GraphicsAPI::GL: {
					ENGINE_INFO("Using OpenGL [] graphics adapter.", OPENGL_VERSION);

					adapter = createScope<GL::GraphicsAdapter>(projectDescriptor, window, properties);

					break;
				}
			#endif
			#ifdef ENABLE_VK_ADAPTER
				case GraphicsAPI::VK: {
					ENGINE_INFO("Using Vulkan graphics adapter.");

					adapter = createScope<VK::GraphicsAdapter>(projectDescriptor, window, properties);

					break;
				}
			#endif
		}

		DEBUG_ASSERT(adapter != nullptr);

		graphicsApi = properties.GraphicsAPI;

		SwapchainDescriptor swapchainDescriptor{};
		swapchainDescriptor.extent.width = window.getSize()[0];
		swapchainDescriptor.extent.height = window.getSize()[1];
		swapchainDescriptor.vSync = properties.VerticalSync;

		swapchain = adapter->createSwapchain(swapchainDescriptor);

		commandEncoders = sequence<ref<CommandEncoder>>(Application::MAX_FRAMES_IN_FLIGHT);
		flightFences = sequence<ref<Fence>>(Application::MAX_FRAMES_IN_FLIGHT);
		acquireNextImageSemaphores = sequence<ref<Semaphore>>(Application::MAX_FRAMES_IN_FLIGHT);
		finishedRenderSemaphores = sequence<ref<Semaphore>>(Application::MAX_FRAMES_IN_FLIGHT);

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::AUTO;

		const ref<Queue> graphicsQueue = adapter->getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);

		FenceDescriptor flightFenceDescriptor{};
		flightFenceDescriptor.signaled = true;

		uint32 frameInFlightIndex = 0;

		while (frameInFlightIndex < Application::MAX_FRAMES_IN_FLIGHT) {
			commandEncoders[frameInFlightIndex] = graphicsQueue->createCommandEncoder(commandBufferDescriptor);
			flightFences[frameInFlightIndex] = adapter->createFence(flightFenceDescriptor);
			acquireNextImageSemaphores[frameInFlightIndex] = adapter->createSemaphore();
			finishedRenderSemaphores[frameInFlightIndex] = adapter->createSemaphore();

			frameInFlightIndex += 1;
		}
	}

	void Renderer::deinitialize() {
		uint32 frameInFlightIndex = 0;

		while (frameInFlightIndex < Application::MAX_FRAMES_IN_FLIGHT) {
			finishedRenderSemaphores[frameInFlightIndex].~shared_ptr();
			acquireNextImageSemaphores[frameInFlightIndex].~shared_ptr();
			flightFences[frameInFlightIndex].~shared_ptr();
			commandEncoders[frameInFlightIndex].~shared_ptr();

			frameInFlightIndex += 1;
		}

		swapchain.~shared_ptr();

		adapter->~GraphicsAdapter();
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

	ref<Fence> Renderer::createFence(const FenceDescriptor& descriptor) {
		return adapter->createFence(descriptor);
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

	ref<Semaphore> Renderer::getAcquireNextImageSemaphore(uint32 frameInFlightIndex) {
		return acquireNextImageSemaphores[frameInFlightIndex];
	}

	ref<CommandEncoder> Renderer::getCommandEncoder(uint32 frameInFlightIndex) {
		return commandEncoders[frameInFlightIndex];
	}

	ref<Device> Renderer::getDevice() {
		return adapter->getDevice();
	}

	ref<Semaphore> Renderer::getFinishedRenderSemaphore(uint32 frameInFlightIndex) {
		return finishedRenderSemaphores[frameInFlightIndex];
	}

	ref<Fence> Renderer::getFlightFence(uint32 frameInFlightIndex) {
		return flightFences[frameInFlightIndex];
	}

	uint32 Renderer::getImageIndex() {
		return imageIndex;
	}

	ref<Surface> Renderer::getSurface() {
		return adapter->getSurface();
	}

	ref<Swapchain> Renderer::getSwapchain() {
		return swapchain;
	}

	ref<ShaderModule> Renderer::loadShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) {
		return adapter->createShaderModule(descriptor, asset);
	}

	ref<ShaderModule> Renderer::loadShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path) {
		std::string pathString = "Asset/Shader/";
		pathString += path;

		switch (graphicsApi) {
			case GraphicsAPI::DX12: {
				pathString += ".hlsl";

				break;
			}
			case GraphicsAPI::GL: {
				#ifdef GL_SOURCE_FROM_SPIRV
					pathString += ".spv";
				#else
					pathString += ".glsl";
				#endif

				break;
			}
			case GraphicsAPI::VK: {
				pathString += ".spv";

				break;
			}
			default: {
				ENGINE_ERROR_UNSUPPORTED();

				break;
			}
		}

		path = view<char8>(&pathString[0], pathString.size());

		const sequence<char8> code = Filesystem::openTextBuffer(&path[0]);

		return adapter->createShaderModule(descriptor, view<char8>(&code[0], code.count()));
	}

	void Renderer::setImageIndex(uint32 imageIndex) {
		Renderer::imageIndex = imageIndex;
	}
}