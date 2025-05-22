#include "pch.hpp"

#include "Backend/OpenGL/OpenGLRenderPass.hpp"
#include "Backend/OpenGL/OpenGLShaderModule.hpp"

#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	OpenGLRenderPass::OpenGLRenderPass(const RenderPassDescriptor& descriptor) :
		RenderPass(descriptor.depthStencilAttachment.enabled)
	{
		for (const Attachment& colorAttachment : descriptor.colorAttachments) {
			attachments.emplace_back(colorAttachment);

			if (colorAttachment.sampleCount > 1) {
				Attachment resolveAttachment = colorAttachment;
				resolveAttachment.sampleCount = 1;

				attachments.emplace_back(colorAttachment);
			}
		}

		colorAttachmentCount = static_cast<uint32>(attachments.size());

		if (depthStencil) {
			// TODO
			//attachments.emplace_back(descriptor.depthStencilAttachment);

			screenProgram = createScreenProgram();
		}
	}

	void OpenGLRenderPass::setAttachmentImageView(uint32 attachmentIndex, const ref<ImageView>& imageView) {
		// TODO
	}

	GLuint OpenGLRenderPass::createScreenProgram() {
		const chain<uint8> screenVertexShaderModuleCode = Filesystem::loadByteBuffer("../ProjectA/src/Asset/Shader/Screen.vert");
		const OpenGLShaderModule screenVertexShaderModule(screenVertexShaderModuleCode, ShaderStage::VERTEX);

		const chain<uint8> screenFragmentShaderModuleCode = Filesystem::loadByteBuffer("../ProjectA/src/Asset/Shader/Screen.frag");
		const OpenGLShaderModule screenFragmentShaderModule(screenFragmentShaderModuleCode, ShaderStage::FRAGMENT);

		const GLuint screenProgram = glCreateProgram();

		glAttachShader(screenProgram, screenVertexShaderModule.object);
		glAttachShader(screenProgram, screenFragmentShaderModule.object);
		glLinkProgram(screenProgram);

		GLint linkStatus;

		glGetProgramiv(screenProgram, GL_LINK_STATUS, &linkStatus);

		if (linkStatus != GL_TRUE) {
			GLint infoLogLength;

			glGetProgramiv(screenProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

			chain<GLchar> infoLog(infoLogLength);

			glGetProgramInfoLog(screenProgram, infoLogLength, nullptr, infoLog.data());

			ENGINE_ERROR("Screen program linking failed:\n[]", infoLog.data());
			BREAKPOINT();

			return 0;
		}

		glDetachShader(screenProgram, screenVertexShaderModule.object);
		glDetachShader(screenProgram, screenFragmentShaderModule.object);

		return screenProgram;
	}
}