#include "pch.hpp"

#include "Backend/OpenGL/OpenGLRenderPass.hpp"
#include "Backend/OpenGL/OpenGLShaderModule.hpp"

#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	OpenGLRenderPass::OpenGLRenderPass(const RenderPassDescriptor& descriptor) :
		loadOperation(descriptor.loadOperation),
		sampleCount(descriptor.sampleCount)
	{
		if (sampleCount > 1) {
			const chain<uint8> multisampleVertexShaderModuleCode = Filesystem::loadByteBuffer("../ProjectA/src/Asset/Shader/Screen.vert");
			const OpenGLShaderModule multisampleVertexShaderModule(multisampleVertexShaderModuleCode, ShaderStage::VERTEX);

			const chain<uint8> multisampleFragmentShaderModuleCode = Filesystem::loadByteBuffer("../ProjectA/src/Asset/Shader/Screen.frag");
			const OpenGLShaderModule multisampleFragmentShaderModule(multisampleFragmentShaderModuleCode, ShaderStage::FRAGMENT);

			multisampleProgram = glCreateProgram();

			glAttachShader(multisampleProgram, multisampleVertexShaderModule.object);
			glAttachShader(multisampleProgram, multisampleFragmentShaderModule.object);
			glLinkProgram(multisampleProgram);

			GLint linkStatus;

			glGetProgramiv(multisampleProgram, GL_LINK_STATUS, &linkStatus);

			if (linkStatus != GL_TRUE) {
				GLint infoLogLength;

				glGetProgramiv(multisampleProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

				chain<GLchar> infoLog(infoLogLength);

				glGetProgramInfoLog(multisampleProgram, infoLogLength, nullptr, infoLog.data());

				ENGINE_ERROR("Multisample program linking failed.\n[]", infoLog.data());
				BREAKPOINT();
			}

			glDetachShader(multisampleProgram, multisampleVertexShaderModule.object);
			glDetachShader(multisampleProgram, multisampleFragmentShaderModule.object);

			glCreateFramebuffers(2, &multisampleFramebuffer);
		}
	}

	OpenGLRenderPass::~OpenGLRenderPass() {
		if (multisampleFramebuffer != 0) {
			// This also deletes the intermediate multisample framebuffer.
			glDeleteFramebuffers(2, &multisampleFramebuffer);
		}
	}

	void OpenGLRenderPass::recreateMultisampleImages(uint32 extentWidth, uint32 extentHeight) {
		glBindFramebuffer(GL_FRAMEBUFFER, multisampleFramebuffer);

		ImageDescriptor multisampleImageDescriptor{};
		multisampleImageDescriptor.type = ImageType::TEXTURE_2D;
		multisampleImageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
		multisampleImageDescriptor.usage = ImageUsage::COLOR_ATTACHMENT;
		multisampleImageDescriptor.width = extentWidth;
		multisampleImageDescriptor.height = extentHeight;
		multisampleImageDescriptor.depth = 1;
		multisampleImageDescriptor.mipLevelCount = 1;
		multisampleImageDescriptor.arrayLayerCount = 1;
		multisampleImageDescriptor.sampleCount = sampleCount;

		multisampleImage = createScope<OpenGLImage>(multisampleImageDescriptor);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampleImage->object, 0);

		DEBUG_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glBindFramebuffer(GL_FRAMEBUFFER, multisampleIntermediateFramebuffer);

		ImageDescriptor screenImageDescriptor{};
		screenImageDescriptor.type = ImageType::TEXTURE_2D;
		screenImageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
		screenImageDescriptor.usage = ImageUsage::COLOR_ATTACHMENT;
		screenImageDescriptor.width = extentWidth;
		screenImageDescriptor.height = extentHeight;
		screenImageDescriptor.depth = 1;
		screenImageDescriptor.mipLevelCount = 1;
		screenImageDescriptor.arrayLayerCount = 1;
		screenImageDescriptor.sampleCount = 1;

		screenImage = createScope<OpenGLImage>(screenImageDescriptor);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenImage->object, 0);

		DEBUG_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}