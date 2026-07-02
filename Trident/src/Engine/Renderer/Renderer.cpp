// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	const LayerCompositor* Renderer::layerCompositor;

	GraphicsAPI Renderer::getGraphicsApi() {
		return layerCompositor->adapter->getApi();
	}

	void Renderer::initialize(const LayerCompositor* layerCompositor) {
		Renderer::layerCompositor = layerCompositor;
	}

	void Renderer::resize(WindowResizeEvent& event) {}

	void Renderer::onEvent(Event& event) {}

	ref<Buffer> Renderer::createDynamicBuffer(const BufferDescriptor& descriptor) {
		return layerCompositor->device->createDynamicBuffer(descriptor);
	}

	ref<ComputePipeline> Renderer::createComputePipeline(const ComputePipelineDescriptor& descriptor) {
		return layerCompositor->device->createComputePipeline(descriptor);
	}

	ref<DescriptorSetLayout> Renderer::createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) {
		return layerCompositor->device->createDescriptorSetLayout(bindings);
	}

	ref<Fence> Renderer::createFence(const FenceDescriptor& descriptor) {
		return layerCompositor->device->createFence(descriptor);
	}

	ref<Framebuffer> Renderer::createFramebuffer(const FramebufferDescriptor& descriptor) {
		return layerCompositor->device->createFramebuffer(descriptor);
	}

	ref<Image> Renderer::createDynamicImage(const ImageDescriptor& descriptor) {
		return layerCompositor->device->createDynamicImage(descriptor);
	}

	ref<Image> Renderer::createStaticImage(const ImageDescriptor& descriptor) {
		return layerCompositor->device->createStaticImage(descriptor);
	}

	ref<PipelineLayout> Renderer::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) {
		return layerCompositor->device->createPipelineLayout(descriptor);
	}

	ref<RenderPass> Renderer::createRenderPass(const RenderPassDescriptor& descriptor) {
		return layerCompositor->device->createRenderPass(layerCompositor->surface, descriptor);
	}

	ref<GraphicsPipeline> Renderer::createGraphicsPipeline(const GraphicsPipelineDescriptor& descriptor) {
		return layerCompositor->device->createGraphicsPipeline(descriptor);
	}

	ref<Sampler> Renderer::createSampler(const SamplerDescriptor& descriptor) {
		return layerCompositor->device->createSampler(descriptor);
	}

	ref<Semaphore> Renderer::createSemaphore() {
		return layerCompositor->device->createSemaphore();
	}

	ref<Buffer> Renderer::createStaticBuffer(const BufferDescriptor& descriptor) {
		return layerCompositor->device->createStaticBuffer(descriptor);
	}

	ref<Device> Renderer::getDevice() {
		return layerCompositor->device;
	}

	uint32 Renderer::getSwapchainImageIndex() {
		return layerCompositor->swapchainImageIndex;
	}

	ref<Surface> Renderer::getSurface() {
		return layerCompositor->surface;
	}

	ref<Swapchain> Renderer::getSwapchain() {
		return layerCompositor->swapchain;
	}

	ref<ShaderModule> Renderer::loadShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) {
		return layerCompositor->device->createShaderModule(descriptor, asset);
	}

	ref<ShaderModule> Renderer::loadShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path) {
		std::string pathString = "Asset/Shader/";
		pathString += path;

		switch (layerCompositor->adapter->getApi()) {
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

		return layerCompositor->device->createShaderModule(descriptor, view<char8>(&code[0], code.count()));
	}

	ref<ShaderModule> Renderer::loadInternalShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path) {
		return loadShaderModule(descriptor, Logger::format("../../../../../Trident/src/Asset/Shader/[]", path));
	}
}