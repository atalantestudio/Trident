#include "pch.hpp"

#include "Backend/OpenGL/OpenGLSampler.hpp"

namespace ProjectA {
	OpenGLSampler::OpenGLSampler(const SamplerDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[OpenGLSampler]");

		glCreateSamplers(1, &object);
	}

	OpenGLSampler::~OpenGLSampler() {
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[~OpenGLSampler]");

		glDeleteSamplers(1, &object);
	}
}