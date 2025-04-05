#include "pch.hpp"

#include "Backend/OpenGL/CullMode.hpp"
#include "Backend/OpenGL/FrontFace.hpp"
#include "Backend/OpenGL/OpenGLRenderPipeline.hpp"
#include "Backend/OpenGL/OpenGLShaderModule.hpp"
#include "Backend/OpenGL/PrimitiveTopology.hpp"
#include "Backend/OpenGL/VertexInputAttributeType.hpp"

namespace ProjectA {
	OpenGLRenderPipeline::OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor) :
		layout(*std::static_pointer_cast<OpenGLPipelineLayout>(descriptor.pipelineLayout)),
		program(glCreateProgram()),
		primitiveTopology(getGlPrimitiveTopology(descriptor.primitiveTopology)),
		frontFace(getGlFrontFace(descriptor.frontFace)),
		cullMode(getGlCullMode(descriptor.cullMode))
	{
		PROFILE_SCOPE(FUNC_SIG);

		glCreateVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		const ref<OpenGLShaderModule> vertexShaderModule = std::static_pointer_cast<OpenGLShaderModule>(descriptor.vertexShaderModule);
		const ref<OpenGLShaderModule> fragmentShaderModule = std::static_pointer_cast<OpenGLShaderModule>(descriptor.fragmentShaderModule);

		glAttachShader(program, vertexShaderModule->object);
		glAttachShader(program, fragmentShaderModule->object);
		glLinkProgram(program);

		GLint linkStatus;

		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

		if (linkStatus != GL_TRUE) {
			GLint infoLogLength;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

			chain<GLchar> infoLog(infoLogLength);

			glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data());

			ENGINE_FATAL("Program linking failed.\n[]", infoLog.data());

			BREAKPOINT();
		}

		glDetachShader(program, vertexShaderModule->object);
		glDetachShader(program, fragmentShaderModule->object);

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
					ENGINE_ASSERT(false, "Vertex attribute type is not supported.");
			}

			glVertexAttribBinding(vertexInputAttribute.location, vertexInputAttribute.binding);
			glEnableVertexAttribArray(vertexInputAttribute.location);

			const VertexBinding& vertexBinding = descriptor.vertexBufferLayout.vertexBindings[vertexInputAttribute.binding];

			if (vertexBinding.inputRate == InputRate::INSTANCE) {
				glVertexBindingDivisor(vertexInputAttribute.location, 1);
			}
		}
	}

	OpenGLRenderPipeline::~OpenGLRenderPipeline() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteProgram(program);
		glDeleteVertexArrays(1, &vertexArray);
	}
}