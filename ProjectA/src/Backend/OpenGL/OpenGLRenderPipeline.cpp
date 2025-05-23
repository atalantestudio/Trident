#include "pch.hpp"

#include "Backend/OpenGL/OpenGLRenderPipeline.hpp"
#include "Backend/OpenGL/OpenGLShaderModule.hpp"

#include "Backend/OpenGL/CompareOperation.hpp"
#include "Backend/OpenGL/CullMode.hpp"
#include "Backend/OpenGL/FrontFace.hpp"
#include "Backend/OpenGL/PolygonMode.hpp"
#include "Backend/OpenGL/PrimitiveTopology.hpp"
#include "Backend/OpenGL/VertexInputAttributeType.hpp"

namespace ProjectA {
	OpenGLRenderPipeline::OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor) :
		layout(*std::static_pointer_cast<OpenGLPipelineLayout>(descriptor.pipelineLayout)),
		program(glCreateProgram()),
		primitiveTopology(getGlPrimitiveTopology(descriptor.primitiveTopology)),
		frontFace(getGlFrontFace(descriptor.frontFace)),
		cullMode(getGlCullMode(descriptor.cullMode)),
		polygonModeFace(getGlPolygonModeFace(descriptor.cullMode)),
		polygonMode(getGlPolygonMode(descriptor.polygonMode)),
		lineWidth(descriptor.lineWidth),
		minSampleShading(descriptor.minSampleShading),
		depthTest(descriptor.depthStencilState.depthTest),
		depthWrite(descriptor.depthStencilState.depthWrite),
		depthFunc(getGlDepthFunc(descriptor.depthStencilState.compareOperation)),
		dynamicLineWidth(descriptor.dynamicState & DynamicState::LINE_WIDTH)
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

			std::vector<GLchar> infoLog(infoLogLength);

			glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data());

			ENGINE_ERROR("Program linking failed:\n[]", infoLog.data());
			BREAKPOINT();
		}

		glDetachShader(program, vertexShaderModule->object);
		glDetachShader(program, fragmentShaderModule->object);

		GLuint attribindex = 0;

		for (uint32 vertexInputBindingIndex = 0; vertexInputBindingIndex < descriptor.vertexInputBindings.size(); vertexInputBindingIndex += 1) {
			const VertexInputBinding& vertexInputBinding = descriptor.vertexInputBindings[vertexInputBindingIndex];
			const std::vector<VertexInputAttribute>& attributes = vertexInputBinding.getAttributes();

			for (const VertexInputAttribute& attribute : attributes) {
				const GLenum type = getGlType(attribute.type);
				const GLint size = getGlSize(attribute.type);
				const GLvoid* offset = (const GLvoid*) (const std::uintptr_t) attribute.offset;

				switch (type) {
					case GL_FLOAT:
						glVertexAttribFormat(attribindex, size, type, attribute.normalized, attribute.offset);

						break;
					case GL_BYTE:
					case GL_UNSIGNED_BYTE:
					case GL_SHORT:
					case GL_UNSIGNED_SHORT:
					case GL_INT:
					case GL_UNSIGNED_INT:
						glVertexAttribIFormat(attribindex, size, type, attribute.offset);

						break;
					case GL_DOUBLE:
						glVertexAttribLFormat(attribindex, size, type, attribute.offset);

						break;
					default:
						ENGINE_ASSERT(false, "Unsupported vertex input attribute type.");
				}

				glVertexAttribBinding(attribindex, vertexInputBindingIndex);
				glEnableVertexAttribArray(attribindex);

				if (vertexInputBinding.getInputRate() == InputRate::INSTANCE) {
					glVertexBindingDivisor(vertexInputBindingIndex, 1);
				}

				attribindex += 1;
			}
		}
	}

	OpenGLRenderPipeline::~OpenGLRenderPipeline() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteProgram(program);
		glDeleteVertexArrays(1, &vertexArray);
	}
}