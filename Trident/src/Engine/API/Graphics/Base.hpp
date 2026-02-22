// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	enum class AcquireNextImageStatus : uint8;
	enum class PresentStatus : uint8;
	enum class QueueFamilyCapability : uint8;

	struct AttachmentImageDescriptor;
	struct BeginRenderPassDescriptor;
	struct BufferDescriptor;
	struct CommandBufferDescriptor;
	struct ComputePipelineDescriptor;
	struct FenceDescriptor;
	struct FramebufferDescriptor;
	struct GraphicsPipelineDescriptor;
	struct ImageDescriptor;
	struct ImageViewDescriptor;
	struct InstanceDescriptor;
	struct PipelineLayoutDescriptor;
	struct RenderPassDescriptor;
	struct SamplerDescriptor;
	struct ShaderModuleDescriptor;
	struct SubpassDescriptor;
	struct SwapchainDescriptor;
	struct WriteDescriptorSet;

	struct DescriptorSetLayoutBinding;
	struct Subpass;

	class Buffer;

	template<typename ElementType>
	class Buffer2;

	class CommandBuffer;
	class CommandEncoder;
	class CommandPool;
	class ComputePipeline;
	class DescriptorSet;
	class DescriptorSetLayout;
	class Device;
	class Fence;
	class Framebuffer;
	class GraphicsAdapter;
	class GraphicsPipeline;
	class Image;
	class ImageView;
	class Instance;
	class MemoryAllocator;
	class PhysicalDevice;
	class PipelineLayout;
	class Queue;
	class RenderPass;
	class Sampler;
	class Semaphore;
	class ShaderModule;
	class Surface;
	class Swapchain;
	class Window;

	typedef AcquireNextImageStatus (*AcquireNextImageImplementation)(const Device* device, const Swapchain* swapchain, const Semaphore* semaphore, uint32& imageIndex);
	typedef sequence<ref<DescriptorSet>> (*AllocateDescriptorSetsImplementation)(const Device* device, view<ref<DescriptorSetLayout>> descriptorSetLayouts);
	typedef void (*BindAdapterImplementation)(GraphicsAdapter* adapter);
	typedef ref<Buffer> (*CreateBufferImplementation)(const Device* device, const BufferDescriptor& descriptor, uint32 stride);
	typedef ref<CommandEncoder> (*CreateCommandEncoderImplementation)(const Device* device, const CommandPool* commandPool, const CommandBufferDescriptor& descriptor);
	typedef ref<CommandPool> (*CreateCommandPoolImplementation)(const Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability);
	typedef ref<ComputePipeline> (*CreateComputePipelineImplementation)(const Device* device, const ComputePipelineDescriptor& descriptor);
	typedef ref<DescriptorSetLayout> (*CreateDescriptorSetLayoutImplementation)(const Device* device, const sequence<DescriptorSetLayoutBinding>& bindings);
	typedef ref<Device> (*CreateDeviceImplementation)(const GraphicsAdapter* adapter, const PhysicalDevice* physicalDevice);
	typedef ref<Fence> (*CreateFenceImplementation)(const Device* device, const FenceDescriptor& descriptor);
	typedef ref<Framebuffer> (*CreateFramebufferImplementation)(const Device* device, const FramebufferDescriptor& descriptor);
	typedef ref<Image> (*CreateImageImplementation)(const ImageDescriptor& descriptor);
	typedef ref<Instance> (*CreateInstanceImplementation)(const GraphicsAdapter* adapter, const InstanceDescriptor& instanceDescriptor);
	typedef ref<MemoryAllocator> (*CreateMemoryAllocatorImplementation)(const Device* device);
	typedef ref<PipelineLayout> (*CreatePipelineLayoutImplementation)(const Device* device, const PipelineLayoutDescriptor& descriptor);
	typedef ref<RenderPass> (*CreateRenderPassImplementation)(const Device* device, const RenderPassDescriptor& descriptor);
	typedef ref<GraphicsPipeline> (*CreateGraphicsPipelineImplementation)(const Device* device, const GraphicsPipelineDescriptor& descriptor);
	typedef ref<Sampler> (*CreateSamplerImplementation)(const SamplerDescriptor& descriptor);
	typedef ref<Semaphore> (*CreateSemaphoreImplementation)(const Device* device);
	typedef ref<ShaderModule> (*CreateShaderModuleFromAssetImplementation)(const Device* device, const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset);

	// TODO: Deprecate (replace with CreateShaderModuleFromAssetImplementation).
	typedef ref<ShaderModule> (*CreateShaderModuleFromCodeImplementation)(const Device* device, const ShaderModuleDescriptor& descriptor, view<char8> code);

	typedef ref<Surface> (*CreateSurfaceImplementation)(const Instance* instance, const Window* window);
	typedef ref<Swapchain> (*CreateSwapchainImplementation)(const Device* device, const SwapchainDescriptor& descriptor);
	typedef void (*DestroyBufferImplementation)(const Device* device, const Buffer* buffer);
	typedef void (*DestroyCommandEncoderImplementation)(const Device* device, const CommandPool* commandPool, const CommandEncoder* commandEncoder);
	typedef void (*DestroyCommandPoolImplementation)(const Device* device, const CommandPool* commandPool);
	typedef void (*DestroyComputePipelineImplementation)(const Device* device, const ComputePipeline* computePipeline);
	typedef void (*DestroyDescriptorSetLayoutImplementation)(const Device* device, const DescriptorSetLayout* descriptorSetLayout);
	typedef void (*DestroyDeviceImplementation)(const Device* device);
	typedef void (*DestroyFenceImplementation)(const Device* device, const Fence* fence);
	typedef void (*DestroyFramebufferImplementation)(const Device* device, const Framebuffer* framebuffer);
	typedef void (*DestroyGraphicsPipelineImplementation)(const Device* device, const GraphicsPipeline* graphicsPipeline);
	typedef void (*DestroyInstanceImplementation)(const Instance* instance);
	typedef void (*DestroyPipelineLayoutImplementation)(const Device* device, const PipelineLayout* pipelineLayout);
	typedef void (*DestroyQueueImplementation)(const Queue* queue);
	typedef void (*DestroySemaphoreImplementation)(const Device* device, const Semaphore* semaphore);
	typedef void (*DestroyShaderModuleImplementation)(const Device* device, const ShaderModule* shaderModule);
	typedef void (*DestroySurfaceImplementation)(const Instance* instance, const Surface* surface);
	typedef void (*DestroySwapchainImplementation)(const Device* device, const Swapchain* swapchain);
	typedef ref<Queue> (*GetQueueImplementation)(const Device* device, QueueFamilyCapability queueFamilyCapability);
	typedef ImageFormat (*GetSurfaceFormatImplementation)(const Surface* surface);
	typedef void* (*MapBufferImplementation)(const Device* device, const Buffer* buffer, uint64 offset, uint64 size);
	typedef PresentStatus (*PresentImplementation)(const Queue* queue, view<ref<Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Semaphore> waitSemaphore);
	typedef void (*ResetFenceImplementation)(const Device* device, const Fence* fence);
	typedef ref<PhysicalDevice> (*SelectPhysicalDeviceImplementation)(const Instance* instance, bool preferIntegratedGpu);
	typedef void (*SubmitImplementation)(const Queue* queue, view<ref<CommandBuffer>> commandBuffers);
	typedef void (*Submit2Implementation)(const Queue* queue, view<ref<CommandBuffer>> commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Fence> fence);
	typedef void (*UnmapBufferImplementation)(const Device* device, const Buffer* buffer);
	typedef void (*UpdateDescriptorSetsImplementation)(const Device* device, view<WriteDescriptorSet> writeDescriptorSets);
	typedef void (*WaitForFenceImplementation)(const Device* device, const Fence* fence);
	typedef void (*WaitIdleDeviceImplementation)(const Device* device);
	typedef void (*WaitIdleQueueImplementation)(const Queue* queue);
}