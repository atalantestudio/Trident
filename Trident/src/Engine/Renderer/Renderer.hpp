// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Event/Window/WindowResizeEvent.hpp"
#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/API/Graphics/CommandEncoder.hpp"
#include "Engine/API/Graphics/Descriptor/ComputePipelineDescriptor.hpp"
#include "Engine/API/Graphics/Descriptor/GraphicsPipelineDescriptor.hpp"
#include "Engine/API/Graphics/Descriptor/ImageDescriptor.hpp"
#include "Engine/API/Graphics/Descriptor/ImageViewDescriptor.hpp"
#include "Engine/API/Graphics/Descriptor/PipelineLayoutDescriptor.hpp"
#include "Engine/API/Graphics/Descriptor/StaticMemoryDescriptor.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/Application/Application.hpp"

namespace Trident {
	class Renderer {
		public:
			static GraphicsAPI getGraphicsApi();

			static void initialize(const LayerCompositor* layerCompositor);

		public:
			virtual void build() = 0;

			/// Called after `build`.
			virtual StaticMemoryDescriptor getStaticMemoryDescriptor() const {
				return {};
			}

			virtual void postBuild() {}

			/// Called after `build` and before rendering the first frame.
			virtual void encodeOneTimeCommands() const {}

			virtual void render(ref<CommandEncoder> commandEncoder) const = 0;

			virtual void resize(WindowResizeEvent& event);

			virtual void onEvent(Event& event);

		public:
			template<typename ElementType>
			inline static ref<Buffer<ElementType>> createBuffer(const BufferDescriptor& descriptor) {
				return layerCompositor->device->createBuffer<ElementType>(descriptor);
			}

			/// Creates and returns a buffer suitable for memory transfers from the CPU to the GPU.
			template<typename ElementType>
			inline static ref<Buffer<ElementType>> createStagingBuffer(uint64 size) {
				return layerCompositor->device->createBuffer<ElementType>({
					.size = size,
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Staging",
					#endif
				});
			}

			static ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor);

			static ref<DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings);

			static ref<Fence> createFence(const FenceDescriptor& descriptor);

			static ref<Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor);

			static ref<Image> createImage(const ImageDescriptor& descriptor);

			static ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor);

			static ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor);

			static ref<GraphicsPipeline> createGraphicsPipeline(const GraphicsPipelineDescriptor& descriptor);

			static ref<Sampler> createSampler(const SamplerDescriptor& descriptor);

			static ref<Semaphore> createSemaphore();

			static ref<BaseBuffer> createStaticBuffer(const BufferDescriptor& descriptor);

			static ref<Image> createStaticImage(const ImageDescriptor& descriptor);

			static ref<Subpass> createSubpass(const SubpassDescriptor& descriptor);

			static ref<Device> getDevice();

			static uint32 getSwapchainImageIndex();

			static ref<Surface> getSurface();

			static ref<Swapchain> getSwapchain();

			static ref<ShaderModule> loadShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset);

			// TODO: Deprecate.
			static ref<ShaderModule> loadShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path);

			// TODO: Deprecate.
			static ref<ShaderModule> loadInternalShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path);

		public:
			void setLayer(const Layer* layer) {
				this->layer = layer;
			}

		protected:
			const Layer* getLayer() const {
				return layer;
			}

		private:
			static const LayerCompositor* layerCompositor;

			const Layer* layer;
	};
}