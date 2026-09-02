// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/VK/CommandPool.hpp"
#include "Engine/Adapter/Graphics/VK/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/VK/DescriptorSetLayout.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/VK/DeviceExtension/EXT_hardware_ray_tracing/Device.hpp"
#include "Engine/Adapter/Graphics/VK/Fence.hpp"
#include "Engine/Adapter/Graphics/VK/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/VK/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/VK/GraphicsPipeline.hpp"
#include "Engine/Adapter/Graphics/VK/Image.hpp"
#include "Engine/Adapter/Graphics/VK/ImageView.hpp"
#include "Engine/Adapter/Graphics/VK/Instance.hpp"
#include "Engine/Adapter/Graphics/VK/MemoryAllocator.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/VK/Queue.hpp"
#include "Engine/Adapter/Graphics/VK/RenderPass.hpp"
#include "Engine/Adapter/Graphics/VK/Sampler.hpp"
#include "Engine/Adapter/Graphics/VK/Semaphore.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/VK/Surface.hpp"
#include "Engine/Adapter/Graphics/VK/Swapchain.hpp"

namespace Trident::VK {
	static ref<Trident::MemoryAllocator> createMemoryAllocator(const Trident::Device* device) {
		return allocateRef<VK::MemoryAllocator>(device);
	}

	void bindAdapter(Trident::GraphicsAdapter* adapter) {
		adapter->logger = ConsoleLogger(std::cout, LogLevel::ALL, "VULKAN");

		adapter->acquireNextImageImplementation = VK::acquireNextImage;
		adapter->allocateDescriptorSetsImplementation = VK::allocateDescriptorSets;
		adapter->beginImplementation = VK::begin;
		adapter->beginRenderPassImplementation = VK::beginRenderPass;
		adapter->bindComputePipelineImplementation = VK::bindComputePipeline;
		adapter->bindDescriptorSetsImplementation = VK::bindDescriptorSets;
		adapter->bindGraphicsPipelineImplementation = VK::bindGraphicsPipeline;
		adapter->bindIndexBufferImplementation = VK::bindIndexBuffer;
		adapter->bindVertexBufferImplementation = VK::bindVertexBuffer;
		adapter->copyBufferToBufferImplementation = VK::copyBufferToBuffer;
		adapter->copyBufferToImageImplementation = VK::copyBufferToImage;
		adapter->copyImageToBufferImplementation = VK::copyImageToBuffer;
		adapter->createBufferImplementation = VK::createBuffer;
		adapter->createCommandEncoderImplementation = VK::createCommandEncoder;
		adapter->createCommandPoolImplementation = VK::createCommandPool;
		adapter->createComputePipelineImplementation = VK::createComputePipeline;
		adapter->createDescriptorSetLayoutImplementation = VK::createDescriptorSetLayout;
		adapter->createDeviceImplementation = VK::createDevice;
		adapter->createFenceImplementation = VK::createFence;
		adapter->createFramebufferImplementation = VK::createFramebuffer;
		adapter->createGraphicsPipelineImplementation = VK::createGraphicsPipeline;
		adapter->createImageImplementation = VK::createImage;
		adapter->createImageViewImplementation = VK::createImageView;
		adapter->createInstanceImplementation = VK::createInstance;
		adapter->allocateMemoryImplementation = VK::allocateMemory;
		adapter->createMemoryAllocatorImplementation = VK::createMemoryAllocator;
		adapter->createPipelineLayoutImplementation = VK::createPipelineLayout;
		adapter->createRenderPassImplementation = VK::createRenderPass;
		adapter->createSamplerImplementation = VK::createSampler;
		adapter->createSemaphoreImplementation = VK::createSemaphore;
		adapter->createShaderModuleFromAssetImplementation = VK::createShaderModule;
		adapter->createShaderModuleFromCodeImplementation = VK::createShaderModule;
		adapter->createSurfaceImplementation = VK::createSurface;
		adapter->createSwapchainImplementation = VK::createSwapchain;
		adapter->destroyBufferImplementation = VK::destroyBuffer;
		adapter->destroyCommandEncoderImplementation = VK::destroyCommandEncoder;
		adapter->destroyCommandPoolImplementation = VK::destroyCommandPool;
		adapter->destroyComputePipelineImplementation = VK::destroyComputePipeline;
		adapter->destroyDescriptorSetLayoutImplementation = VK::destroyDescriptorSetLayout;
		adapter->destroyDeviceImplementation = VK::destroyDevice;
		adapter->destroyFenceImplementation = VK::destroyFence;
		adapter->destroyFramebufferImplementation = VK::destroyFramebuffer;
		adapter->destroyGraphicsPipelineImplementation = VK::destroyGraphicsPipeline;
		adapter->destroyPipelineLayoutImplementation = VK::destroyPipelineLayout;
		adapter->destroyImageImplementation = VK::destroyImage;
		adapter->destroyImageViewImplementation = VK::destroyImageView;
		adapter->destroyInstanceImplementation = VK::destroyInstance;
		adapter->destroyQueueImplementation = VK::destroyQueue;
		adapter->destroyRenderPassImplementation = VK::destroyRenderPass;
		adapter->destroySamplerImplementation = VK::destroySampler;
		adapter->destroySemaphoreImplementation = VK::destroySemaphore;
		adapter->destroyShaderModuleImplementation = VK::destroyShaderModule;
		adapter->destroySurfaceImplementation = VK::destroySurface;
		adapter->destroySwapchainImplementation = VK::destroySwapchain;
		adapter->deviceWaitIdleImplementation = VK::waitIdle;
		adapter->dispatchImplementation = VK::dispatch;
		adapter->dispatchIndirectImplementation = VK::dispatchIndirect;
		adapter->drawImplementation = VK::draw;
		adapter->drawIndirectImplementation = VK::drawIndirect;
		adapter->drawIndexedImplementation = VK::drawIndexed;
		adapter->drawIndexedIndirectImplementation = VK::drawIndexedIndirect;
		adapter->endImplementation = VK::end;
		adapter->endRenderPassImplementation = VK::endRenderPass;
		adapter->findMemoryTypeIndexImplementation = VK::findMemoryTypeIndex;
		adapter->getBufferMemoryRequirementsImplementation = VK::getBufferMemoryRequirements;
		adapter->getImageMemoryRequirementsImplementation = VK::getImageMemoryRequirements;
		adapter->getPhysicalDeviceMemoryTypeCountImplementation = VK::getPhysicalDeviceMemoryTypeCountImplementation;
		adapter->getPhysicalDeviceNameImplementation = VK::getPhysicalDeviceName;
		adapter->insertMemoryBarriersImplementation = VK::insertMemoryBarriers;
		adapter->mapBufferImplementation = VK::mapBuffer;
		adapter->nextSubpassImplementation = VK::nextSubpass;
		adapter->presentImplementation = VK::present;
		adapter->queueWaitIdleImplementation = VK::waitIdle;
		adapter->resetImplementation = VK::reset;
		adapter->resetFenceImplementation = VK::resetFence;
		adapter->selectPhysicalDeviceImplementation = VK::selectPhysicalDevice;
		adapter->setLineWidthImplementation = VK::setLineWidth;
		adapter->setScissorImplementation = VK::setScissor;
		adapter->setViewportImplementation = VK::setViewport;
		adapter->suballocateBufferMemoryImplementation = VK::suballocateBufferMemory;
		adapter->suballocateImageMemoryImplementation = VK::suballocateImageMemory;
		adapter->submitImplementation = VK::submit;
		adapter->submit2Implementation = VK::submit;
		adapter->unmapBufferImplementation = VK::unmapBuffer;
		adapter->updateDescriptorSetsImplementation = VK::updateDescriptorSets;
		adapter->updateSwapchainImplementation = VK::recreateSwapchain;
		adapter->waitForFenceImplementation = VK::waitForFence;

		// EXT_hardware_ray_tracing
		adapter->buildAccelerationStructuresImplementation = VK::buildAccelerationStructures;
		adapter->createAccelerationStructureBuildTriangleGeometryImplementation = VK::createAccelerationStructureBuildTriangleGeometry;
		adapter->createAccelerationStructureImplementation = VK::createAccelerationStructure;
		adapter->getAccelerationStructureBuildSizesImplementation = VK::getAccelerationStructureBuildSizes;
	}
}