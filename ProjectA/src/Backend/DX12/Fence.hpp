// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Backend/DX12/Device.hpp"

#include "Engine/API/Rendering/Fence.hpp"

namespace ProjectA {
	class DX12::Fence : public ProjectA::Fence {
		public:
			explicit Fence(const DX12::Device& device);

		private:
			ComPtr<ID3D12Fence> fence;
	};
}