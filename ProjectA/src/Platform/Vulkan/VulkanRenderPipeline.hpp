#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"

#include "Platform/Vulkan/VulkanSwapchain.hpp"

namespace ProjectA {
	class VulkanRenderPipeline : public RenderPipeline, public ObjectWrapper<VkPipeline> {
		public:
			VulkanRenderPipeline(const VkDevice device, const RenderPipelineDescriptor& descriptor);

			~VulkanRenderPipeline();

			// --- TODO: Remove these implementations ---

			void bind() const override;
			void unbind() const override;

			void setUniform(const std::string& name, int32 value) const override;
			void setUniform(const std::string& name, const int32* value, uint32 count) const override;
			void setUniform(const std::string& name, float32 value) const override;
			void setUniform(const std::string& name, const vec2<float32>& value) const override;
			void setUniform(const std::string& name, const vec3<float32>& value) const override;
			void setUniform(const std::string& name, const vec4<float32>& value) const override;
			void setUniform(const std::string& name, const mat4x4<float32>& value) const override;

		private:
			const VkDevice device;
	};
}