#include "pch.hpp"

#include "Backend/OpenGL/DescriptorType.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"
#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"
#include "Backend/OpenGL/OpenGLRenderPipeline.hpp"
#include "Backend/OpenGL/OpenGLSampler.hpp"
#include "Backend/OpenGL/OpenGLShaderModule.hpp"
#include "Backend/OpenGL/OpenGLTextureView.hpp"
#include "Backend/OpenGL/VertexInputAttributeType.hpp"

namespace ProjectA {
	OpenGLRenderPipeline::OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor) :
		program(glCreateProgram()),
		primitiveTopology(primitiveTopologies[static_cast<uint32>(descriptor.primitiveTopology)]),
		frontFace(frontFaces[static_cast<uint32>(descriptor.frontFace)]),
		cullMode(cullModes[static_cast<uint32>(descriptor.cullMode)])
	{
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[OpenGLRenderPipeline]");

		glCreateVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		const GLuint vertexShaderModule = static_cast<const OpenGLShaderModule*>(descriptor.vertexShaderModule.get())->object;
		const GLuint fragmentShaderModule = static_cast<const OpenGLShaderModule*>(descriptor.fragmentShaderModule.get())->object;

		glAttachShader(program, vertexShaderModule);
		glAttachShader(program, fragmentShaderModule);
		glLinkProgram(program);

		GLint linkStatus;

		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

		if (linkStatus == 0) {
			GLint infoLogLength;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

			std::vector<GLchar> infoLog(infoLogLength);

			glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data());

			ENGINE_FATAL("Program linking failed.\n[]", infoLog.data());

			throw std::runtime_error("Failed to link OpenGL program.");
		}

		glDetachShader(program, vertexShaderModule);
		glDetachShader(program, fragmentShaderModule);

		for (const VertexAttribute& vertexInputAttribute : descriptor.vertexBufferLayout.vertexAttributes) {
			const GLenum type = getGlType(vertexInputAttribute.type);
			const GLint size = getGlSize(vertexInputAttribute.type);
			const GLvoid* offset = reinterpret_cast<const GLvoid*>(static_cast<std::uintptr_t>(vertexInputAttribute.offset));

			switch (type) {
				case GL_FLOAT:
					glVertexAttribFormat(vertexInputAttribute.location, size, type, vertexInputAttribute.normalized, vertexInputAttribute.offset);

					break;
				case GL_BYTE:
				case GL_UNSIGNED_BYTE:
				case GL_SHORT:
				case GL_UNSIGNED_SHORT:
				case GL_INT:
				case GL_UNSIGNED_INT:
					glVertexAttribIFormat(vertexInputAttribute.location, size, type, vertexInputAttribute.offset);

					break;
				case GL_DOUBLE:
					glVertexAttribLFormat(vertexInputAttribute.location, size, type, vertexInputAttribute.offset);

					break;
				default:
					throw std::runtime_error("Vertex attribute type is not supported.");
			}

			glVertexAttribBinding(vertexInputAttribute.location, vertexInputAttribute.binding);
			glEnableVertexAttribArray(vertexInputAttribute.location);

			const VertexBinding& vertexBinding = descriptor.vertexBufferLayout.vertexBindings[vertexInputAttribute.binding];

			if (vertexBinding.inputRate == InputRate::INSTANCE) {
				glVertexBindingDivisor(vertexInputAttribute.location, 1);
			}
		}

		const OpenGLPipelineLayout* pipelineLayout = static_cast<const OpenGLPipelineLayout*>(descriptor.pipelineLayout.get());

		// GLSL shaders are required to specify the block index and the binding,
		// saving the calls to glGetUniformBlockIndex and glUniformBlockBinding.
		for (uint32 setIndex = 0; setIndex < pipelineLayout->descriptorSetLayouts.size(); setIndex += 1) {
			const DescriptorSetLayout& descriptorSetLayout = pipelineLayout->descriptorSetLayouts[setIndex];

			for (uint32 bindingIndex = 0; bindingIndex < descriptorSetLayout.bindings.size(); bindingIndex += 1) {
				const DescriptorSetLayoutBinding& binding = descriptorSetLayout.bindings[bindingIndex];

				for (const ref<Buffer>& buffer : binding.buffers) {
					const OpenGLBuffer* glBuffer = static_cast<const OpenGLBuffer*>(buffer.get());

					/*switch (glBuffer->target) {
						case GL_UNIFORM_BUFFER:
							glUniformBlockBinding(program, setIndex, bindingIndex);

							break;
						case GL_SHADER_STORAGE_BUFFER:
							glShaderStorageBlockBinding(program, setIndex, bindingIndex);

							break;
						default:
							throw std::runtime_error("Buffer target not supported.");
					}*/

					glBindBufferRange(getGlDescriptorType(binding.descriptorType), bindingIndex, glBuffer->object, 0, glBuffer->size);
				}

				for (uint32 textureViewIndex = 0; textureViewIndex < binding.textureViews.size(); textureViewIndex += 1) {
					const OpenGLTextureView* textureView = static_cast<const OpenGLTextureView*>(binding.textureViews[textureViewIndex].get());
					const OpenGLSampler* sampler = static_cast<const OpenGLSampler*>(binding.samplers[textureViewIndex].get());

					glBindTextureUnit(textureViewIndex, textureView->object);
					glBindSampler(textureViewIndex, sampler->object);
				}
			}
		}
	}

	OpenGLRenderPipeline::~OpenGLRenderPipeline() {
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[~OpenGLRenderPipeline]");

		glDeleteProgram(program);
		glDeleteVertexArrays(1, &vertexArray);
	}
}