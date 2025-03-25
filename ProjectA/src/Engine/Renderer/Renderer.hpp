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
#include "Engine/Renderer/Texture.hpp"

typedef struct GLFWwindow GLFWwindow;

namespace ProjectA {
	class Renderer {
		public:
			static void registerWindow(GLFWwindow* window);

			static GraphicsAPI getGraphicsApi();

			static const scope<RendererAPI>& getBackend();

			static void setGraphicsApi(GraphicsAPI api);

			static void recreate();

		public:
			virtual void render() = 0;

			virtual void onEvent(Event& event) = 0;

		protected:
			static ref<Device> getDevice();

			static void getWindowExtent(sint32& extentWidth, sint32& extentHeight);

			static ref<Buffer> createBuffer(const BufferDescriptor& descriptor);

			static ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor);

			static ref<Fence> createFence();

			static ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor);

			static ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor);

			static ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor);

			static ref<Sampler> createSampler(const SamplerDescriptor& descriptor);

			static ref<Semaphore> createSemaphore();

			static ref<ShaderModule> createShaderModule(ShaderStage stage, const chain<uint8>& code);

			static ref<Texture> createTexture(const TextureDescriptor& descriptor);

		private:
			static GraphicsAPI graphicsApi;
			static GLFWwindow* window;
			static scope<RendererAPI> rendererApi;
	};
}