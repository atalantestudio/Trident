// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/DescriptorSet.hpp"
#include "Engine/API/Rendering/DescriptorType.hpp"

namespace ProjectA {
	class DX12::DescriptorSet final : public ProjectA::DescriptorSet {
		public:
			struct DescriptorSetBinding {
				DescriptorType descriptorType;
				uint32 descriptorCount;
			};

		public:
			sequence<DescriptorSetBinding> bindings;
			ComPtr<ID3D12DescriptorHeap> cbvSrvUavViewDescriptorHeap = nullptr;
			ComPtr<ID3D12DescriptorHeap> samplerDescriptorHeap = nullptr;
	};
}