// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/API/Rendering/Buffer.hpp"

namespace ProjectA {
	class DX12::Buffer final : public ProjectA::Buffer {
		public:
			explicit Buffer(const BufferDescriptor& descriptor, const DX12::Device& device);

			void unmap() const final override;

			void* getMappedRange(uint64 offset, uint64 size) const final override;

		public:
			ComPtr<ID3D12Resource> resource;
	};
}