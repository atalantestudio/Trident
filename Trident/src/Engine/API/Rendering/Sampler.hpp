// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Rendering/Descriptor/SamplerDescriptor.hpp"

namespace Trident {
	class Sampler {
		public:
			~Sampler();

		protected:
			explicit Sampler(const Device* device);

		// TODO: Don't expose to the client side.
		public:
			const Device* device;
	};
}