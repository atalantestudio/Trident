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

				// Offset from the start of the heap allocated in DescriptorSet.
				UINT viewDescriptorOffset;
				UINT samplerDescriptorOffset;
			};

		public:
			sequence<DescriptorSetBinding> bindings;
			ComPtr<ID3D12DescriptorHeap> viewDescriptorHeap = nullptr;
			ComPtr<ID3D12DescriptorHeap> samplerDescriptorHeap = nullptr;
	};
}