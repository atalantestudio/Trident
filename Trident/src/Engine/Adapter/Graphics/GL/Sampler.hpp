// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Sampler.hpp"

namespace Trident {
	class GL::Sampler final : public Trident::Sampler {
		public:
			explicit Sampler(const SamplerDescriptor& descriptor);

			~Sampler() final override;

		public:
			GLuint sampler;
	};
}