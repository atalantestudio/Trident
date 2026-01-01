#include "pch.hpp"

#include "Backend/GL/RenderPipeline.hpp"
#include "Backend/GL/ShaderModule.hpp"

#include "Backend/GL/BlendFactor.hpp"
#include "Backend/GL/CompareOperation.hpp"
#include "Backend/GL/CullMode.hpp"
#include "Backend/GL/FrontFace.hpp"
#include "Backend/GL/PolygonMode.hpp"
#include "Backend/GL/PrimitiveTopology.hpp"
#include "Backend/GL/VertexInputAttributeType.hpp"

namespace ProjectA {
	OpenGLRenderPipeline::OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor) :
		RenderPipeline(descriptor),
		program(glCreateProgram()),
		primitiveTopology(getGlPrimitiveTopology(descriptor.primitiveTopology)),
		frontFace(getGlFrontFace(descriptor.frontFace)),
		cullMode(getGlCullMode(descriptor.cullMode)),
		polygonModeFace(getGlPolygonModeFace(descriptor.cullMode)),
		polygonMode(getGlPolygonMode(descriptor.polygonMode)),
		lineWidth(descriptor.lineWidth),
		minSampleShading(descriptor.minSampleShading),
		drawBuffers(descriptor.colorBlendState.attachments.size()),
		depthTest(descriptor.depthStencilState.depthTest),
		depthWrite(descriptor.depthStencilState.depthWrite),
		depthFunc(getGlDepthFunc(descriptor.depthStencilState.compareOperation)),
		dynamicLineWidth(descriptor.dynamicState & DynamicState::LINE_WIDTH),
		subpassIndex(descriptor.subpassIndex)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		if (descriptor.cullMode == CullMode::FRONT_AND_BACK) {
			ENGINE_WARNING("[OpenGL] The polygon mode is discarded because the cull mode is FRONT_AND_BACK.");
		}

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

			DEBUG_ASSERT(infoLogLength > 0);

			sequence<GLchar> infoLog(infoLogLength);

			glGetProgramInfoLog(program, infoLogLength, nullptr, &infoLog[0]);

			ENGINE_ERROR("Program linking failed:\n[]", infoLog);

			DEBUG_ASSERT(false);

			return;
		}

		glDetachShader(program, vertexShaderModule->object);
		glDetachShader(program, fragmentShaderModule->object);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			glCreateVertexArrays(1, &vaobj);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
			glGenVertexArrays(1, &vaobj);
			glBindVertexArray(vaobj);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		GLuint attribindex = 0;

		for (uint32 bindingindex = 0; bindingindex < descriptor.vertexInputBindings.count(); bindingindex += 1) {
			const VertexInputBinding& vertexInputBinding = descriptor.vertexInputBindings[bindingindex];
			const std::vector<VertexInputAttribute>& attributes = vertexInputBinding.getAttributes();

			for (const VertexInputAttribute& attribute : attributes) {
				const GLenum type = getGlType(attribute.type);
				const GLint size = getGlSize(attribute.type);
				const GLuint relativeoffset = attribute.offset;

				switch (type) {
					case GL_FLOAT:
						#if OPENGL_VERSION >= OPENGL_VERSION_4_5
							glVertexArrayAttribFormat(vaobj, attribindex, size, type, GL_FALSE, relativeoffset);
						#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
							glVertexAttribFormat(attribindex, size, type, GL_FALSE, relativeoffset);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif

						break;
					case GL_BYTE:
					case GL_UNSIGNED_BYTE:
					case GL_SHORT:
					case GL_UNSIGNED_SHORT:
					case GL_INT:
					case GL_UNSIGNED_INT:
						#if OPENGL_VERSION >= OPENGL_VERSION_4_5
							glVertexArrayAttribIFormat(vaobj, attribindex, size, type, relativeoffset);
						#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
							glVertexAttribIFormat(attribindex, size, type, relativeoffset);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif

						break;
					case GL_DOUBLE:
						#if OPENGL_VERSION >= OPENGL_VERSION_4_5
							glVertexArrayAttribLFormat(vaobj, attribindex, size, type, relativeoffset);
						#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
							glVertexAttribLFormat(attribindex, size, type, relativeoffset);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif

						break;
					default:
						ENGINE_ASSERT(false, "Unsupported vertex input attribute type.");
				}

				#if OPENGL_VERSION >= OPENGL_VERSION_4_5
					glVertexArrayAttribBinding(vaobj, attribindex, bindingindex);
				#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
					glVertexAttribBinding(attribindex, bindingindex);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif

				#if OPENGL_VERSION >= OPENGL_VERSION_4_5
					glEnableVertexArrayAttrib(vaobj, attribindex);
				#else
					glEnableVertexAttribArray(attribindex);
				#endif

				if (vertexInputBinding.getInputRate() == InputRate::INSTANCE) {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_5
						glVertexArrayBindingDivisor(vaobj, bindingindex, 1);
					#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
						glVertexBindingDivisor(bindingindex, 1);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}

				attribindex += 1;
			}
		}

		for (uint32 colorBlendAttachmentIndex = 0; colorBlendAttachmentIndex < descriptor.colorBlendState.attachments.size(); colorBlendAttachmentIndex += 1) {
			const ColorBlendAttachment& colorBlendAttachment = descriptor.colorBlendState.attachments[colorBlendAttachmentIndex];

			OpenGLDrawBuffer& drawBuffer = drawBuffers[colorBlendAttachmentIndex];
			drawBuffer.blend = colorBlendAttachment.blend;
			drawBuffer.srcColorBlendFactor = getGlBlendFactor(colorBlendAttachment.srcColorBlendFactor);
			drawBuffer.dstColorBlendFactor = getGlBlendFactor(colorBlendAttachment.dstColorBlendFactor);
		}

		#if OPENGL_VERSION < OPENGL_VERSION_4_5 && defined(OPENGL_UNBIND_AFTER_USE)
			#if OPENGL_VERSION >= OPENGL_VERSION_3_0
				glBindVertexArray(0);
			#else
				ENGINE_ERROR_UNSUPPORTED();
			#endif
		#endif
	}

	OpenGLRenderPipeline::~OpenGLRenderPipeline() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glDeleteProgram(program);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_0
			glDeleteVertexArrays(1, &vaobj);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}
}