#pragma once

#include "Engine/Renderer/RendererAPI.hpp"

#include "Platform/Vulkan/VulkanDevice.hpp"
#include "Platform/Vulkan/VulkanInstance.hpp"

namespace ProjectA {
	class VulkanRendererAPI : public RendererAPI {
		public:
			VulkanRendererAPI();

			// TODO: Implement
			void setViewport(int32 x, int32, uint32 width, uint32 height) const override {}

			// TODO: Implement
			void setClearColor(const vec4<float32>& color) const override {}

			// TODO: Implement
			void clear() const override {}

			// TODO: Implement
			void drawIndexed(const ref<RenderPipeline>& renderPipeline, uint32 indexCount) const override {}

			ref<ShaderModule> createShaderModule(const ShaderStage stage, const std::string& code) const override;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const override;

		private:
			const VulkanInstance instance;
			const VulkanDevice device;
	};
}