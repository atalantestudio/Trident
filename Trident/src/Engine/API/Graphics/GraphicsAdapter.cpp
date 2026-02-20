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

	ref<Buffer> GraphicsAdapter::createBuffer(const BufferDescriptor& descriptor, const Device* device) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		// TODO
		BREAKPOINT();

		return nullptr;
		// return createBufferImplementation(descriptor, 1, device);
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

	ref<Fence> GraphicsAdapter::createFence(const FenceDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createFenceImplementation(descriptor);
	}

	ref<Framebuffer> GraphicsAdapter::createFramebuffer(const FramebufferDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createFramebufferImplementation(descriptor);
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

	ref<RenderPass> GraphicsAdapter::createRenderPass(const RenderPassDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createRenderPassImplementation(descriptor);
	}

	ref<RenderPipeline> GraphicsAdapter::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createRenderPipelineImplementation(descriptor);
	}

	ref<Sampler> GraphicsAdapter::createSampler(const SamplerDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createSamplerImplementation(descriptor);
	}

	ref<Semaphore> GraphicsAdapter::createSemaphore() const {
		return createSemaphoreImplementation();
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

	ref<Swapchain> GraphicsAdapter::createSwapchain(const SwapchainDescriptor& descriptor, const Device* device) const {
		return createSwapchainImplementation(descriptor, device);
	}

	void* GraphicsAdapter::mapBuffer(const Buffer* buffer, uint64 size, uint64 offset) const {
		// TODO: Validate arguments.

		return mapBufferImplementation(buffer, size, offset);
	}

	ref<PhysicalDevice> GraphicsAdapter::selectPhysicalDevice(const Instance* instance, bool preferIntegratedGpu) const {
		return selectPhysicalDeviceImplementation(this, instance, preferIntegratedGpu);
	}

	void GraphicsAdapter::unmapBuffer(const Buffer* buffer) const {
		unmapBufferImplementation(buffer);
	}
}