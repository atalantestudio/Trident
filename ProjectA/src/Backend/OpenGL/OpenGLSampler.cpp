#include "pch.hpp"

#include "Backend/OpenGL/OpenGLSampler.hpp"

#include "Backend/OpenGL/SamplerFilter.hpp"

namespace ProjectA {
	OpenGLSampler::OpenGLSampler(const SamplerDescriptor& descriptor) {
		PROFILE_SCOPE(FUNC_SIG);

		glCreateSamplers(1, &object);

		glSamplerParameteri(object, GL_TEXTURE_MAG_FILTER, getGlFilter(descriptor.magFilter));
		glSamplerParameteri(object, GL_TEXTURE_MIN_FILTER, getGlFilter(descriptor.minFilter));
		glSamplerParameteri(object, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(object, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(object, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	OpenGLSampler::~OpenGLSampler() {
		PROFILE_SCOPE(FUNC_SIG);

		glDeleteSamplers(1, &object);
	}
}