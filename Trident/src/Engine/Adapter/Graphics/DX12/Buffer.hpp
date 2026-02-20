// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/API/Rendering/Buffer.hpp"

namespace Trident {
	class DX12::Buffer final : public Trident::Buffer {
		public:
			explicit Buffer(const Trident::Device* device, const BufferDescriptor& descriptor);

		public:
			ID3D12Resource* resource;
			ID3D12CommandSignature* commandSignature = nullptr;
			D3D12_HEAP_TYPE heapType;

			// TODO: Use.
			UINT stride;
	};

	namespace DX12 {
		ref<Trident::Buffer> createBuffer(const Trident::Device* device, const BufferDescriptor& descriptor, UINT stride);

		void destroyBuffer(const Trident::Device* device, const Trident::Buffer* buffer);

		void* mapBuffer(const Trident::Device* device, const Trident::Buffer* buffer, uint64 offset, uint64 size);

		void unmapBuffer(const Trident::Device* device, const Trident::Buffer* buffer);
	}
}