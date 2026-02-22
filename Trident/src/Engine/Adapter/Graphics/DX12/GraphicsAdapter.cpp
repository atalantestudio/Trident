// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/DX12/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/Fence.hpp"
#include "Engine/Adapter/Graphics/DX12/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/DX12/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/Adapter/Graphics/DX12/MemoryAllocator.hpp"
#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/DX12/Queue.hpp"
#include "Engine/Adapter/Graphics/DX12/RenderPipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/Sampler.hpp"
#include "Engine/Adapter/Graphics/DX12/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"

namespace Trident {
	namespace DX12 {
		static ref<DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings, const Trident::Device* device) {
			return createRef<DescriptorSetLayout>(bindings);
		}

		static ref<Trident::MemoryAllocator> createMemoryAllocator(const Trident::Device* device) {
			return createRef<DX12::MemoryAllocator>(static_cast<const DX12::Device*>(device));
		}

		static ref<RenderPass> createRenderPass(const Trident::Device* device, const RenderPassDescriptor& descriptor) {
			return createRef<RenderPass>(descriptor);
		}
	}

	void DX12::bindAdapter(Trident::GraphicsAdapter* adapter) {
		adapter->logger = ConsoleLogger(std::cout, LogLevel::ALL, "DIRECTX 12");

		adapter->acquireNextImageImplementation = DX12::acquireNextImage;
		adapter->allocateDescriptorSetsImplementation = DX12::allocateDescriptorSets;
		adapter->createBufferImplementation = DX12::createBuffer;
		adapter->createCommandEncoderImplementation = DX12::createCommandEncoder;
		adapter->createCommandPoolImplementation = DX12::createCommandPool;
		adapter->createComputePipelineImplementation = DX12::createComputePipeline;
		adapter->createDescriptorSetLayoutImplementation = DX12::createDescriptorSetLayout;
		adapter->createDeviceImplementation = DX12::createDevice;
		adapter->createFenceImplementation = DX12::createFence;
		adapter->createFramebufferImplementation = DX12::createFramebuffer;
		adapter->createInstanceImplementation = DX12::createInstance;
		adapter->createMemoryAllocatorImplementation = DX12::createMemoryAllocator;
		adapter->createPipelineLayoutImplementation = DX12::createPipelineLayout;
		adapter->createRenderPassImplementation = DX12::createRenderPass;
		adapter->createShaderModuleFromAssetImplementation = DX12::createShaderModuleFromAsset;
		adapter->createShaderModuleFromCodeImplementation = DX12::createShaderModuleFromCode;
		adapter->createSurfaceImplementation = DX12::createSurface;
		adapter->createSwapchainImplementation = DX12::createSwapchain;
		adapter->destroyBufferImplementation = DX12::destroyBuffer;
		adapter->destroyCommandEncoderImplementation = DX12::destroyCommandEncoder;
		adapter->destroyCommandPoolImplementation = DX12::destroyCommandPool;
		adapter->destroyDeviceImplementation = DX12::destroyDevice;
		adapter->destroyFenceImplementation = DX12::destroyFence;
		adapter->destroyFramebufferImplementation = DX12::destroyFramebuffer;
		adapter->destroyInstanceImplementation = DX12::destroyInstance;
		adapter->destroyQueueImplementation = DX12::destroyQueue;
		adapter->destroySurfaceImplementation = DX12::destroySurface;
		adapter->destroySwapchainImplementation = DX12::destroySwapchain;
		adapter->getQueueImplementation = DX12::getQueue;
		adapter->getSurfaceFormatImplementation = DX12::getSurfaceFormat;
		adapter->mapBufferImplementation = DX12::mapBuffer;
		adapter->resetFenceImplementation = DX12::resetFence;
		adapter->selectPhysicalDeviceImplementation = DX12::selectPhysicalDevice;
		adapter->unmapBufferImplementation = DX12::unmapBuffer;
		adapter->updateDescriptorSetsImplementation = DX12::updateDescriptorSets;
		adapter->waitForFenceImplementation = DX12::waitForFence;
		adapter->waitIdleImplementation = DX12::waitIdle;
	}

	ref<Trident::ComputePipeline> DX12::createComputePipeline(const ComputePipelineDescriptor& descriptor, const Trident::Device* device) {
		return createRef<DX12::ComputePipeline>(descriptor, static_cast<const DX12::Device*>(device));
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
}