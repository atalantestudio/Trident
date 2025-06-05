#include "pch.hpp"

#include "Backend/OpenGL/OpenGLRenderPass.hpp"
#include "Backend/OpenGL/OpenGLShaderModule.hpp"

#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	OpenGLRenderPass::OpenGLRenderPass(const RenderPassDescriptor& descriptor) :
		RenderPass(descriptor)
	{
		PROFILE_SCOPE(FUNC_SIG);

		GLint maxColorAttachments;

		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);

		DEBUG_ASSERT(colorAttachmentCount < static_cast<uint32>(maxColorAttachments));
	}

	ref<Image> OpenGLRenderPass::createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 colorAttachmentIndex) const {
		DEBUG_ASSERT(colorAttachmentIndex < colorAttachmentCount);
		DEBUG_ASSERT((resolveAttachmentCount == 0 && colorAttachmentIndex != presentAttachmentIndex) || resolveAttachmentCount > 0);

		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		const ColorAttachment& colorAttachment = colorAttachments[colorAttachmentIndex];

		ImageDescriptor colorImageDescriptor{};
		colorImageDescriptor.type = descriptor.type;
		colorImageDescriptor.format = colorAttachment.format;
		colorImageDescriptor.usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::TRANSIENT_ATTACHMENT;
		colorImageDescriptor.width = descriptor.width;
		colorImageDescriptor.height = descriptor.height;
		colorImageDescriptor.depth = descriptor.depth;
		colorImageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		colorImageDescriptor.arrayLayerCount = descriptor.arrayLayerCount;
		colorImageDescriptor.sampleCount = colorAttachment.sampleCount;

		return createRef<OpenGLImage>(colorImageDescriptor);
	}

	ref<Image> OpenGLRenderPass::createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor) const {
		DEBUG_ASSERT(depthStencil);

		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		ImageDescriptor depthStencilImageDescriptor{};
		depthStencilImageDescriptor.type = descriptor.type;
		depthStencilImageDescriptor.format = depthStencilAttachment.format;
		depthStencilImageDescriptor.usage = ImageUsage::DEPTH_STENCIL_ATTACHMENT;
		depthStencilImageDescriptor.width = descriptor.width;
		depthStencilImageDescriptor.height = descriptor.height;
		depthStencilImageDescriptor.depth = descriptor.depth;
		depthStencilImageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		depthStencilImageDescriptor.arrayLayerCount = descriptor.arrayLayerCount;
		depthStencilImageDescriptor.sampleCount = depthStencilAttachment.sampleCount;

		return createRef<OpenGLImage>(depthStencilImageDescriptor);
	}

	GLuint OpenGLRenderPass::createScreenProgram() {
		const std::vector<uint8> screenVertexShaderModuleCode = Filesystem::loadByteBuffer("../ProjectA/src/Asset/Shader/Screen.vert");
		const OpenGLShaderModule screenVertexShaderModule(screenVertexShaderModuleCode, ShaderStage::VERTEX);

		const std::vector<uint8> screenFragmentShaderModuleCode = Filesystem::loadByteBuffer("../ProjectA/src/Asset/Shader/Screen.frag");
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

			std::vector<GLchar> infoLog(infoLogLength);

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