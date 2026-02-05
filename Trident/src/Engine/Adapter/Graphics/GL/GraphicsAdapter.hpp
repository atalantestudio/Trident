// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Device.hpp"
#include "Engine/Adapter/Graphics/GL/Instance.hpp"
#include "Engine/Adapter/Graphics/GL/Surface.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/Application/ApplicationProperties.hpp"

namespace Trident {
	class GL::GraphicsAdapter final : public Trident::GraphicsAdapter {
		public:
			static constexpr GLenum defaultFramebufferDrawBuffer = GL_FRONT_LEFT;

		public:
			explicit GraphicsAdapter(const ProjectDescriptor& projectDescriptor, const Window& window, const ApplicationProperties& properties);

			ref<Trident::Buffer> createBuffer(const BufferDescriptor& descriptor) const final override;

			ref<Trident::ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) const final override;

			ref<Trident::DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const final override;

			ref<Trident::Fence> createFence(const FenceDescriptor& descriptor) const final override;

			ref<Trident::Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor) const final override;

			ref<Trident::Image> createImage(const ImageDescriptor& descriptor) const final override;

			ref<Trident::PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const final override;

			ref<Trident::RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const final override;

			ref<Trident::RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const final override;

			ref<Trident::Sampler> createSampler(const SamplerDescriptor& descriptor) const final override;

			ref<Semaphore> createSemaphore() const final override;

			ref<Trident::ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const final override;
			ref<Trident::ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) const final override;

			ref<Trident::Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const final override;

			ref<Trident::Device> getDevice() const final override;

			ref<Trident::Surface> getSurface() const final override;

		private:
			void loadProcedures() const;

			#if defined(DEBUG_ENABLE_MESSENGER)
				static void debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
			#endif

		private:
			ref<GL::Instance> instance;
			ref<GL::Device> device;
			ref<GL::Surface> surface;
	};
}