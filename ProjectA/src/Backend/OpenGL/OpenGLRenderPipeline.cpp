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

		for (uint32 vertexInputBindingIndex = 0; vertexInputBindingIndex < descriptor.vertexInputBindings.size(); vertexInputBindingIndex += 1) {
			const VertexInputBinding& vertexInputBinding = descriptor.vertexInputBindings[vertexInputBindingIndex];
			const chain<VertexInputAttribute>& attributes = vertexInputBinding.getAttributes();

			for (uint32 attributeIndex = 0; attributeIndex < attributes.size(); attributeIndex += 1) {
				const VertexInputAttribute& attribute = attributes[attributeIndex];

				const GLenum type = getGlType(attribute.type);
				const GLint size = getGlSize(attribute.type);
				const GLvoid* offset = (const GLvoid*) (const std::uintptr_t) attribute.offset;

				switch (type) {
					case GL_FLOAT:
						glVertexAttribFormat(attribute.location, size, type, attribute.normalized, attribute.offset);

						break;
					case GL_BYTE:
					case GL_UNSIGNED_BYTE:
					case GL_SHORT:
					case GL_UNSIGNED_SHORT:
					case GL_INT:
					case GL_UNSIGNED_INT:
						glVertexAttribIFormat(attribute.location, size, type, attribute.offset);

						break;
					case GL_DOUBLE:
						glVertexAttribLFormat(attribute.location, size, type, attribute.offset);

						break;
					default:
						ENGINE_ASSERT(false, "Unsupported vertex input attribute type.");
				}

				glVertexAttribBinding(attribute.location, vertexInputBindingIndex);
				glEnableVertexAttribArray(attribute.location);

				if (vertexInputBinding.getInputRate() == InputRate::INSTANCE) {
					glVertexBindingDivisor(attribute.location, 1);
				}
			}
		}
	}

	OpenGLRenderPipeline::~OpenGLRenderPipeline() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteProgram(program);
		glDeleteVertexArrays(1, &vertexArray);
	}
}