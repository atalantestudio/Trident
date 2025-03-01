#pragma once

#include "Engine/Renderer/Buffer/BufferLayout.hpp"
#include "Engine/Renderer/PipelineLayout.hpp"
#include "Engine/Renderer/PrimitiveTopology.hpp"
#include "Engine/Renderer/RenderPass.hpp"
#include "Engine/Renderer/ShaderModule.hpp"
#include "Engine/Renderer/Swapchain.hpp"

namespace ProjectA {
	struct RenderPipelineDescriptor {
		ref<RenderPass> renderPass;
		ref<Swapchain> swapchain;
		ref<PipelineLayout> pipelineLayout;
		BufferLayout vertexBufferLayout;
		ref<ShaderModule> vertexShaderModule;
		ref<ShaderModule> fragmentShaderModule;
		PrimitiveTopology primitiveTopology;
	};

	class RenderPipeline {
		public:
			virtual ~RenderPipeline() = default;

			// TODO: Remove
			virtual void bind() const = 0;
			virtual void unbind() const = 0;

			virtual void setUniform(const std::string& name, int32 value) const = 0;
			virtual void setUniform(const std::string& name, const int32* value, uint32 count) const = 0;
			virtual void setUniform(const std::string& name, float32 value) const = 0;
			virtual void setUniform(const std::string& name, const vec2<float32>& value) const = 0;
			virtual void setUniform(const std::string& name, const vec3<float32>& value) const = 0;
			virtual void setUniform(const std::string& name, const vec4<float32>& value) const = 0;
			virtual void setUniform(const std::string& name, const mat4x4<float32>& value) const = 0;
	};
}