// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/BlendFactor.hpp"
#include "Engine/Adapter/Graphics/GL/CompareOperation.hpp"
#include "Engine/Adapter/Graphics/GL/CullMode.hpp"
#include "Engine/Adapter/Graphics/GL/FrontFace.hpp"
#include "Engine/Adapter/Graphics/GL/PolygonMode.hpp"
#include "Engine/Adapter/Graphics/GL/PrimitiveTopology.hpp"
#include "Engine/Adapter/Graphics/GL/RenderPass.hpp"
#include "Engine/Adapter/Graphics/GL/RenderPipeline.hpp"
#include "Engine/Adapter/Graphics/GL/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/GL/VertexInputAttributeType.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::RenderPipeline::RenderPipeline(const RenderPipelineDescriptor& descriptor) :
		Trident::RenderPipeline(descriptor),
		program(glCreateProgram()),
		primitiveTopology(getPrimitiveTopology(descriptor.primitiveTopology)),
		frontFace(getFrontFace(descriptor.frontFace)),
		cullMode(getCullMode(descriptor.cullMode)),
		polygonModeFace(getPolygonModeFace(descriptor.cullMode)),
		polygonMode(getPolygonMode(descriptor.polygonMode)),
		lineWidth(descriptor.lineWidth),
		minSampleShading(descriptor.minSampleShading),
		drawBuffers(descriptor.colorBlendState.attachments.count()),
		depthTest(descriptor.depthStencilState.depthTest),
		depthWrite(descriptor.depthStencilState.depthWrite),
		depthFunc(getDepthFunc(descriptor.depthStencilState.compareOperation)),
		multisample(std::static_pointer_cast<GL::RenderPass>(descriptor.renderPass)->multisample),
		dynamicLineWidth(descriptor.dynamicState & DynamicState::LINE_WIDTH),
		subpassIndex(descriptor.subpassIndex)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		if (descriptor.cullMode == CullMode::FRONT_AND_BACK) {
			ADAPTER_WARNING("Cull mode is FRONT_AND_BACK; discarding polygon mode.");
		}

		const ref<GL::ShaderModule> vertexShaderModule = std::static_pointer_cast<GL::ShaderModule>(descriptor.vertexShaderModule);
		const ref<GL::ShaderModule> fragmentShaderModule = std::static_pointer_cast<GL::ShaderModule>(descriptor.fragmentShaderModule);

		glAttachShader(program, vertexShaderModule->shader);
		glAttachShader(program, fragmentShaderModule->shader);
		glLinkProgram(program);

		GLint linkStatus;

		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

		if (linkStatus != GL_TRUE) {
			GLint infoLogLength;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

			DEBUG_ASSERT(infoLogLength > 0);

			sequence<GLchar> infoLog(infoLogLength);

			glGetProgramInfoLog(program, infoLogLength, nullptr, &infoLog[0]);

			ADAPTER_ASSERT(false, "Program linking failed:\n[]", infoLog);

			return;
		}

		glDetachShader(program, vertexShaderModule->shader);
		glDetachShader(program, fragmentShaderModule->shader);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			glCreateVertexArrays(1, &vertexArray);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
			glGenVertexArrays(1, &vertexArray);
			glBindVertexArray(vertexArray);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		GLuint attribindex = 0;

		for (uint32 bindingindex = 0; bindingindex < descriptor.vertexInputBindings.count(); bindingindex += 1) {
			const VertexInputBinding& vertexInputBinding = descriptor.vertexInputBindings[bindingindex];
			const std::vector<VertexInputAttribute>& attributes = vertexInputBinding.getAttributes();

			for (const VertexInputAttribute& attribute : attributes) {
				const GLenum type = getType(attribute.type);
				const GLint size = getSize(attribute.type);
				const GLuint relativeoffset = attribute.offset;

				switch (type) {
					case GL_FLOAT:
						#if OPENGL_VERSION >= OPENGL_VERSION_4_5
							glVertexArrayAttribFormat(vertexArray, attribindex, size, type, GL_FALSE, relativeoffset);
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
							glVertexArrayAttribIFormat(vertexArray, attribindex, size, type, relativeoffset);
						#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
							glVertexAttribIFormat(attribindex, size, type, relativeoffset);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif

						break;
					case GL_DOUBLE:
						#if OPENGL_VERSION >= OPENGL_VERSION_4_5
							glVertexArrayAttribLFormat(vertexArray, attribindex, size, type, relativeoffset);
						#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
							glVertexAttribLFormat(attribindex, size, type, relativeoffset);
						#else
							ENGINE_ERROR_UNSUPPORTED();
						#endif

						break;
					default:
						ADAPTER_ASSERT(false, "Unsupported vertex input attribute type.");
				}

				#if OPENGL_VERSION >= OPENGL_VERSION_4_5
					glVertexArrayAttribBinding(vertexArray, attribindex, bindingindex);
				#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
					glVertexAttribBinding(attribindex, bindingindex);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif

				#if OPENGL_VERSION >= OPENGL_VERSION_4_5
					glEnableVertexArrayAttrib(vertexArray, attribindex);
				#else
					glEnableVertexAttribArray(attribindex);
				#endif

				if (vertexInputBinding.getInputRate() == InputRate::INSTANCE) {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_5
						glVertexArrayBindingDivisor(vertexArray, bindingindex, 1);
					#elif OPENGL_VERSION >= OPENGL_VERSION_4_3
						glVertexBindingDivisor(bindingindex, 1);
					#else
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}

				attribindex += 1;
			}
		}

		for (uint32 colorBlendAttachmentIndex = 0; colorBlendAttachmentIndex < descriptor.colorBlendState.attachments.count(); colorBlendAttachmentIndex += 1) {
			const ColorBlendAttachment& colorBlendAttachment = descriptor.colorBlendState.attachments[colorBlendAttachmentIndex];

			DrawBuffer& drawBuffer = drawBuffers[colorBlendAttachmentIndex];
			drawBuffer.blend = colorBlendAttachment.blend;
			drawBuffer.srcColorBlendFactor = getBlendFactor(colorBlendAttachment.srcColorBlendFactor);
			drawBuffer.dstColorBlendFactor = getBlendFactor(colorBlendAttachment.dstColorBlendFactor);
		}

		#if OPENGL_VERSION < OPENGL_VERSION_4_5 && defined(OPENGL_UNBIND_AFTER_USE)
			#if OPENGL_VERSION >= OPENGL_VERSION_3_0
				glBindVertexArray(0);
			#else
				ENGINE_ERROR_UNSUPPORTED();
			#endif
		#endif
	}

	GL::RenderPipeline::~RenderPipeline() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glDeleteProgram(program);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_0
			glDeleteVertexArrays(1, &vertexArray);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}
}