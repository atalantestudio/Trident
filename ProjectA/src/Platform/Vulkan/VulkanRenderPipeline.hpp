#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"

namespace ProjectA {
	class VulkanRenderPipeline : public RenderPipeline, public ObjectWrapper<VkPipeline> {
		public:
			VulkanRenderPipeline(const VkDevice device, const VkRenderPass renderPass, const RenderPipelineDescriptor& descriptor);

			~VulkanRenderPipeline();

			// TODO: Remove
			void bind() const override;
			void unbind() const override;

			// TODO: Remove
			const std::vector<ref<VertexBuffer>>& getVertexBuffers() const override;

			// TODO: Remove
			const ref<IndexBuffer>& getIndexBuffer() const override;

			// TODO: Remove
			void addVertexBuffer(const ref<VertexBuffer>& vertexBuffer) override;

			// TODO: Remove
			void setIndexBuffer(const ref<IndexBuffer>& indexBuffer) override;

			// TODO: Remove
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