#include "pch.hpp"

#include "Backend/OpenGL/OpenGLSampler.hpp"

#include "Backend/OpenGL/SamplerFilter.hpp"

namespace ProjectA {
	OpenGLSampler::OpenGLSampler(const SamplerDescriptor& descriptor) {
		PROFILE_SCOPE(FUNC_SIG);

		glCreateSamplers(1, &object);

		glSamplerParameteri(object, GL_TEXTURE_MAG_FILTER, getGlFilter(descriptor.magFilter));
		glSamplerParameteri(object, GL_TEXTURE_MIN_FILTER, getGlFilter(descriptor.minFilter));
	}

	OpenGLSampler::~OpenGLSampler() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteSamplers(1, &object);
	}
}