// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/Fence.hpp"
#include "Engine/Adapter/Graphics/DX12/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/DX12/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/Adapter/Graphics/DX12/MemoryAllocator.hpp"
#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/DX12/RenderPipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/Sampler.hpp"
#include "Engine/Adapter/Graphics/DX12/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"

namespace Trident {
	void DX12::bindAdapter(Trident::GraphicsAdapter* adapter) {
		adapter->logger = ConsoleLogger(std::cout, LogLevel::ALL, "DIRECTX 12");

		adapter->createBufferImplementation = DX12::createBuffer;
		adapter->createComputePipelineImplementation = DX12::createComputePipeline;
		adapter->createDescriptorSetLayoutImplementation = DX12::createDescriptorSetLayout;
		adapter->createDeviceImplementation = DX12::createDevice;
		adapter->createInstanceImplementation = DX12::createInstance;
		adapter->createMemoryAllocatorImplementation = DX12::createMemoryAllocator;
		adapter->createPipelineLayoutImplementation = DX12::createPipelineLayout;
		adapter->createShaderModuleFromAssetImplementation = DX12::createShaderModuleFromAsset;
		adapter->createShaderModuleFromCodeImplementation = DX12::createShaderModuleFromCode;
		adapter->createSurfaceImplementation = DX12::createSurface;
		adapter->createSwapchainImplementation = DX12::createSwapchain;
		adapter->destroyBufferImplementation = DX12::destroyBuffer;
		adapter->destroyInstanceImplementation = DX12::destroyInstance;
		adapter->mapBufferImplementation = DX12::mapBuffer;
		adapter->selectPhysicalDeviceImplementation = DX12::selectPhysicalDevice;
		adapter->unmapBufferImplementation = DX12::unmapBuffer;
	}

	ref<Trident::ComputePipeline> DX12::createComputePipeline(const ComputePipelineDescriptor& descriptor, const Trident::Device* device) {
		return createRef<DX12::ComputePipeline>(descriptor, static_cast<const DX12::Device*>(device));
	}

	ref<DescriptorSetLayout> DX12::createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings, const Trident::Device* device) {
		return createRef<DescriptorSetLayout>(bindings);
	}

	ref<Trident::Device> DX12::createDevice(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice) {
		return createRef<DX12::Device>(adapter, static_cast<const DX12::PhysicalDevice*>(physicalDevice));
	}

	ref<Trident::MemoryAllocator> DX12::createMemoryAllocator(const Trident::Device* device) {
		DX12::MemoryAllocator a(static_cast<const DX12::Device*>(device));

		return createRef<DX12::MemoryAllocator>(static_cast<const DX12::Device*>(device));
	}

	ref<Trident::PipelineLayout> DX12::createPipelineLayout(const PipelineLayoutDescriptor& descriptor, const Trident::Device* device) {
		return createRef<DX12::PipelineLayout>(descriptor, static_cast<const DX12::Device*>(device));
	}

	ref<Trident::ShaderModule> DX12::createShaderModuleFromAsset(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset, const Trident::Device* device) {
		ADAPTER_ASSERT(false, "Shader module assets are not supported by the DX12 adapter.");

		return nullptr;
	}

	ref<Trident::ShaderModule> DX12::createShaderModuleFromCode(const ShaderModuleDescriptor& descriptor, view<char8> code, const Trident::Device* device) {
		return createRef<DX12::ShaderModule>(descriptor, code);
	}

	ref<Trident::Surface> DX12::createSurface(const Trident::Instance* instance, const Window* window) {
		return createRef<DX12::Surface>(static_cast<const DX12::Instance*>(instance), window);
	}

	ref<Trident::Swapchain> DX12::createSwapchain(const SwapchainDescriptor& descriptor, const Trident::Device* device) {
		return createRef<DX12::Swapchain>(descriptor, static_cast<const DX12::Device*>(device));
	}
}