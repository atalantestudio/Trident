// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// TODO(graphics/api): Split into multiple API headers.

	enum class AcquireNextImageStatus : uint8;
	enum DeviceExtensionName : uint8;
	enum class GraphicsAPI : uint8;
	enum class ImageLayout : uint8;
	enum class ImageSampleCount : uint8;
	enum class ImageType : uint8;
	enum class ImageViewType : uint8;
	enum class IndexFormat;
	enum class MemoryProperty : uint8;
	enum class PipelineStage : uint8;
	enum class PresentStatus : uint8;
	enum class QueueFamilyCapability : uint8;

	struct ShaderModuleAsset;

	struct AttachmentImageDescriptor;
	struct BeginRenderPassDescriptor;
	struct BufferDescriptor;
	struct BufferCopyRegion;
	struct BufferImageCopyRegion;
	struct BufferMemoryBarrier;
	struct CommandBufferDescriptor;
	struct ComputePipelineDescriptor;
	struct DescriptorSetLayoutBinding;
	struct DescriptorSetLayoutDescriptor;
	struct FenceDescriptor;
	struct FramebufferDescriptor;
	struct GraphicsPipelineDescriptor;
	struct ImageDescriptor;
	struct ImageMemoryBarrier;
	struct ImageViewDescriptor;
	struct InstanceDescriptor;
	struct MemoryAllocation;
	struct MemoryRequirements;
	struct MemorySuballocation;
	struct PipelineLayoutDescriptor;
	struct RenderPassDescriptor;
	struct SamplerDescriptor;
	struct ShaderModuleDescriptor;
	struct Subpass;
	struct SwapchainDescriptor;
	struct WriteDescriptorSet;

	class BaseBufferView;
	class Buffer;

	template<typename T>
	class BufferView;

	class CommandBuffer;
	class CommandEncoder;
	class CommandPool;
	class ComputePipeline;
	class DescriptorSet;
	class DescriptorSetLayout;
	class Device;

	template<DeviceExtensionName... Extensions>
	class ExtendedCommandEncoder;

	template<DeviceExtensionName... Extensions>
	class ExtendedDevice;

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

	typedef void (*BindAdapterImplementation)(GraphicsAdapter* adapter);

	typedef AcquireNextImageStatus (*AcquireNextImageImplementation)(const Device* device, const Swapchain* swapchain, const Semaphore* semaphore, uint32& imageIndex);
	typedef sequence<ref<DescriptorSet>> (*AllocateDescriptorSetsImplementation)(Device* device, view<ref<DescriptorSetLayout>> descriptorSetLayouts);
	typedef MemoryAllocation (*AllocateMemoryImplementation)(MemoryAllocator* memoryAllocator, bool dynamic, uint32 memoryTypeIndex, uint64 size, uint64 alignment, bool shaderDeviceAddress);
	typedef void (*BeginImplementation)(const CommandEncoder* commandEncoder);
	typedef void (*BeginRenderPassImplementation)(CommandEncoder* commandEncoder, const BeginRenderPassDescriptor& descriptor);
	typedef void (*BindComputePipelineImplementation)(CommandEncoder* commandEncoder, ref<ComputePipeline> computePipeline);
	typedef void (*BindDescriptorSetsImplementation)(CommandEncoder* commandEncoder, ref<PipelineLayout> pipelineLayout, uint32 descriptorSetIndexOffset, view<ref<DescriptorSet>> descriptorSets);
	typedef void (*BindGraphicsPipelineImplementation)(CommandEncoder* commandEncoder, ref<GraphicsPipeline> graphicsPipeline);
	typedef void (*BindIndexBufferImplementation)(CommandEncoder* commandEncoder, ref<Buffer> indexBuffer, uint64 offset, IndexFormat format);
	typedef void (*BindVertexBufferImplementation)(CommandEncoder* commandEncoder, uint32 bindingIndex, ref<Buffer> vertexBuffer, uint64 offset);
	typedef void (*CopyBufferToBufferImplementation)(CommandEncoder* commandEncoder, ref<Buffer> sourceBuffer, ref<Buffer> destinationBuffer, view<BufferCopyRegion> copyRegions);
	typedef void (*CopyBufferToImageImplementation)(CommandEncoder* commandEncoder, ref<Buffer> sourceBuffer, ref<Image> destinationImage, view<BufferImageCopyRegion> copyRegions);
	typedef void (*CopyImageToBufferImplementation)(CommandEncoder* commandEncoder, ref<Image> sourceImage, ref<Buffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions);
	typedef ref<Buffer> (*CreateBufferImplementation)(const Device* device, const BufferDescriptor& descriptor);
	typedef ref<CommandEncoder> (*CreateCommandEncoderImplementation)(const Device* device, const CommandPool* commandPool, const CommandBufferDescriptor& descriptor);
	typedef ref<CommandPool> (*CreateCommandPoolImplementation)(const Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability);
	typedef ref<ComputePipeline> (*CreateComputePipelineImplementation)(const Device* device, const ComputePipelineDescriptor& descriptor);
	typedef ref<DescriptorSetLayout> (*CreateDescriptorSetLayoutImplementation)(const Device* device, const DescriptorSetLayoutDescriptor& descriptor);
	typedef ref<Device> (*CreateDeviceImplementation)(const GraphicsAdapter* adapter, const PhysicalDevice* physicalDevice, view<DeviceExtensionName> enabledExtensions);
	typedef ref<Fence> (*CreateFenceImplementation)(const Device* device, const FenceDescriptor& descriptor);
	typedef ref<Framebuffer> (*CreateFramebufferImplementation)(const Device* device, const FramebufferDescriptor& descriptor);
	typedef ref<GraphicsPipeline> (*CreateGraphicsPipelineImplementation)(const Device* device, const GraphicsPipelineDescriptor& descriptor);
	typedef ref<Image> (*CreateImageImplementation)(const Device* device, const ImageDescriptor& descriptor);
	typedef ref<ImageView> (*CreateImageViewImplementation)(const Device* device, Image* image, const ImageViewDescriptor& descriptor);
	typedef ref<Instance> (*CreateInstanceImplementation)(const GraphicsAdapter* adapter, const InstanceDescriptor& instanceDescriptor);
	typedef ref<MemoryAllocator> (*CreateMemoryAllocatorImplementation)(const Device* device);
	typedef ref<PipelineLayout> (*CreatePipelineLayoutImplementation)(const Device* device, const PipelineLayoutDescriptor& descriptor);
	typedef ref<RenderPass> (*CreateRenderPassImplementation)(const Device* device, const RenderPassDescriptor& descriptor);
	typedef ref<Sampler> (*CreateSamplerImplementation)(const Device* device, const SamplerDescriptor& descriptor);
	typedef ref<Semaphore> (*CreateSemaphoreImplementation)(const Device* device);
	typedef ref<ShaderModule> (*CreateShaderModuleFromAssetImplementation)(const Device* device, const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset);
	typedef ref<ShaderModule> (*CreateShaderModuleFromCodeImplementation)(const Device* device, const ShaderModuleDescriptor& descriptor, const sequence<uint8>& code);
	typedef ref<Surface> (*CreateSurfaceImplementation)(const Instance* instance, ref<PhysicalDevice> physicalDevice, const Window* window);
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
	typedef void (*DestroyImageImplementation)(const Device* device, const Image* image);
	typedef void (*DestroyImageViewImplementation)(const Device* device, const ImageView* imageView);
	typedef void (*DestroyInstanceImplementation)(const Instance* instance);
	typedef void (*DestroyPipelineLayoutImplementation)(const Device* device, const PipelineLayout* pipelineLayout);
	typedef void (*DestroyQueueImplementation)(const Queue* queue);
	typedef void (*DestroyRenderPassImplementation)(const Device* device, const RenderPass* renderPass);
	typedef void (*DestroySamplerImplementation)(const Device* device, const Sampler* sampler);
	typedef void (*DestroySemaphoreImplementation)(const Device* device, const Semaphore* semaphore);
	typedef void (*DestroyShaderModuleImplementation)(const Device* device, const ShaderModule* shaderModule);
	typedef void (*DestroySurfaceImplementation)(const Instance* instance, const Surface* surface);
	typedef void (*DestroySwapchainImplementation)(const Device* device, const Swapchain* swapchain);
	typedef void (*DeviceWaitIdleImplementation)(const Device* device);
	typedef void (*DispatchImplementation)(CommandEncoder* commandEncoder, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ);
	typedef void (*DispatchIndirectImplementation)(CommandEncoder* commandEncoder, ref<Buffer> buffer, uint64 bufferOffset);
	typedef void (*DrawImplementation)(CommandEncoder* commandEncoder, uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset);
	typedef void (*DrawIndirectImplementation)(CommandEncoder* commandEncoder, ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride);
	typedef void (*DrawIndexedImplementation)(CommandEncoder* commandEncoder, uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset);
	typedef void (*DrawIndexedIndirectImplementation)(CommandEncoder* commandEncoder, ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride);
	typedef ref<CommandBuffer> (*EndImplementation)(const CommandEncoder* commandEncoder);
	typedef void (*EndRenderPassImplementation)(CommandEncoder* commandEncoder);
	typedef void (*FlushMappedMemoryRangesImplementation)(const Device* device, view<BaseBufferView> mappedMemoryRanges);
	typedef uint32 (*FindMemoryTypeIndexImplementation)(const MemoryAllocator* memoryAllocator, MemoryProperty memoryProperties, bool transferDestination);
	typedef MemoryRequirements (*GetBufferMemoryRequirementsImplementation)(const MemoryAllocator* memoryAllocator, ref<Buffer> buffer);
	typedef MemoryRequirements (*GetImageMemoryRequirementsImplementation)(const MemoryAllocator* memoryAllocator, ref<Image> image);
	typedef uint32 (*GetPhysicalDeviceMemoryTypeCountImplementation)(const PhysicalDevice* physicalDevice);
	typedef view<char8> (*GetPhysicalDeviceNameImplementation)(const PhysicalDevice* physicalDevice);
	typedef ImageFormat (*GetSurfaceFormatImplementation)(const Surface* surface);
	typedef void (*InsertMemoryBarriersImplementation)(CommandEncoder* commandEncoder, view<BufferMemoryBarrier> bufferMemoryBarriers, view<ImageMemoryBarrier> imageMemoryBarriers);
	typedef void* (*MapBufferImplementation)(const Device* device, Buffer* buffer, uint64 offset, uint64 size);
	typedef void (*NextSubpassImplementation)(CommandEncoder* commandEncoder);
	typedef PresentStatus (*PresentImplementation)(const Queue* queue, view<ref<Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Semaphore> waitSemaphore);
	typedef void (*QueueWaitIdleImplementation)(const Queue* queue);
	typedef void (*ResetFenceImplementation)(const Device* device, const Fence* fence);
	typedef void (*ResetImplementation)(CommandEncoder* commandEncoder);
	typedef ref<PhysicalDevice> (*SelectPhysicalDeviceImplementation)(const Instance* instance, bool preferIntegratedGpu);
	typedef void (*SetLineWidthImplementation)(CommandEncoder* commandEncoder, float32 lineWidth);
	typedef void (*SetScissorImplementation)(CommandEncoder* commandEncoder, sint32 xOffset, sint32 yOffset, Extent2D extent);
	typedef void (*SetViewportImplementation)(CommandEncoder* commandEncoder, vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth);
	typedef MemorySuballocation (*SuballocateBufferMemoryImplementation)(MemoryAllocator* memoryAllocator, uint32 allocationIndex, ref<Buffer> buffer);
	typedef MemorySuballocation (*SuballocateImageMemoryImplementation)(MemoryAllocator* memoryAllocator, uint32 allocationIndex, ref<Image> image);
	typedef void (*SubmitImplementation)(const Queue* queue, view<ref<CommandBuffer>> commandBuffers);
	typedef void (*Submit2Implementation)(const Queue* queue, view<ref<CommandBuffer>> commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Fence> fence);
	typedef void (*UnmapBufferImplementation)(const Device* device, Buffer* buffer);
	typedef void (*UpdateDescriptorSetsImplementation)(const Device* device, view<WriteDescriptorSet> writeDescriptorSets);
	typedef void (*UpdateSwapchainImplementation)(const Device* device, Swapchain* swapchain, const SwapchainDescriptor& descriptor);
	typedef void (*WaitForFenceImplementation)(const Device* device, Fence* fence);

	// EXT_hardware_ray_tracing

	enum class AccelerationStructureBuildFlags : uint8;
	enum class GeometryFlags : uint8;
	enum class InstanceFlags;

	struct AccelerationStructureBuildInstanceGeometryDescriptor;
	struct AccelerationStructureBuildSizes;
	struct AccelerationStructureBuildTriangleGeometryDescriptor;
	struct AccelerationStructureDescriptor;
	struct DeviceAddressRegion;
	struct RayTracingPipelineDescriptor;

	class AccelerationStructure;
	class AccelerationStructureBuildGeometry;
	class RayTracingPipeline;

	typedef void (*BindRayTracingPipelineImplementation)(CommandEncoder* commandEncoder, ref<RayTracingPipeline> pipeline);
	typedef void (*BuildAccelerationStructuresImplementation)(CommandEncoder* commandEncoder, view<ref<AccelerationStructure>> accelerationStructures, view<BaseBufferView> scratchViews);
	typedef ref<AccelerationStructureBuildGeometry> (*CreateAccelerationStructureBuildInstanceGeometryImplementation)(const Device* device, const AccelerationStructureBuildInstanceGeometryDescriptor& descriptor);
	typedef ref<AccelerationStructureBuildGeometry> (*CreateAccelerationStructureBuildTriangleGeometryImplementation)(const Device* device, const AccelerationStructureBuildTriangleGeometryDescriptor& descriptor);
	typedef ref<AccelerationStructure> (*CreateAccelerationStructureImplementation)(const Device* device, const AccelerationStructureDescriptor& descriptor);
	typedef ref<RayTracingPipeline> (*CreateRayTracingPipelineImplementation)(const Device* device, const RayTracingPipelineDescriptor& descriptor);
	typedef void (*DestroyAccelerationStructureImplementation)(const Device* device, const AccelerationStructure* accelerationStructure);
	typedef void (*DestroyRayTracingPipelineImplementation)(const Device* device, const RayTracingPipeline* pipeline);
	typedef AccelerationStructureBuildSizes (*GetAccelerationStructureBuildSizesImplementation)(const Device* device, ref<AccelerationStructureBuildGeometry> buildGeometry);
	typedef uint64 (*GetAccelerationStructureDeviceAddressImplementation)(const Device* device, const AccelerationStructure* accelerationStructure);
	typedef uint64 (*GetBufferDeviceAddressImplementation)(const Device* device, ref<Buffer> buffer);
	typedef void (*GetRayTracingShaderGroupHandlesImplementation)(const Device* device, const RayTracingPipeline* pipeline, range<uint32, uint32> groupRange, void* mappedRange);
	typedef void (*TraceRaysImplementation)(CommandEncoder* commandEncoder, const DeviceAddressRegion& generationShaderBindingTable, const DeviceAddressRegion& missShaderBindingTable, const DeviceAddressRegion& hitShaderBindingTable, const DeviceAddressRegion& callableShaderBindingTable, Extent3D extent);
}