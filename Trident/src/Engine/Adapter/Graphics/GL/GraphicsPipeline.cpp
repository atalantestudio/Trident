// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/BlendFactor.hpp"
#include "Engine/Adapter/Graphics/GL/CompareOperation.hpp"
#include "Engine/Adapter/Graphics/GL/CullMode.hpp"
#include "Engine/Adapter/Graphics/GL/FrontFace.hpp"
#include "Engine/Adapter/Graphics/GL/GraphicsPipeline.hpp"
#include "Engine/Adapter/Graphics/GL/PolygonMode.hpp"
#include "Engine/Adapter/Graphics/GL/PrimitiveTopology.hpp"
#include "Engine/Adapter/Graphics/GL/RenderPass.hpp"
#include "Engine/Adapter/Graphics/GL/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/GL/VertexInputAttributeFormat.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::GraphicsPipeline::GraphicsPipeline(const Trident::Device* device) :
		Trident::GraphicsPipeline(device)
	{}

	ref<Trident::GraphicsPipeline> GL::createGraphicsPipeline(const Trident::Device* device, const GraphicsPipelineDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::GraphicsPipeline> graphicsPipeline = allocateRef<GL::GraphicsPipeline>(device);
		graphicsPipeline->program = glCreateProgram();
		graphicsPipeline->primitiveTopology = getPrimitiveTopology(descriptor.primitiveTopology);
		graphicsPipeline->frontFace = getFrontFace(descriptor.frontFace);
		graphicsPipeline->cullMode = getCullMode(descriptor.cullMode);
		graphicsPipeline->polygonModeFace = getPolygonModeFace(descriptor.cullMode);
		graphicsPipeline->polygonMode = getPolygonMode(descriptor.polygonMode);
		graphicsPipeline->lineWidth = descriptor.lineWidth;
		graphicsPipeline->minSampleShading = descriptor.minSampleShading;
		graphicsPipeline->vertexInputBindingStrides = sequence<GLsizei>(descriptor.vertexInputBindings.count());
		graphicsPipeline->drawBuffers = sequence<GL::GraphicsPipeline::DrawBuffer>(descriptor.colorBlendState.attachments.count());
		graphicsPipeline->depthTest = descriptor.depthStencilState.depthTest;
		graphicsPipeline->depthWrite = descriptor.depthStencilState.depthWrite;
		graphicsPipeline->depthFunc = getDepthFunc(descriptor.depthStencilState.compareOperation);
		graphicsPipeline->multisample = ref_cast<GL::RenderPass>(descriptor.renderPass)->multisample;
		graphicsPipeline->dynamicLineWidth = descriptor.dynamicState & DynamicState::LINE_WIDTH;
		graphicsPipeline->subpassIndex = descriptor.subpassIndex;

		const ref<GL::ShaderModule> vertexShaderModule = ref_cast<GL::ShaderModule>(descriptor.vertexShaderModule);
		const ref<GL::ShaderModule> fragmentShaderModule = ref_cast<GL::ShaderModule>(descriptor.fragmentShaderModule);

		glAttachShader(graphicsPipeline->program, vertexShaderModule->shader);
		glAttachShader(graphicsPipeline->program, fragmentShaderModule->shader);
		glLinkProgram(graphicsPipeline->program);

		GLint linkStatus;

		glGetProgramiv(graphicsPipeline->program, GL_LINK_STATUS, &linkStatus);

		if (linkStatus != GL_TRUE) {
			GLint infoLogLength;

			glGetProgramiv(graphicsPipeline->program, GL_INFO_LOG_LENGTH, &infoLogLength);

			DEBUG_ASSERT(infoLogLength > 0);

			sequence<GLchar> infoLog(infoLogLength);

			glGetProgramInfoLog(graphicsPipeline->program, infoLogLength, nullptr, &infoLog[0]);

			ADAPTER_ASSERT(false, "Program linking failed:\n[]", infoLog);

			return nullptr;
		}

		glDetachShader(graphicsPipeline->program, vertexShaderModule->shader);
		glDetachShader(graphicsPipeline->program, fragmentShaderModule->shader);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_0
			glGenVertexArrays(1, &graphicsPipeline->vertexArray);
			glBindVertexArray(graphicsPipeline->vertexArray);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		GLuint bindingindex = 0;
		GLuint attribindex = 0;

		while (bindingindex < descriptor.vertexInputBindings.count()) {
			const VertexInputBinding& vertexInputBinding = descriptor.vertexInputBindings[bindingindex];
			const std::vector<Trident::VertexInputAttribute>& attributes = vertexInputBinding.getAttributes();

			for (const Trident::VertexInputAttribute& attribute : attributes) {
				const GL::VertexInputAttribute _attribute = getAttribute(attribute.format);
				const GLuint relativeoffset = attribute.offset;

				#if OPENGL_VERSION >= OPENGL_VERSION_4_3
					switch (_attribute.type) {
						case GL_FLOAT: {
							glVertexAttribFormat(attribindex, _attribute.size, _attribute.type, _attribute.normalized, relativeoffset);

							break;
						}
						case GL_BYTE:
						case GL_UNSIGNED_BYTE:
						case GL_SHORT:
						case GL_UNSIGNED_SHORT:
						case GL_INT:
						case GL_UNSIGNED_INT:
						case GL_UNSIGNED_INT_2_10_10_10_REV: {
							if (_attribute.normalized) {
								glVertexAttribFormat(attribindex, _attribute.size, _attribute.type, _attribute.normalized, relativeoffset);
							}
							else {
								glVertexAttribIFormat(attribindex, _attribute.size, _attribute.type, relativeoffset);
							}

							break;
						}
						case GL_DOUBLE: {
							glVertexAttribLFormat(attribindex, _attribute.size, _attribute.type, relativeoffset);

							break;
						}
						default: {
							ADAPTER_ASSERT(false, "Unsupported vertex input attribute type.");

							break;
						}
					}

					glVertexAttribBinding(attribindex, bindingindex);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif

				glEnableVertexAttribArray(attribindex);

				if (vertexInputBinding.getInputRate() == InputRate::INSTANCE) {
					#if OPENGL_VERSION >= OPENGL_VERSION_4_3
						glVertexBindingDivisor(bindingindex, 1);
					#else
						// TODO(graphics/gl/graphics-pipeline): Revert to glVertexAttribDivisor.
						ENGINE_ERROR_UNSUPPORTED();
					#endif
				}

				attribindex += 1;
			}

			graphicsPipeline->vertexInputBindingStrides[bindingindex] = vertexInputBinding.stride;

			bindingindex += 1;
		}

		for (uint32 colorBlendAttachmentIndex = 0; colorBlendAttachmentIndex < descriptor.colorBlendState.attachments.count(); colorBlendAttachmentIndex += 1) {
			const ColorBlendAttachment& colorBlendAttachment = descriptor.colorBlendState.attachments[colorBlendAttachmentIndex];

			GL::GraphicsPipeline::DrawBuffer& drawBuffer = graphicsPipeline->drawBuffers[colorBlendAttachmentIndex];
			drawBuffer.blend = colorBlendAttachment.blend;
			drawBuffer.srcColorBlendFactor = getBlendFactor(colorBlendAttachment.srcColorBlendFactor);
			drawBuffer.dstColorBlendFactor = getBlendFactor(colorBlendAttachment.dstColorBlendFactor);
		}

		#if OPENGL_VERSION >= OPENGL_VERSION_4_3 && defined(DEBUG_OBJECT_NAME)
			if (descriptor.name.count() > 0) {
				glObjectLabel(GL_PROGRAM, graphicsPipeline->program, static_cast<GLsizei>(descriptor.name.count()), &descriptor.name[0]);
			}
		#endif

		return graphicsPipeline;
	}

	void GL::destroyGraphicsPipeline(const Trident::Device* device, const Trident::GraphicsPipeline* graphicsPipeline) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::GraphicsPipeline* _graphicsPipeline = static_cast<const GL::GraphicsPipeline*>(graphicsPipeline);

		glDeleteProgram(_graphicsPipeline->program);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_0
			glDeleteVertexArrays(1, &_graphicsPipeline->vertexArray);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}
}