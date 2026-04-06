// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Graphics/Sampler.hpp"

namespace Trident {
	class VK::Sampler final : public Trident::Sampler {
		public:
			explicit Sampler(const Trident::Device* device);

		public:
			VkSampler sampler;
	};

	namespace VK {
		ref<Trident::Sampler> createSampler(const Trident::Device* device, const SamplerDescriptor& descriptor);

		void destroySampler(const Trident::Device* device, const Trident::Sampler* sampler);
	}
}