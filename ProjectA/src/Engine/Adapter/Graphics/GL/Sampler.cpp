// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Sampler.hpp"
#include "Engine/Adapter/Graphics/GL/SamplerFilter.hpp"

namespace ProjectA {
	GL::Sampler::Sampler(const SamplerDescriptor& descriptor) :
		ProjectA::Sampler(descriptor)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			glCreateSamplers(1, &sampler);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_3
			glGenSamplers(1, &sampler);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, getFilter(descriptor.magFilter));
			glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, getFilter(descriptor.minFilter));
			glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}

	GL::Sampler::~Sampler() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_3
			glDeleteSamplers(1, &sampler);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}
}