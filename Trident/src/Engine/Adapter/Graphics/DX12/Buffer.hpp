// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Buffer.hpp"

namespace Trident {
	class DX12::BaseBuffer final : public Trident::BaseBuffer {
		public:
			explicit BaseBuffer(const Trident::Device* device, const BufferDescriptor& descriptor);

		public:
			ID3D12Resource* resource;
			ID3D12CommandSignature* commandSignature = nullptr;

			#if defined(DEBUG_OBJECT_NAME)
				sequence<char8> name;
			#endif
	};

	namespace DX12 {
		ref<Trident::BaseBuffer> createBuffer(const Trident::Device* device, const BufferDescriptor& descriptor);

		void destroyBuffer(const Trident::Device* device, const Trident::BaseBuffer* buffer);

		void* mapBuffer(const Trident::Device* device, Trident::BaseBuffer* buffer, uint64 offset, uint64 size);

		void unmapBuffer(const Trident::Device* device, Trident::BaseBuffer* buffer);
	}
}