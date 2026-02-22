// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

#include "Engine/Adapter/Graphics/DX12/GraphicsAdapter.hpp"
// #include "Engine/Adapter/Graphics/GL/bindAdapter.hpp"
// #include "Engine/Adapter/Graphics/VK/bindAdapter.hpp"

#include "Engine/API/Rendering/Instance.hpp"

namespace Trident {
	ConsoleLogger GraphicsAdapter::logger(std::cout, LogLevel::ALL, "");

	GraphicsAdapter::GraphicsAdapter(const ApplicationProperties& properties) :
		api(properties.GraphicsAPI)
	{
		DEBUG_ASSERT(properties.GraphicsAPI < GraphicsAPI::count);

		static constexpr BindAdapterImplementation BIND_ADAPTER_IMPLEMENTATIONS[static_cast<index>(GraphicsAPI::count)] = {
			DX12::bindAdapter,
			// GL::bindAdapter,
			nullptr,
			// VK::bindAdapter,
			nullptr,
		};

		BIND_ADAPTER_IMPLEMENTATIONS[static_cast<index>(properties.GraphicsAPI)](this);
	}

	GraphicsAPI GraphicsAdapter::getApi() const {
		return api;
	}

	AcquireNextImageStatus GraphicsAdapter::acquireNextImage(const Device* device, const Swapchain* swapchain, const Semaphore* semaphore, uint32& imageIndex) const {
		return acquireNextImageImplementation(device, swapchain, semaphore, imageIndex);
	}

	sequence<ref<DescriptorSet>> GraphicsAdapter::allocateDescriptorSets(const Device* device, view<ref<DescriptorSetLayout>> descriptorSetLayouts) const {
		return allocateDescriptorSetsImplementation(device, descriptorSetLayouts);
	}

	ref<Buffer> GraphicsAdapter::createBuffer(const BufferDescriptor& descriptor, const Device* device) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		// TODO
		BREAKPOINT();

		return nullptr;
		// return createBufferImplementation(descriptor, 1, device);
	}

	ref<CommandEncoder> GraphicsAdapter::createCommandEncoder(const Device* device, const CommandPool* commandPool, const CommandBufferDescriptor& descriptor) const {
		return createCommandEncoderImplementation(device, commandPool, descriptor);
	}

	ref<CommandPool> GraphicsAdapter::createCommandPool(const Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) const {
		return createCommandPoolImplementation(device, queueFamilyIndex, queueFamilyCapability);
	}

	ref<ComputePipeline> GraphicsAdapter::createComputePipeline(const ComputePipelineDescriptor& descriptor, const Device* device) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createComputePipelineImplementation(descriptor, device);
	}

	ref<DescriptorSetLayout> GraphicsAdapter::createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings, const Device* device) const {
		return createDescriptorSetLayoutImplementation(bindings, device);
	}

	ref<Device> GraphicsAdapter::createDevice(const PhysicalDevice* physicalDevice) const {
		ref<Device> device = createDeviceImplementation(this, physicalDevice);
		device->memoryAllocator = createMemoryAllocatorImplementation(device.get());

		return device;
	}

	ref<Fence> GraphicsAdapter::createFence(const Device* device, const FenceDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createFenceImplementation(device, descriptor);
	}

	ref<Framebuffer> GraphicsAdapter::createFramebuffer(const Device* device, const FramebufferDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createFramebufferImplementation(device, descriptor);
	}

	ref<Image> GraphicsAdapter::createImage(const ImageDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createImageImplementation(descriptor);
	}

	ref<Instance> GraphicsAdapter::createInstance(const InstanceDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createInstanceImplementation(this, descriptor);
	}

	ref<PipelineLayout> GraphicsAdapter::createPipelineLayout(const PipelineLayoutDescriptor& descriptor, const Device* device) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createPipelineLayoutImplementation(descriptor, device);
	}

	ref<RenderPass> GraphicsAdapter::createRenderPass(const Device* device, const RenderPassDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createRenderPassImplementation(device, descriptor);
	}

	ref<GraphicsPipeline> GraphicsAdapter::createGraphicsPipeline(const Device* device, const GraphicsPipelineDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createGraphicsPipelineImplementation(device, descriptor);
	}

	ref<Sampler> GraphicsAdapter::createSampler(const SamplerDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createSamplerImplementation(descriptor);
	}

	ref<Semaphore> GraphicsAdapter::createSemaphore(const Device* device) const {
		return createSemaphoreImplementation(device);
	}

	ref<ShaderModule> GraphicsAdapter::createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset, const Device* device) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createShaderModuleFromAssetImplementation(descriptor, asset, device);
	}

	ref<ShaderModule> GraphicsAdapter::createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code, const Device* device) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createShaderModuleFromCodeImplementation(descriptor, code, device);
	}

	ref<Surface> GraphicsAdapter::createSurface(const Instance* instance, const Window* window) const {
		return createSurfaceImplementation(instance, window);
	}

	ref<Swapchain> GraphicsAdapter::createSwapchain(const Device* device, const SwapchainDescriptor& descriptor) const {
		return createSwapchainImplementation(device, descriptor);
	}

	void GraphicsAdapter::destroyBuffer(const Device* device, const Buffer* buffer) const {
		destroyBufferImplementation(device, buffer);
	}

	void GraphicsAdapter::destroyCommandEncoder(const Device* device, const CommandPool* commandPool, const CommandEncoder* commandEncoder) const {
		destroyCommandEncoderImplementation(device, commandPool, commandEncoder);
	}

	void GraphicsAdapter::destroyCommandPool(const Device* device, const CommandPool* commandPool) const {
		destroyCommandPoolImplementation(device, commandPool);
	}

	void GraphicsAdapter::destroyDevice(const Device* device) const {
		destroyDeviceImplementation(device);
	}

	void GraphicsAdapter::destroyFence(const Device* device, const Fence* fence) const {
		destroyFenceImplementation(device, fence);
	}

	void GraphicsAdapter::destroyFramebuffer(const Device* device, const Framebuffer* framebuffer) const {
		destroyFramebufferImplementation(device, framebuffer);
	}

	void GraphicsAdapter::destroyGraphicsPipeline(const Device* device, const GraphicsPipeline* graphicsPipeline) const {
		destroyGraphicsPipelineImplementation(device, graphicsPipeline);
	}

	void GraphicsAdapter::destroyInstance(const Instance* instance) const {
		destroyInstanceImplementation(instance);
	}

	void GraphicsAdapter::destroyQueue(const Queue* queue) const {
		destroyQueueImplementation(queue);
	}

	void GraphicsAdapter::destroySemaphore(const Device* device, const Semaphore* semaphore) const {
		destroySemaphoreImplementation(device, semaphore);
	}

	void GraphicsAdapter::destroySurface(const Instance* instance, const Surface* surface) const {
		destroySurfaceImplementation(instance, surface);
	}

	void GraphicsAdapter::destroySwapchain(const Device* device, const Swapchain* swapchain) const {
		destroySwapchainImplementation(device, swapchain);
	}

	ref<Queue> GraphicsAdapter::getQueue(const Device* device, QueueFamilyCapability queueFamilyCapability) const {
		return getQueueImplementation(device, queueFamilyCapability);
	}

	ImageFormat GraphicsAdapter::getSurfaceFormat(const Surface* surface) const {
		return getSurfaceFormatImplementation(surface);
	}

	void* GraphicsAdapter::mapBuffer(const Device* device, const Buffer* buffer, uint64 offset, uint64 size) const {
		return mapBufferImplementation(device, buffer, offset, size);
	}

	PresentStatus GraphicsAdapter::present(const Queue* queue, view<ref<Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Semaphore> waitSemaphore) const {
		return presentImplementation(queue, swapchains, presentableImageIndices, waitSemaphore);
	}

	void GraphicsAdapter::resetFence(const Device* device, const Fence* fence) const {
		resetFenceImplementation(device, fence);
	}

	ref<PhysicalDevice> GraphicsAdapter::selectPhysicalDevice(const Instance* instance, bool preferIntegratedGpu) const {
		return selectPhysicalDeviceImplementation(instance, preferIntegratedGpu);
	}

	void GraphicsAdapter::submit(const Queue* queue, view<ref<CommandBuffer>> commandBuffers) const {
		submitImplementation(queue, commandBuffers);
	}

	void GraphicsAdapter::submit(const Queue* queue, view<ref<CommandBuffer>> commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Fence> fence) const {
		submit2Implementation(queue, commandBuffers, waitSemaphore, signalSemaphore, fence);
	}

	void GraphicsAdapter::unmapBuffer(const Device* device, const Buffer* buffer) const {
		unmapBufferImplementation(device, buffer);
	}

	void GraphicsAdapter::updateDescriptorSets(const Device* device, view<WriteDescriptorSet> writeDescriptorSets) const {
		updateDescriptorSetsImplementation(device, writeDescriptorSets);
	}

	void GraphicsAdapter::waitForFence(const Device* device, const Fence* fence) const {
		waitForFenceImplementation(device, fence);
	}

	void GraphicsAdapter::waitIdle(const Device* device) const {
		waitIdleDeviceImplementation(device);
	}

	void GraphicsAdapter::waitIdle(const Queue* queue) const {
		waitIdleQueueImplementation(queue);
	}
}