#pragma once

#include "pch.hpp"

#include "Engine/Camera/Camera.hpp"
#include "Engine/Renderer/GraphicsAPI.hpp"
#include "Engine/Renderer/RendererAPI.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"

namespace ProjectA {
	class Renderer {
		public:
			static GraphicsAPI graphicsApi;

			static const Camera* camera;

		public:
			static void initialize();

			static void destroy();

			static GraphicsAPI getGraphicsApi();

			static scope<RendererAPI>& getRendererApi();

			//static void submit(const ref<RenderPipeline>& renderPipeline);

			static void onWindowResize(uint32 width, uint32 height);

			// TODO: RenderPassCommandEncoder

			// OpenGL: drawArrays/drawArraysInstanced
			// void draw(const uint32 vertexCount, const uint32 instanceCount, const uint32 vertexOffset) const;

		private:
			static scope<RendererAPI> rendererApi;

		// --- Start of new Renderer parent class ---

		// TODO: Make protected
		public:
			// TODO: Make non-static (and const)
			static ref<ShaderModule> createShaderModule(const ShaderStage stage, const std::string& code);

			// TODO: Make non-static (and const)
			static ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor);

		public:
			virtual void render() const = 0;
	};
}