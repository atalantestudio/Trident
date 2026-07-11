// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Filter.hpp"
#include "Engine/Adapter/Graphics/GL/Sampler.hpp"

namespace Trident {
	GL::Sampler::Sampler(const Trident::Device* device) :
		Trident::Sampler(device)
	{}

	ref<Trident::Sampler> GL::createSampler(const Trident::Device* device, const SamplerDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::Sampler> sampler = createRef<GL::Sampler>(device);

		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			glCreateSamplers(1, &sampler->sampler);
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_3
			glGenSamplers(1, &sampler->sampler);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		#if OPENGL_VERSION >= OPENGL_VERSION_3_3
			static constexpr GLfloat BORDER_COLOR[] = {0.0f, 0.0f, 0.0f, 1.0f};

			glSamplerParameteri(sampler->sampler, GL_TEXTURE_MAG_FILTER, getFilter(descriptor.magFilter));
			glSamplerParameteri(sampler->sampler, GL_TEXTURE_MIN_FILTER, getFilter(descriptor.minFilter));
			glSamplerParameteri(sampler->sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glSamplerParameteri(sampler->sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glSamplerParameteri(sampler->sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glSamplerParameterf(sampler->sampler, GL_TEXTURE_MIN_LOD, 0.0f);
			glSamplerParameterf(sampler->sampler, GL_TEXTURE_MAX_LOD, 1000.0f);
			glSamplerParameterfv(sampler->sampler, GL_TEXTURE_BORDER_COLOR, BORDER_COLOR);
			glSamplerParameteri(sampler->sampler, GL_TEXTURE_COMPARE_MODE, GL_NONE);
			glSamplerParameteri(sampler->sampler, GL_TEXTURE_COMPARE_FUNC, GL_NEVER);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif

		return sampler;
	}

	void GL::destroySampler(const Trident::Device* device, const Trident::Sampler* sampler) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::Sampler* _sampler = static_cast<const GL::Sampler*>(sampler);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_3
			glDeleteSamplers(1, &_sampler->sampler);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}
}