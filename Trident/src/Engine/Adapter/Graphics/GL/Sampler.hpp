// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Sampler.hpp"

namespace Trident {
	class GL::Sampler final : public Trident::Sampler {
		public:
			explicit Sampler(const Trident::Device* device);

		public:
			GLuint sampler;
	};

	namespace GL {
		ref<Trident::Sampler> createSampler(const Trident::Device* device, const SamplerDescriptor& descriptor);

		void destroySampler(const Trident::Device* device, const Trident::Sampler* sampler);
	}
}