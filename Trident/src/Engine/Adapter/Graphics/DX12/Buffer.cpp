// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::Buffer::Buffer(const Trident::Device* device, const BufferDescriptor& descriptor) :
		#if defined(DEBUG_OBJECT_NAME)
			name(descriptor.name),
		#endif
		Trident::Buffer(device, descriptor)
	{}

	ref<Trident::Buffer> DX12::createBuffer(const Trident::Device* device, const BufferDescriptor& descriptor) {
		return allocateRef<DX12::Buffer>(device, descriptor);
	}

	void DX12::destroyBuffer(const Trident::Device* device, const Trident::Buffer* buffer) {
		const DX12::Buffer* _buffer = static_cast<const DX12::Buffer*>(buffer);

		_buffer->resource->Release();

		if (_buffer->commandSignature) {
			_buffer->commandSignature->Release();
		}
	}

	void* DX12::mapBuffer(const Trident::Device* device, Trident::Buffer* buffer, uint64 offset, uint64 size) {
		const DX12::Buffer* _buffer = static_cast<const DX12::Buffer*>(buffer);
		void* data;

		D3D12_RANGE readRange{};
		readRange.Begin = offset;
		readRange.End = offset + size;

		const HRESULT result = _buffer->resource->Map(0, &readRange, &data);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to map resource.");

		uint8* mappedRange = static_cast<uint8*>(data) + offset;

		return static_cast<void*>(mappedRange);
	}

	void DX12::unmapBuffer(const Trident::Device* device, Trident::Buffer* buffer) {
		const DX12::Buffer* _buffer = static_cast<const DX12::Buffer*>(buffer);

		_buffer->resource->Unmap(0, nullptr);
	}
}