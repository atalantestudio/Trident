// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/Instance.hpp"
#include "Engine/Adapter/Graphics/DX12/Surface.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::DX12 {
	void bindAdapter(Trident::GraphicsAdapter* adapter);

	ref<Trident::ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor, const Trident::Device* device);

	ref<DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings, const Trident::Device* device);

	ref<Trident::Device> createDevice(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice);

	ref<Trident::MemoryAllocator> createMemoryAllocator(const Trident::Device* device);

	ref<Trident::PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor, const Trident::Device* device);

	ref<Trident::ShaderModule> createShaderModuleFromAsset(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset, const Trident::Device* device);

	ref<Trident::ShaderModule> createShaderModuleFromCode(const ShaderModuleDescriptor& descriptor, view<char8> code, const Trident::Device* device);

	ref<Trident::Surface> createSurface(const Trident::Instance* instance, const Window* window);

	ref<Trident::Swapchain> createSwapchain(const SwapchainDescriptor& descriptor, const Trident::Device* device);
}