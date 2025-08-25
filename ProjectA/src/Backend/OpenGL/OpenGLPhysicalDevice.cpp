#include "pch.hpp"

#include "Backend/OpenGL/OpenGLPhysicalDevice.hpp"

namespace ProjectA {
	OpenGLPhysicalDevice::OpenGLPhysicalDevice() {
		const GLubyte* vendor = glGetString(GL_VENDOR);
		const GLubyte* renderer = glGetString(GL_RENDERER);
		const GLubyte* version = glGetString(GL_VERSION);

		ENGINE_INFO("Using [] [] [].", vendor, renderer, version);

		setLimits();
	}

	void OpenGLPhysicalDevice::setLimits() {
		limits.bufferImageGranularity = 0;

		glGetFloatv(GL_SMOOTH_LINE_WIDTH_GRANULARITY, &limits.lineWidthGranularity);
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, pointer_cast<GLint*>(&limits.maxColorAttachmentCount));

		limits.maxBoundDescriptorSetCount = ~0;

		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, pointer_cast<GLint*>(&limits.maxComputeWorkgroupCount[0]));
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, pointer_cast<GLint*>(&limits.maxComputeWorkgroupCount[1]));
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, pointer_cast<GLint*>(&limits.maxComputeWorkgroupCount[2]));
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, pointer_cast<GLint*>(&limits.maxComputeWorkgroupSize[0]));
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, pointer_cast<GLint*>(&limits.maxComputeWorkgroupSize[1]));
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, pointer_cast<GLint*>(&limits.maxComputeWorkgroupSize[2]));
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
	}
}