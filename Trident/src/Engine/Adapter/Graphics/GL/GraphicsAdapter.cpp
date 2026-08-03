// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Buffer.hpp"
#include "Engine/Adapter/Graphics/GL/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/GL/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/GL/Fence.hpp"
#include "Engine/Adapter/Graphics/GL/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/GL/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/GL/GraphicsPipeline.hpp"
#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/ImageView.hpp"
#include "Engine/Adapter/Graphics/GL/MemoryAllocator.hpp"
#include "Engine/Adapter/Graphics/GL/RenderPass.hpp"
#include "Engine/Adapter/Graphics/GL/Sampler.hpp"
#include "Engine/Adapter/Graphics/GL/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/GL/Swapchain.hpp"
#include "Engine/API/Graphics/CommandPool.hpp"
#include "Engine/API/Graphics/MemoryAllocator.hpp"

namespace Trident::GL {
	static ref<CommandPool> createCommandPool(const Trident::Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) {
		return createRef<CommandPool>(device);
	}

	static ref<DescriptorSetLayout> createDescriptorSetLayout(const Trident::Device* device, view<DescriptorSetLayoutBinding> bindings) {
		return createRef<DescriptorSetLayout>(device, bindings);
	}

	static ref<Trident::MemoryAllocator> createMemoryAllocator(const Trident::Device* device) {
		return createRef<MemoryAllocator>(device);
	}

	static ref<PipelineLayout> createPipelineLayout(const Trident::Device* device, const PipelineLayoutDescriptor& descriptor) {
		return createRef<PipelineLayout>(device);
	}

	static ref<Semaphore> createSemaphore(const Trident::Device* device) {
		return createRef<Semaphore>(device);
	}

	static void destroyCommandPool(const Trident::Device* device, const CommandPool* commandPool) {}

	static void destroyDescriptorSetLayout(const Trident::Device* device, const DescriptorSetLayout* descriptorSetLayout) {}

	static void destroyPipelineLayout(const Trident::Device* device, const PipelineLayout* pipelineLayout) {}

	static void destroySemaphore(const Trident::Device* device, const Semaphore* semaphore) {}

	void bindAdapter(Trident::GraphicsAdapter* adapter) {
		adapter->logger = ConsoleLogger(std::cout, LogLevel::ALL, "OPENGL");

		adapter->acquireNextImageImplementation = GL::acquireNextImage;
		adapter->allocateDescriptorSetsImplementation = GL::allocateDescriptorSets;
		adapter->allocateMemoryImplementation = GL::allocateMemory;
		adapter->beginImplementation = GL::begin;
		adapter->beginRenderPassImplementation = GL::beginRenderPass;
		adapter->bindComputePipelineImplementation = GL::bindComputePipeline;
		adapter->bindDescriptorSetsImplementation = GL::bindDescriptorSets;
		adapter->bindGraphicsPipelineImplementation = GL::bindGraphicsPipeline;
		adapter->bindIndexBufferImplementation = GL::bindIndexBuffer;
		adapter->bindVertexBufferImplementation = GL::bindVertexBuffer;
		adapter->copyBufferToBufferImplementation = GL::copyBufferToBuffer;
		adapter->copyBufferToImageImplementation = GL::copyBufferToImage;
		adapter->copyImageToBufferImplementation = GL::copyImageToBuffer;
		adapter->createBufferImplementation = GL::createBuffer;
		adapter->createCommandEncoderImplementation = GL::createCommandEncoder;
		adapter->createCommandPoolImplementation = GL::createCommandPool;
		adapter->createComputePipelineImplementation = GL::createComputePipeline;
		adapter->createDescriptorSetLayoutImplementation = GL::createDescriptorSetLayout;
		adapter->createDeviceImplementation = GL::createDevice;
		adapter->createFenceImplementation = GL::createFence;
		adapter->createFramebufferImplementation = GL::createFramebuffer;
		adapter->createGraphicsPipelineImplementation = GL::createGraphicsPipeline;
		adapter->createImageImplementation = GL::createImage;
		adapter->createImageViewImplementation = GL::createImageView;
		adapter->createInstanceImplementation = GL::createInstance;
		adapter->createMemoryAllocatorImplementation = GL::createMemoryAllocator;
		adapter->createPipelineLayoutImplementation = GL::createPipelineLayout;
		adapter->createRenderPassImplementation = GL::createRenderPass;
		adapter->createSamplerImplementation = GL::createSampler;
		adapter->createSemaphoreImplementation = GL::createSemaphore;
		adapter->createShaderModuleFromAssetImplementation = GL::createShaderModule;
		adapter->createShaderModuleFromCodeImplementation = GL::createShaderModule;
		adapter->createSurfaceImplementation = GL::createSurface;
		adapter->createSwapchainImplementation = GL::createSwapchain;
		adapter->destroyBufferImplementation = GL::destroyBuffer;
		adapter->destroyCommandEncoderImplementation = GL::destroyCommandEncoder;
		adapter->destroyCommandPoolImplementation = GL::destroyCommandPool;
		adapter->destroyComputePipelineImplementation = GL::destroyComputePipeline;
		adapter->destroyDescriptorSetLayoutImplementation = GL::destroyDescriptorSetLayout;
		adapter->destroyDeviceImplementation = GL::destroyDevice;
		adapter->destroyFenceImplementation = GL::destroyFence;
		adapter->destroyFramebufferImplementation = GL::destroyFramebuffer;
		adapter->destroyGraphicsPipelineImplementation = GL::destroyGraphicsPipeline;
		adapter->destroyPipelineLayoutImplementation = GL::destroyPipelineLayout;
		adapter->destroyImageImplementation = GL::destroyImage;
		adapter->destroyImageViewImplementation = GL::destroyImageView;
		adapter->destroyInstanceImplementation = GL::destroyInstance;
		adapter->destroyQueueImplementation = GL::destroyQueue;
		adapter->destroyRenderPassImplementation = GL::destroyRenderPass;
		adapter->destroySamplerImplementation = GL::destroySampler;
		adapter->destroySemaphoreImplementation = GL::destroySemaphore;
		adapter->destroyShaderModuleImplementation = GL::destroyShaderModule;
		adapter->destroySurfaceImplementation = GL::destroySurface;
		adapter->destroySwapchainImplementation = GL::destroySwapchain;
		adapter->deviceWaitIdleImplementation = GL::waitIdle;
		adapter->dispatchImplementation = GL::dispatch;
		adapter->dispatchIndirectImplementation = GL::dispatchIndirect;
		adapter->drawImplementation = GL::draw;
		adapter->drawIndirectImplementation = GL::drawIndirect;
		adapter->drawIndexedImplementation = GL::drawIndexed;
		adapter->drawIndexedIndirectImplementation = GL::drawIndexedIndirect;
		adapter->endImplementation = GL::end;
		adapter->endRenderPassImplementation = GL::endRenderPass;
		adapter->findMemoryTypeIndexImplementation = GL::findMemoryTypeIndex;
		adapter->getBufferMemoryRequirementsImplementation = GL::getBufferMemoryRequirements;
		adapter->getImageMemoryRequirementsImplementation = GL::getImageMemoryRequirements;
		adapter->getPhysicalDeviceMemoryTypeCountImplementation = GL::getPhysicalDeviceMemoryTypeCountImplementation;
		adapter->getPhysicalDeviceNameImplementation = GL::getPhysicalDeviceName;
		adapter->getQueueImplementation = GL::getQueue;
		adapter->insertMemoryBarriersImplementation = GL::insertMemoryBarriers;
		adapter->mapBufferImplementation = GL::mapBuffer;
		adapter->nextSubpassImplementation = GL::nextSubpass;
		adapter->presentImplementation = GL::present;
		adapter->queueWaitIdleImplementation = GL::waitIdle;
		adapter->resetImplementation = GL::reset;
		adapter->resetFenceImplementation = GL::resetFence;
		adapter->selectPhysicalDeviceImplementation = GL::selectPhysicalDevice;
		adapter->setLineWidthImplementation = GL::setLineWidth;
		adapter->setScissorImplementation = GL::setScissor;
		adapter->setViewportImplementation = GL::setViewport;
		adapter->suballocateBufferMemoryImplementation = GL::suballocateBufferMemory;
		adapter->suballocateImageMemoryImplementation = GL::suballocateImageMemory;
		adapter->submitImplementation = GL::submit;
		adapter->submit2Implementation = GL::submit;
		adapter->unmapBufferImplementation = GL::unmapBuffer;
		adapter->updateDescriptorSetsImplementation = GL::updateDescriptorSets;
		adapter->updateSwapchainImplementation = GL::recreateSwapchain;
		adapter->waitForFenceImplementation = GL::waitForFence;
	}
}