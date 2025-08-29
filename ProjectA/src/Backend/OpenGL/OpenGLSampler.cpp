#include "pch.hpp"

#include "Backend/OpenGL/OpenGLSampler.hpp"

#include "Backend/OpenGL/SamplerFilter.hpp"

namespace ProjectA {
	OpenGLSampler::OpenGLSampler(const SamplerDescriptor& descriptor) :
		Sampler(descriptor)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			glCreateSamplers(1, &object);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_3
			glGenSamplers(1, &object);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			glSamplerParameteri(object, GL_TEXTURE_MAG_FILTER, getGlFilter(descriptor.magFilter));
			glSamplerParameteri(object, GL_TEXTURE_MIN_FILTER, getGlFilter(descriptor.minFilter));
			glSamplerParameteri(object, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glSamplerParameteri(object, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glSamplerParameteri(object, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}

	OpenGLSampler::~OpenGLSampler() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_3
			glDeleteSamplers(1, &object);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}
}