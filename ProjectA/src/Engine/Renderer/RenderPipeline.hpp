#pragma once

#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/PrimitiveTopology.hpp"
#include "Engine/Renderer/ShaderModule.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

namespace ProjectA {
	struct RenderPipelineDescriptor {
		ref<ShaderModule> vertexShaderModule;
		ref<ShaderModule> fragmentShaderModule;
		PrimitiveTopology primitiveTopology;
	};

	class RenderPipeline {
		public:
			static ref<RenderPipeline> create(const RenderPipelineDescriptor& descriptor);

			virtual ~RenderPipeline() = default;

			// TODO: Remove
			virtual void bind() const = 0;
			virtual void unbind() const = 0;

			virtual const std::vector<ref<VertexBuffer>>& getVertexBuffers() const = 0;

			virtual const ref<IndexBuffer>& getIndexBuffer() const = 0;

			virtual void addVertexBuffer(const ref<VertexBuffer>& vertexBuffer) = 0;

			virtual void setIndexBuffer(const ref<IndexBuffer>& indexBuffer) = 0;

			virtual void setUniform(const std::string& name, int32 value) const = 0;
			virtual void setUniform(const std::string& name, const int32* value, uint32 count) const = 0;
			virtual void setUniform(const std::string& name, float32 value) const = 0;
			virtual void setUniform(const std::string& name, const lyah::vec2<float32>& value) const = 0;
			virtual void setUniform(const std::string& name, const lyah::vec3<float32>& value) const = 0;
			virtual void setUniform(const std::string& name, const lyah::vec4<float32>& value) const = 0;
			virtual void setUniform(const std::string& name, const lyah::mat4x4<float32>& value) const = 0;
	};
}