// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/DX12/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/Fence.hpp"
#include "Engine/Adapter/Graphics/DX12/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/DX12/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/DX12/GraphicsPipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/Adapter/Graphics/DX12/ImageView.hpp"
#include "Engine/Adapter/Graphics/DX12/Instance.hpp"
#include "Engine/Adapter/Graphics/DX12/MemoryAllocator.hpp"
#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/DX12/Queue.hpp"
#include "Engine/Adapter/Graphics/DX12/Sampler.hpp"
#include "Engine/Adapter/Graphics/DX12/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/DX12/Surface.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"

namespace Trident::DX12 {
	static ref<DescriptorSetLayout> createDescriptorSetLayout(const Trident::Device* device, const sequence<DescriptorSetLayoutBinding>& bindings) {
		return createRef<DescriptorSetLayout>(device, bindings);
	}

	static ref<Trident::MemoryAllocator> createMemoryAllocator(const Trident::Device* device) {
		return createRef<DX12::MemoryAllocator>(static_cast<const DX12::Device*>(device));
	}

	static ref<RenderPass> createRenderPass(const Trident::Device* device, ref<Trident::Surface> surface, const RenderPassDescriptor& descriptor) {
		return createRef<RenderPass>(device, descriptor);
	}

	static ref<Semaphore> createSemaphore(const Trident::Device* device) {
		return createRef<Semaphore>(device);
	}

	static void destroyDescriptorSetLayout(const Trident::Device* device, const DescriptorSetLayout* descriptorSetLayout) {}

	static void destroySemaphore(const Trident::Device* device, const Semaphore* semaphore) {}

	static void destroyRenderPass(const Trident::Device* device, const RenderPass* renderPass) {}

	void bindAdapter(Trident::GraphicsAdapter* adapter) {
		adapter->logger = ConsoleLogger(std::cout, LogLevel::ALL, "DIRECTX 12");

		adapter->acquireNextImageImplementation = DX12::acquireNextImage;
		adapter->allocateDescriptorSetsImplementation = DX12::allocateDescriptorSets;
		adapter->beginImplementation = DX12::begin;
		adapter->beginRenderPassImplementation = DX12::beginRenderPass;
		adapter->bindComputePipelineImplementation = DX12::bindComputePipeline;
		adapter->bindDescriptorSetsImplementation = DX12::bindDescriptorSets;
		adapter->bindGraphicsPipelineImplementation = DX12::bindGraphicsPipeline;
		adapter->bindIndexBufferImplementation = DX12::bindIndexBuffer;
		adapter->bindVertexBufferImplementation = DX12::bindVertexBuffer;
		adapter->copyBufferToBufferImplementation = DX12::copyBufferToBuffer;
		adapter->copyBufferToImageImplementation = DX12::copyBufferToImage;
		adapter->copyImageToBufferImplementation = DX12::copyImageToBuffer;
		adapter->createBufferImplementation = DX12::createBuffer;
		adapter->createCommandEncoderImplementation = DX12::createCommandEncoder;
		adapter->createCommandPoolImplementation = DX12::createCommandPool;
		adapter->createComputePipelineImplementation = DX12::createComputePipeline;
		adapter->createDescriptorSetLayoutImplementation = DX12::createDescriptorSetLayout;
		adapter->createDeviceImplementation = DX12::createDevice;
		adapter->createFenceImplementation = DX12::createFence;
		adapter->createFramebufferImplementation = DX12::createFramebuffer;
		adapter->createGraphicsPipelineImplementation = DX12::createGraphicsPipeline;
		adapter->createImageImplementation = DX12::createImage;
		adapter->createImageViewImplementation = DX12::createImageView;
		adapter->createInstanceImplementation = DX12::createInstance;
		adapter->createMemoryAllocatorImplementation = DX12::createMemoryAllocator;
		adapter->createPipelineLayoutImplementation = DX12::createPipelineLayout;
		adapter->createRenderPassImplementation = DX12::createRenderPass;
		adapter->createSamplerImplementation = DX12::createSampler;
		adapter->createSemaphoreImplementation = DX12::createSemaphore;
		adapter->createShaderModuleFromAssetImplementation = DX12::createShaderModule;
		adapter->createShaderModuleFromCodeImplementation = DX12::createShaderModule;
		adapter->createSurfaceImplementation = DX12::createSurface;
		adapter->createSwapchainImplementation = DX12::createSwapchain;
		adapter->destroyBufferImplementation = DX12::destroyBuffer;
		adapter->destroyCommandEncoderImplementation = DX12::destroyCommandEncoder;
		adapter->destroyCommandPoolImplementation = DX12::destroyCommandPool;
		adapter->destroyComputePipelineImplementation = DX12::destroyComputePipeline;
		adapter->destroyDescriptorSetLayoutImplementation = DX12::destroyDescriptorSetLayout;
		adapter->destroyDeviceImplementation = DX12::destroyDevice;
		adapter->destroyFenceImplementation = DX12::destroyFence;
		adapter->destroyFramebufferImplementation = DX12::destroyFramebuffer;
		adapter->destroyGraphicsPipelineImplementation = DX12::destroyGraphicsPipeline;
		adapter->destroyPipelineLayoutImplementation = DX12::destroyPipelineLayout;
		adapter->destroyImageImplementation = DX12::destroyImage;
		adapter->destroyImageViewImplementation = DX12::destroyImageView;
		adapter->destroyInstanceImplementation = DX12::destroyInstance;
		adapter->destroyQueueImplementation = DX12::destroyQueue;
		adapter->destroyRenderPassImplementation = DX12::destroyRenderPass;
		adapter->destroySamplerImplementation = DX12::destroySampler;
		adapter->destroySemaphoreImplementation = DX12::destroySemaphore;
		adapter->destroyShaderModuleImplementation = DX12::destroyShaderModule;
		adapter->destroySurfaceImplementation = DX12::destroySurface;
		adapter->destroySwapchainImplementation = DX12::destroySwapchain;
		adapter->deviceWaitIdleImplementation = DX12::waitIdle;
		adapter->dispatchImplementation = DX12::dispatch;
		adapter->dispatchIndirectImplementation = DX12::dispatchIndirect;
		adapter->drawImplementation = DX12::draw;
		adapter->drawIndirectImplementation = DX12::drawIndirect;
		adapter->drawIndexedImplementation = DX12::drawIndexed;
		adapter->drawIndexedIndirectImplementation = DX12::drawIndexedIndirect;
		adapter->endImplementation = DX12::end;
		adapter->endRenderPassImplementation = DX12::endRenderPass;
		adapter->getPhysicalDeviceNameImplementation = DX12::getPhysicalDeviceName;
		adapter->getQueueImplementation = DX12::getQueue;
		adapter->insertMemoryBarriersImplementation = DX12::insertMemoryBarriers;
		adapter->mapBufferImplementation = DX12::mapBuffer;
		adapter->nextSubpassImplementation = DX12::nextSubpass;
		adapter->presentImplementation = DX12::present;
		adapter->queueWaitIdleImplementation = DX12::waitIdle;
		adapter->resetImplementation = DX12::reset;
		adapter->resetFenceImplementation = DX12::resetFence;
		adapter->selectPhysicalDeviceImplementation = DX12::selectPhysicalDevice;
		adapter->setLineWidthImplementation = DX12::setLineWidth;
		adapter->setScissorImplementation = DX12::setScissor;
		adapter->setViewportImplementation = DX12::setViewport;
		adapter->submitImplementation = DX12::submit;
		adapter->submit2Implementation = DX12::submit;
		adapter->unmapBufferImplementation = DX12::unmapBuffer;
		adapter->updateDescriptorSetsImplementation = DX12::updateDescriptorSets;
		adapter->updateSwapchainImplementation = DX12::recreateSwapchain;
		adapter->waitForFenceImplementation = DX12::waitForFence;
	}
}