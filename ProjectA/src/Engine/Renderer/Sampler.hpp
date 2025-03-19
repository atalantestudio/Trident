#pragma once

#include "Engine/Renderer/SamplerFilter.hpp"

namespace ProjectA {
	struct SamplerDescriptor {
		SamplerFilter magFilter;
		SamplerFilter minFilter;
	};

	class Sampler {
		public:
			virtual ~Sampler() = default;
	};
}