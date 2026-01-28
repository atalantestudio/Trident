// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/DescriptorSet.hpp"
#include "Engine/API/Rendering/DescriptorType.hpp"

namespace Trident {
	class DX12::DescriptorSet final : public Trident::DescriptorSet {
		public:
			struct DescriptorSetBinding {
				DescriptorType descriptorType;
				UINT descriptorCount;
				D3D12_CPU_DESCRIPTOR_HANDLE viewDescriptorHandleOffset;
			};

		public:
			sequence<DescriptorSetBinding> bindings;
			UINT64 viewDescriptorHandleOffset;
	};
}