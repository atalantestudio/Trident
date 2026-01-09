// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Device.hpp"
#include "Engine/Adapter/Graphics/GL/Instance.hpp"
#include "Engine/Adapter/Graphics/GL/Surface.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/Application/ApplicationProperties.hpp"

namespace ProjectA {
	class GL::GraphicsAdapter final : public ProjectA::GraphicsAdapter {
		public:
			static constexpr GLenum defaultFramebufferDrawBuffer = GL_FRONT_LEFT;

		public:
			explicit GraphicsAdapter(const ProjectDescriptor& projectDescriptor, const Window& window, const ApplicationProperties& properties);

			ref<ProjectA::Buffer> createBuffer(const BufferDescriptor& descriptor) const final override;

			ref<ProjectA::ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) const final override;

			ref<ProjectA::DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const final override;

			ref<ProjectA::Fence> createFence() const final override;

			ref<ProjectA::Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor) const final override;

			ref<ProjectA::Image> createImage(const ImageDescriptor& descriptor) const final override;

			ref<ProjectA::PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const final override;

			ref<ProjectA::RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const final override;

			ref<ProjectA::RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const final override;

			ref<ProjectA::Sampler> createSampler(const SamplerDescriptor& descriptor) const final override;

			ref<Semaphore> createSemaphore() const final override;

			ref<ProjectA::ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const final override;
			ref<ProjectA::ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) const final override;

			ref<ProjectA::Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const final override;

			ref<ProjectA::Device> getDevice() const final override;

			ref<ProjectA::Surface> getSurface() const final override;

		private:
			void loadProcedures() const;

			#ifdef DEBUG_ENABLE_MESSENGER
				static void debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
			#endif

		private:
			ref<GL::Instance> instance;
			ref<GL::Device> device;
			ref<GL::Surface> surface;
	};
}