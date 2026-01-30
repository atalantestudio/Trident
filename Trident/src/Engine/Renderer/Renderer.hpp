// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Event/Window/WindowResizeEvent.hpp"
#include "Engine/API/Rendering/CommandEncoder.hpp"
#include "Engine/API/Rendering/Descriptor/PipelineLayoutDescriptor.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/Application/Application.hpp"

namespace Trident {
	class Renderer {
		public:
			static GraphicsAPI getGraphicsApi();

			static void initialize(const ProjectDescriptor& projectDescriptor, const Window& window, const ApplicationProperties& properties);

			static void deinitialize();

		public:
			virtual void build() = 0;

			virtual void render(ref<CommandEncoder> commandEncoder) const = 0;

			virtual void resize(WindowResizeEvent& event);

			virtual void onEvent(Event& event);

		public:
			static ref<Buffer> createBuffer(const BufferDescriptor& descriptor);

			// Creates and returns a staging buffer.
			// `size` is in bytes.
			// If `persistent` is true, the buffer will stay mapped until it is destroyed.
			static ref<Buffer> createStagingBuffer(uint64 size, bool persistent = false);

			static ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor);

			static ref<DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings);

			static ref<Fence> createFence(const FenceDescriptor& descriptor);

			static ref<Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor);

			static ref<Image> createImage(const ImageDescriptor& descriptor);

			static ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor);

			static ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor);

			static ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor);

			static ref<Sampler> createSampler(const SamplerDescriptor& descriptor);

			static ref<Semaphore> createSemaphore();

			static ref<Subpass> createSubpass(const SubpassDescriptor& descriptor);

			static ref<Semaphore> getAcquireNextImageSemaphore(uint32 frameInFlightIndex);

			static ref<CommandEncoder> getCommandEncoder(uint32 frameInFlightIndex);

			static ref<Device> getDevice();

			static ref<Semaphore> getFinishedRenderSemaphore(uint32 frameInFlightIndex);

			static ref<Fence> getFlightFence(uint32 frameInFlightIndex);

			static uint32 getImageIndex();

			static ref<Surface> getSurface();

			static ref<Swapchain> getSwapchain();

			static ref<ShaderModule> loadShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset);

			// TODO: Deprecate.
			static ref<ShaderModule> loadShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path);

			// TODO: Deprecate.
			static ref<ShaderModule> loadInternalShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path);

			static void setImageIndex(uint32 imageIndex);

		public:
			void setLayer(const Layer* layer) {
				this->layer = layer;
			}

		protected:
			const Layer* getLayer() const {
				return layer;
			}

		private:
			static GraphicsAPI graphicsApi;
			static scope<GraphicsAdapter> adapter;
			static uint32 imageIndex;
			static ref<Swapchain> swapchain;
			static sequence<ref<CommandEncoder>> commandEncoders;
			static sequence<ref<Fence>> flightFences;
			static sequence<ref<Semaphore>> acquireNextImageSemaphores;
			static sequence<ref<Semaphore>> finishedRenderSemaphores;

		private:
			const Layer* layer;
	};
}