// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#if defined(ENABLE_DX12_ADAPTER)
	#include "Engine/Adapter/Graphics/DX12/GraphicsAdapter.hpp"
#endif

#if defined(ENABLE_GL_ADAPTER)
	#include "Engine/Adapter/Graphics/GL/GraphicsAdapter.hpp"
#endif

#if defined(ENABLE_VK_ADAPTER)
	#include "Engine/Adapter/Graphics/VK/GraphicsAdapter.hpp"
#endif

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	const GraphicsAdapter* Renderer::adapter;

	GraphicsAPI Renderer::getGraphicsApi() {
		return adapter->getApi();
	}

	void Renderer::initialize(const GraphicsAdapter* adapter, const ProjectDescriptor& projectDescriptor, const Window& window, const ApplicationProperties& properties) {
		Renderer::adapter = adapter;
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

		SET_DEBUG_NAME(stagingBufferDescriptor, "Staging buffer");

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

	ref<Device> Renderer::getDevice() {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
		// return adapter->getDevice();
	}

	uint32 Renderer::getSwapchainImageIndex() {
		ENGINE_ERROR_UNIMPLEMENTED();

		return 0;
		// return adapter->getSwapchainImageIndex();
	}

	ref<Surface> Renderer::getSurface() {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
		// return adapter->getSurface();
	}

	ref<Swapchain> Renderer::getSwapchain() {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
		// return adapter->getSwapchain();
	}

	ref<ShaderModule> Renderer::loadShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) {
		return adapter->createShaderModule(descriptor, asset);
	}

	ref<ShaderModule> Renderer::loadShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path) {
		std::string pathString = "Asset/Shader/";
		pathString += path;

		switch (adapter->getApi()) {
			case GraphicsAPI::DX12: {
				pathString += ".hlsl";

				break;
			}
			case GraphicsAPI::GL: {
				#if defined(GL_SOURCE_FROM_SPIRV)
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

	ref<ShaderModule> Renderer::loadInternalShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path) {
		return loadShaderModule(descriptor, Logger::format("../../../../../Trident/src/Asset/Shader/[]", path));
	}
}