// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/DescriptorSet.hpp"
#include "Engine/API/Rendering/DescriptorType.hpp"

namespace Trident {
	class GL::DescriptorSet final : public Trident::DescriptorSet {
		public:
			struct DescriptorSetBinding {
				DescriptorType descriptorType;
				sequence<DescriptorBuffer> descriptorBuffers;
				sequence<DescriptorImage> descriptorImages;
			};

		public:
			sequence<DescriptorSetBinding> bindings;
	};
}