// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class AcquireNextImageStatus : uint8;
	enum class QueueFamilyCapability : uint8;

	struct AttachmentImageDescriptor;
	struct BeginRenderPassDescriptor;
	struct BufferDescriptor;
	struct CommandBufferDescriptor;
	struct ComputePipelineDescriptor;
	struct FenceDescriptor;
	struct FramebufferDescriptor;
	struct ImageDescriptor;
	struct ImageViewDescriptor;
	struct InstanceDescriptor;
	struct PipelineLayoutDescriptor;
	struct RenderPassDescriptor;
	struct RenderPipelineDescriptor;
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
	class Image;
	class ImageView;
	class Instance;
	class MemoryAllocator;
	class PhysicalDevice;
	class PipelineLayout;
	class Queue;
	class RenderPass;
	class RenderPipeline;
	class Sampler;
	class Semaphore;
	class ShaderModule;
	class Surface;
	class Swapchain;
}