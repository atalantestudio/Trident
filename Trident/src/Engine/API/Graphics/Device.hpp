// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/DescriptorSet.hpp"
#include "Engine/API/Graphics/DescriptorSetLayout.hpp"
#include "Engine/API/Graphics/Queue.hpp"
#include "Engine/API/Graphics/QueueFamilyCapability.hpp"
#include "Engine/API/Graphics/Status.hpp"
#include "Engine/API/Graphics/Swapchain.hpp"

namespace Trident {
	class Device {
		friend class GraphicsAdapter;

		public:
			~Device();

			// Acquires the next available swapchain image and writes its index to imageIndex.
			AcquireNextImageStatus acquireNextImage(ref<Swapchain> swapchain, ref<Semaphore> semaphore, uint32& imageIndex) const;

			sequence<ref<DescriptorSet>> allocateDescriptorSets(view<ref<DescriptorSetLayout>> descriptorSetLayouts);

			/// Creates and returns a buffer object.
			/// The returned buffer object is pre-allocated from a dynamic memory allocation.
			// NOTE
			// For most resources (excluding render pass attachments and other resources depending on the screen size),
			// prefer createStaticBuffer over this method.
			template<typename ElementType>
			ref<Buffer<ElementType>> createBuffer(const BufferDescriptor& descriptor) const {
				#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
					uint32 warningCount = 0;
					uint32 errorCount = 0;

					if (descriptor.size == 0) {
						ENGINE_ERROR("descriptor.size must not be 0.");

						errorCount += 1;
					}

					if (descriptor.name.count() == 0) {
						ENGINE_WARNING("descriptor.name should provide a unique name to help identify this buffer.");

						warningCount += 1;
					}

					ENGINE_ASSERT(warningCount == 0 && errorCount == 0, "Invalid argument(s) ([] warnings and [] errors).", warningCount, errorCount);
				#endif

				const ref<BaseBuffer> buffer = adapter->createBufferImplementation(this, descriptor, static_cast<uint32>(sizeof(ElementType)));

				return std::static_pointer_cast<Buffer<ElementType>>(buffer);
			}

			/// Creates and returns a buffer object.
			/// The returned buffer object is to be allocated from a static memory allocation.
			ref<BaseBuffer> createStaticBuffer(const BufferDescriptor& descriptor) const {
				#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
					uint32 warningCount = 0;
					uint32 errorCount = 0;

					if (descriptor.size == 0) {
						ENGINE_ERROR("descriptor.size must not be 0.");

						errorCount += 1;
					}

					if (descriptor.name.count() == 0) {
						ENGINE_WARNING("descriptor.name should provide a unique name to help identify this buffer.");

						warningCount += 1;
					}

					ENGINE_ASSERT(warningCount == 0 && errorCount == 0, "Invalid argument(s) ([] warnings and [] errors).", warningCount, errorCount);
				#endif

				return adapter->createStaticBufferImplementation(this, descriptor);
			}

			ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) const;

			// TODO: Use view<DescriptorSetLayoutBinding> instead.
			ref<DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const;

			ref<Fence> createFence(const FenceDescriptor& descriptor) const;

			ref<Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor) const;

			ref<GraphicsPipeline> createGraphicsPipeline(const GraphicsPipelineDescriptor& descriptor) const;

			ref<Image> createImage(const ImageDescriptor& descriptor) const;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const;

			ref<RenderPass> createRenderPass(ref<Surface> surface, const RenderPassDescriptor& descriptor) const;

			ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const;

			ref<Semaphore> createSemaphore() const;

			ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const;

			// TODO: Deprecate (replace with the asset overload above).
			ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) const;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const;

			ref<MemoryAllocator> getMemoryAllocator() const;

			ref<PhysicalDevice> getPhysicalDevice() const;

			ref<Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const;

			void resetFence(ref<Fence> fence) const;

			void updateDescriptorSets(view<WriteDescriptorSet> writeDescriptorSets) const;

			void waitForFence(ref<Fence> fence) const;

			/// Waits until all queues created with the device reach an idle state.
			/// To wait on a particular queue, see Queue::waitIdle.
			void waitIdle() const;

		protected:
			explicit Device(const GraphicsAdapter* adapter, const PhysicalDevice* physicalDevice);

		public:
			const GraphicsAdapter* adapter;
			const PhysicalDevice* physicalDevice;

			// The memory allocator is automatically created by the adapter after the API creation of the device.
			ref<MemoryAllocator> memoryAllocator;
	};
}