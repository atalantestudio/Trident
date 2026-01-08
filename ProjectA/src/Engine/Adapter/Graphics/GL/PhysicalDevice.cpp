#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/PhysicalDevice.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	OpenGLPhysicalDevice::OpenGLPhysicalDevice() {
		#ifdef BUILD_MODE_DEBUG
			const GLchar* vendor = pointer_cast<const GLchar*>(glGetString(GL_VENDOR));
			const GLchar* renderer = pointer_cast<const GLchar*>(glGetString(GL_RENDERER));
			const GLchar* version = pointer_cast<const GLchar*>(glGetString(GL_VERSION));

			ENGINE_INFO("Using [] [] [].", vendor, renderer, version);
		#endif

		setLimits();
	}

	void OpenGLPhysicalDevice::setLimits() {
		limits.bufferImageGranularity = 0;

		glGetFloatv(GL_SMOOTH_LINE_WIDTH_GRANULARITY, &limits.lineWidthGranularity);
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, pointer_cast<GLint*>(&limits.maxColorAttachmentCount));

		limits.maxBoundDescriptorSetCount = ~0;

		#if OPENGL_VERSION >= OPENGL_VERSION_3_0
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, pointer_cast<GLint*>(&limits.maxComputeWorkgroupCount[0]));
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, pointer_cast<GLint*>(&limits.maxComputeWorkgroupCount[1]));
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, pointer_cast<GLint*>(&limits.maxComputeWorkgroupCount[2]));
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, pointer_cast<GLint*>(&limits.maxComputeWorkgroupSize[0]));
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, pointer_cast<GLint*>(&limits.maxComputeWorkgroupSize[1]));
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, pointer_cast<GLint*>(&limits.maxComputeWorkgroupSize[2]));
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, pointer_cast<GLint*>(&limits.maxFramebufferSize[0]));
		glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, pointer_cast<GLint*>(&limits.maxFramebufferSize[1]));
		glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, pointer_cast<GLint*>(&limits.maxFramebufferLayerCount));
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, pointer_cast<GLint*>(&limits.maxImageDimension1D));
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, pointer_cast<GLint*>(&limits.maxImageDimension2D));
		glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, pointer_cast<GLint*>(&limits.maxImageDimension3D));
		glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, pointer_cast<GLint*>(&limits.maxImageLayerCount));

		limits.maxMemoryAllocationCount = ~0;

		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, pointer_cast<GLint*>(&limits.maxVertexInputAttributeCount));
		glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, pointer_cast<GLint*>(&limits.maxVertexInputAttributeOffset));
		glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, pointer_cast<GLint*>(&limits.maxVertexInputBindingCount));
		glGetIntegerv(GL_MAX_VERTEX_ATTRIB_STRIDE, pointer_cast<GLint*>(&limits.maxVertexInputBindingStride));
		glGetFloatv(GL_SMOOTH_POINT_SIZE_GRANULARITY, &limits.pointSizeGranularity);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_6
			GLint shaderBinaryFormatCount;

			glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &shaderBinaryFormatCount);

			sequence<GLint> shaderBinaryFormats(shaderBinaryFormatCount);

			glGetIntegerv(GL_SHADER_BINARY_FORMATS, &shaderBinaryFormats[0]);

			uint32 spirvShaderBinaryFormatIndex = 0;

			while (spirvShaderBinaryFormatIndex < shaderBinaryFormats.count()) {
				if (shaderBinaryFormats[spirvShaderBinaryFormatIndex] == GL_SHADER_BINARY_FORMAT_SPIR_V) {
					break;
				}

				spirvShaderBinaryFormatIndex += 1;
			}

			ADAPTER_ASSERT(spirvShaderBinaryFormatIndex < shaderBinaryFormats.count(), "GL_SHADER_BINARY_FORMAT_SPIR_V is not supported.");
		#endif
	}
}