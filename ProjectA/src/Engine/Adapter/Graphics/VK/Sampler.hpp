// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Rendering/Sampler.hpp"

namespace ProjectA {
	class VK::Sampler final : public ProjectA::Sampler {
		public:
			explicit Sampler(const SamplerDescriptor& descriptor, const VK::Device& device);

			~Sampler() final override;

		public:
			VkSampler sampler;

		private:
			const VK::Device& device;
	};
}