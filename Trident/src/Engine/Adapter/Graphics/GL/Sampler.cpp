// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Sampler.hpp"
#include "Engine/Adapter/Graphics/GL/SamplerFilter.hpp"

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

		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			glSamplerParameteri(sampler->sampler, GL_TEXTURE_MAG_FILTER, getFilter(descriptor.magFilter));
			glSamplerParameteri(sampler->sampler, GL_TEXTURE_MIN_FILTER, getFilter(descriptor.minFilter));
			glSamplerParameteri(sampler->sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glSamplerParameteri(sampler->sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glSamplerParameteri(sampler->sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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