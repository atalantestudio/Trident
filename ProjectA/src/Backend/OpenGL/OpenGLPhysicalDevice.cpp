#include "pch.hpp"

#include <glad/glad.h>

#include "Backend/OpenGL/OpenGLPhysicalDevice.hpp"

namespace ProjectA {
	OpenGLPhysicalDevice::OpenGLPhysicalDevice() {
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, static_cast<GLint*>(static_cast<void*>(&limits.maxComputeWorkgroupCount[0])));
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, static_cast<GLint*>(static_cast<void*>(&limits.maxComputeWorkgroupCount[1])));
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, static_cast<GLint*>(static_cast<void*>(&limits.maxComputeWorkgroupCount[2])));
	}

	const PhysicalDeviceLimits& OpenGLPhysicalDevice::getLimits() const {
		return limits;
	}
}