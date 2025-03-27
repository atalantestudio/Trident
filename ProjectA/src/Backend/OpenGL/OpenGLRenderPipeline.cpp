#include "pch.hpp"

#include "Backend/OpenGL/OpenGLRenderPipeline.hpp"
#include "Backend/OpenGL/OpenGLShaderModule.hpp"
#include "Backend/OpenGL/VertexInputAttributeType.hpp"

namespace ProjectA {
	OpenGLRenderPipeline::OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor) :
		program(glCreateProgram()),
		primitiveTopology(primitiveTopologies[static_cast<uint32>(descriptor.primitiveTopology)]),
		frontFace(frontFaces[static_cast<uint32>(descriptor.frontFace)]),
		cullMode(cullModes[static_cast<uint32>(descriptor.cullMode)])
	{
		PROFILE_SCOPE(FUNCSIG);

		glCreateVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		const GLuint vertexShaderModule = static_cast<const OpenGLShaderModule*>(descriptor.vertexShaderModule.get())->object;
		const GLuint fragmentShaderModule = static_cast<const OpenGLShaderModule*>(descriptor.fragmentShaderModule.get())->object;

		glAttachShader(program, vertexShaderModule);
		glAttachShader(program, fragmentShaderModule);
		glLinkProgram(program);

		GLint linkStatus;

		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

		if (linkStatus == GL_FALSE) {
			GLint infoLogLength;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

			chain<GLchar> infoLog(infoLogLength);

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
	}

	OpenGLRenderPipeline::~OpenGLRenderPipeline() {
		PROFILE_SCOPE(FUNCSIG);

		glDeleteProgram(program);
		glDeleteVertexArrays(1, &vertexArray);
	}
}