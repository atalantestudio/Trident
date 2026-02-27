// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/Application/ApplicationProperties.hpp"
#include "Engine/Application/ProjectDescriptor.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class GraphicsAdapter {
		public:
			static ConsoleLogger logger;

		public:
			explicit GraphicsAdapter(const ApplicationProperties& properties);

			GraphicsAPI getApi() const;

			AcquireNextImageStatus acquireNextImage(const Device* device, const Swapchain* swapchain, const Semaphore* semaphore, uint32& imageIndex) const;

			sequence<ref<DescriptorSet>> allocateDescriptorSets(Device* device, view<ref<DescriptorSetLayout>> descriptorSetLayouts) const;

			void begin(const CommandEncoder* commandEncoder) const;

			void beginRenderPass(CommandEncoder* commandEncoder, const BeginRenderPassDescriptor& descriptor) const;

			void bindComputePipeline(CommandEncoder* commandEncoder, ref<ComputePipeline> computePipeline) const;

			void bindDescriptorSets(CommandEncoder* commandEncoder, ref<PipelineLayout> pipelineLayout, uint32 descriptorSetIndexOffset, view<ref<DescriptorSet>> descriptorSets) const;

			void bindGraphicsPipeline(CommandEncoder* commandEncoder, ref<GraphicsPipeline> graphicsPipeline) const;

			void bindIndexBuffer(CommandEncoder* commandEncoder, ref<Buffer> indexBuffer, uint64 offset, uint8 indexTypeSize) const;

			void bindVertexBuffer(CommandEncoder* commandEncoder, uint32 bindingIndex, ref<Buffer> vertexBuffer, uint64 offset, uint32 stride) const;

			void copyBufferToBuffer(CommandEncoder* commandEncoder, ref<Buffer> sourceBuffer, ref<Buffer> destinationBuffer, view<BufferCopyRegion> copyRegions) const;

			void copyBufferToImage(CommandEncoder* commandEncoder, ref<Buffer> sourceBuffer, ref<Image> destinationImage, view<BufferImageCopyRegion> copyRegions) const;

			void copyImageToBuffer(CommandEncoder* commandEncoder, ref<Image> sourceImage, ref<Buffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions) const;

			ref<Buffer> createBuffer(const Device* device, const BufferDescriptor& descriptor) const;

			template<typename ElementType>
			ref<Buffer2<ElementType>> createBuffer2(const Device* device, const BufferDescriptor& descriptor) const {
				DEBUG_DESCRIPTOR_GUARD(descriptor);

				ref<Buffer> buffer = createBufferImplementation(device, descriptor, static_cast<uint32>(sizeof(ElementType)));

				return std::static_pointer_cast<Buffer2<ElementType>>(buffer);
			}

			ref<CommandEncoder> createCommandEncoder(const Device* device, const CommandPool* commandPool, const CommandBufferDescriptor& descriptor) const;

			ref<CommandPool> createCommandPool(const Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) const;

			ref<ComputePipeline> createComputePipeline(const Device* device, const ComputePipelineDescriptor& descriptor) const;

			ref<DescriptorSetLayout> createDescriptorSetLayout(const Device* device, const sequence<DescriptorSetLayoutBinding>& bindings) const;

			ref<Device> createDevice(const PhysicalDevice* physicalDevice) const;

			ref<Fence> createFence(const Device* device, const FenceDescriptor& descriptor) const;

			ref<Framebuffer> createFramebuffer(const Device* device, const FramebufferDescriptor& descriptor) const;

			ref<Image> createImage(const Device* device, const ImageDescriptor& descriptor) const;

			ref<ImageView> createImageView(const Device* device, Image* image, const ImageViewDescriptor& descriptor) const;

			ref<Instance> createInstance(const InstanceDescriptor& descriptor) const;

			ref<PipelineLayout> createPipelineLayout(const Device* device, const PipelineLayoutDescriptor& descriptor) const;

			ref<RenderPass> createRenderPass(const Device* device, ref<Surface> surface, const RenderPassDescriptor& descriptor) const;

			ref<GraphicsPipeline> createGraphicsPipeline(const Device* device, const GraphicsPipelineDescriptor& descriptor) const;

			ref<Sampler> createSampler(const Device* device, const SamplerDescriptor& descriptor) const;

			ref<Semaphore> createSemaphore(const Device* device) const;

			ref<ShaderModule> createShaderModule(const Device* device, const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const;
			ref<ShaderModule> createShaderModule(const Device* device, const ShaderModuleDescriptor& descriptor, view<char8> code) const;

			ref<Surface> createSurface(const Instance* instance, ref<PhysicalDevice> physicalDevice, const Window* window) const;

			ref<Swapchain> createSwapchain(const Device* device, const SwapchainDescriptor& descriptor) const;

			void destroyBuffer(const Device* device, const Buffer* buffer) const;

			void destroyCommandEncoder(const Device* device, const CommandPool* commandPool, const CommandEncoder* commandEncoder) const;

			void destroyCommandPool(const Device* device, const CommandPool* commandPool) const;

			void destroyComputePipeline(const Device* device, const ComputePipeline* computePipeline) const;

			void destroyDescriptorSetLayout(const Device* device, const DescriptorSetLayout* descriptorSetLayout) const;

			void destroyDevice(const Device* device) const;

			void destroyFence(const Device* device, const Fence* fence) const;

			void destroyFramebuffer(const Device* device, const Framebuffer* framebuffer) const;

			void destroyGraphicsPipeline(const Device* device, const GraphicsPipeline* graphicsPipeline) const;

			void destroyImage(const Device* device, const Image* image) const;

			void destroyImageView(const Device* device, const ImageView* imageView) const;

			void destroyInstance(const Instance* instance) const;

			void destroyPipelineLayout(const Device* device, const PipelineLayout* pipelineLayout) const;

			void destroyQueue(const Queue* queue) const;

			void destroyRenderPass(const Device* device, const RenderPass* renderPass) const;

			void destroySampler(const Device* device, const Sampler* sampler) const;

			void destroySemaphore(const Device* device, const Semaphore* semaphore) const;

			void destroyShaderModule(const Device* device, const ShaderModule* shaderModule) const;

			void destroySurface(const Instance* instance, const Surface* surface) const;

			void destroySwapchain(const Device* device, const Swapchain* swapchain) const;

			void dispatch(CommandEncoder* commandEncoder, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) const;

			void dispatchIndirect(CommandEncoder* commandEncoder, ref<Buffer> buffer, uint64 bufferOffset) const;

			void draw(CommandEncoder* commandEncoder, uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) const;

			void drawIndirect(CommandEncoder* commandEncoder, ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) const;

			void drawIndexed(CommandEncoder* commandEncoder, uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) const;

			void drawIndexedIndirect(CommandEncoder* commandEncoder, ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) const;

			ref<CommandBuffer> end(const CommandEncoder* commandEncoder) const;

			void endRenderPass(CommandEncoder* commandEncoder) const;

			ref<Queue> getQueue(const Device* device, QueueFamilyCapability queueFamilyCapability) const;

			void insertBufferMemoryBarriers(CommandEncoder* commandEncoder, view<BufferMemoryBarrier> barriers) const;

			void* mapBuffer(const Device* device, const Buffer* buffer, uint64 offset, uint64 size) const;

			void nextSubpass(CommandEncoder* commandEncoder) const;

			PresentStatus present(const Queue* queue, view<ref<Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Semaphore> waitSemaphore) const;

			void reset(CommandEncoder* commandEncoder) const;

			void resetFence(const Device* device, const Fence* fence) const;

			ref<PhysicalDevice> selectPhysicalDevice(const Instance* instance, bool preferIntegratedGpu) const;

			void setImageMemoryBarrier(CommandEncoder* commandEncoder, ref<Image> image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) const;

			void setLineWidth(CommandEncoder* commandEncoder, float32 lineWidth) const;

			void setScissor(CommandEncoder* commandEncoder, sint32 xOffset, sint32 yOffset, Extent2D extent) const;

			void setViewport(CommandEncoder* commandEncoder, vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth) const;

			void submit(const Queue* queue, view<ref<CommandBuffer>> commandBuffers) const;
			void submit(const Queue* queue, view<ref<CommandBuffer>> commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Fence> fence) const;

			void unmapBuffer(const Device* device, const Buffer* buffer) const;

			void updateDescriptorSets(const Device* device, view<WriteDescriptorSet> writeDescriptorSets) const;

			void updateSwapchain(const Device* device, Swapchain* swapchain, const SwapchainDescriptor& descriptor) const;

			void waitForFence(const Device* device, const Fence* fence) const;

			void waitIdle(const Device* device) const;
			void waitIdle(const Queue* queue) const;

		// TODO: Do not expose to the client side.
		public:
			AcquireNextImageImplementation acquireNextImageImplementation;
			AllocateDescriptorSetsImplementation allocateDescriptorSetsImplementation;
			BeginImplementation beginImplementation;
			BeginRenderPassImplementation beginRenderPassImplementation;
			BindComputePipelineImplementation bindComputePipelineImplementation;
			BindDescriptorSetsImplementation bindDescriptorSetsImplementation;
			BindGraphicsPipelineImplementation bindGraphicsPipelineImplementation;
			BindIndexBufferImplementation bindIndexBufferImplementation;
			BindVertexBufferImplementation bindVertexBufferImplementation;
			CopyBufferToBufferImplementation copyBufferToBufferImplementation;
			CopyBufferToImageImplementation copyBufferToImageImplementation;
			CopyImageToBufferImplementation copyImageToBufferImplementation;
			CreateBufferImplementation createBufferImplementation;
			CreateCommandEncoderImplementation createCommandEncoderImplementation;
			CreateCommandPoolImplementation createCommandPoolImplementation;
			CreateComputePipelineImplementation createComputePipelineImplementation;
			CreateDescriptorSetLayoutImplementation createDescriptorSetLayoutImplementation;
			CreateDeviceImplementation createDeviceImplementation;
			CreateFenceImplementation createFenceImplementation;
			CreateFramebufferImplementation createFramebufferImplementation;
			CreateImageImplementation createImageImplementation;
			CreateImageViewImplementation createImageViewImplementation;
			CreateInstanceImplementation createInstanceImplementation;
			CreateMemoryAllocatorImplementation createMemoryAllocatorImplementation;
			CreatePipelineLayoutImplementation createPipelineLayoutImplementation;
			CreateRenderPassImplementation createRenderPassImplementation;
			CreateGraphicsPipelineImplementation createGraphicsPipelineImplementation;
			CreateSamplerImplementation createSamplerImplementation;
			CreateSemaphoreImplementation createSemaphoreImplementation;
			CreateShaderModuleFromAssetImplementation createShaderModuleFromAssetImplementation;
			CreateShaderModuleFromCodeImplementation createShaderModuleFromCodeImplementation;
			CreateSurfaceImplementation createSurfaceImplementation;
			CreateSwapchainImplementation createSwapchainImplementation;
			DestroyBufferImplementation destroyBufferImplementation;
			DestroyCommandEncoderImplementation destroyCommandEncoderImplementation;
			DestroyCommandPoolImplementation destroyCommandPoolImplementation;
			DestroyComputePipelineImplementation destroyComputePipelineImplementation;
			DestroyDescriptorSetLayoutImplementation destroyDescriptorSetLayoutImplementation;
			DestroyDeviceImplementation destroyDeviceImplementation;
			DestroyFenceImplementation destroyFenceImplementation;
			DestroyFramebufferImplementation destroyFramebufferImplementation;
			DestroyGraphicsPipelineImplementation destroyGraphicsPipelineImplementation;
			DestroyImageImplementation destroyImageImplementation;
			DestroyImageViewImplementation destroyImageViewImplementation;
			DestroyInstanceImplementation destroyInstanceImplementation;
			DestroyPipelineLayoutImplementation destroyPipelineLayoutImplementation;
			DestroyQueueImplementation destroyQueueImplementation;
			DestroyRenderPassImplementation destroyRenderPassImplementation;
			DestroySamplerImplementation destroySamplerImplementation;
			DestroySemaphoreImplementation destroySemaphoreImplementation;
			DestroyShaderModuleImplementation destroyShaderModuleImplementation;
			DestroySurfaceImplementation destroySurfaceImplementation;
			DestroySwapchainImplementation destroySwapchainImplementation;
			DispatchImplementation dispatchImplementation;
			DispatchIndirectImplementation dispatchIndirectImplementation;
			DrawImplementation drawImplementation;
			DrawIndirectImplementation drawIndirectImplementation;
			DrawIndexedImplementation drawIndexedImplementation;
			DrawIndexedIndirectImplementation drawIndexedIndirectImplementation;
			EndImplementation endImplementation;
			EndRenderPassImplementation endRenderPassImplementation;
			GetQueueImplementation getQueueImplementation;
			InsertBufferMemoryBarriersImplementation insertBufferMemoryBarriersImplementation;
			MapBufferImplementation mapBufferImplementation;
			NextSubpassImplementation nextSubpassImplementation;
			PresentImplementation presentImplementation;
			ResetImplementation resetImplementation;
			ResetFenceImplementation resetFenceImplementation;
			SelectPhysicalDeviceImplementation selectPhysicalDeviceImplementation;
			SetImageMemoryBarrierImplementation setImageMemoryBarrierImplementation;
			SetLineWidthImplementation setLineWidthImplementation;
			SetScissorImplementation setScissorImplementation;
			SetViewportImplementation setViewportImplementation;
			SubmitImplementation submitImplementation;
			Submit2Implementation submit2Implementation;
			UnmapBufferImplementation unmapBufferImplementation;
			UpdateDescriptorSetsImplementation updateDescriptorSetsImplementation;
			UpdateSwapchainImplementation updateSwapchainImplementation;
			WaitForFenceImplementation waitForFenceImplementation;
			WaitIdleDeviceImplementation waitIdleDeviceImplementation;
			WaitIdleQueueImplementation waitIdleQueueImplementation;

		private:
			GraphicsAPI api;
	};
}