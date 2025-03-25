#pragma once

#include "Engine/Event/Event.hpp"
#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/CommandEncoder.hpp"
#include "Engine/Renderer/Device.hpp"
#include "Engine/Renderer/GraphicsAPI.hpp"
#include "Engine/Renderer/PipelineLayout.hpp"
#include "Engine/Renderer/RendererAPI.hpp"
#include "Engine/Renderer/RenderPass.hpp"
#include "Engine/Renderer/ShaderModule.hpp"
#include "Engine/Renderer/Swapchain.hpp"
#include "Engine/Renderer/Texture.hpp"

typedef struct GLFWwindow GLFWwindow;

namespace ProjectA {
	class Renderer {
		public:
			static void registerWindow(GLFWwindow* window);

			static GraphicsAPI getGraphicsApi();

			static void setGraphicsApi(GraphicsAPI api);

			static void recreate();

		public:
			virtual void render() = 0;

			virtual void onEvent(Event& event) = 0;

		protected:
			ref<Device> getDevice() const;

			void getWindowExtent(sint32& extentWidth, sint32& extentHeight) const;

			ref<Buffer> createBuffer(const BufferDescriptor& descriptor) const;

			ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const;

			ref<Fence> createFence() const;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const;

			ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const;

			ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const;

			ref<Semaphore> createSemaphore() const;

			ref<ShaderModule> createShaderModule(ShaderStage stage, const chain<uint8>& code) const;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const;

			ref<Texture> createTexture(const TextureDescriptor& descriptor) const;

		private:
			static GraphicsAPI graphicsApi;
			static GLFWwindow* window;
			static scope<RendererAPI> rendererApi;
	};
}