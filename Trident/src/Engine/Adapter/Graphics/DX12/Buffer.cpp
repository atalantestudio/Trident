// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::BaseBuffer::BaseBuffer(const Trident::Device* device, const BufferDescriptor& descriptor) :
		#if defined(DEBUG_OBJECT_NAME)
			name(descriptor.name),
		#endif
		Trident::BaseBuffer(device, descriptor)
	{
		// TODO(graphics/dx12/min-buffer-offset-alignment): Move to the engine side.
		if (descriptor.usage & BufferUsage::UNIFORM) {
			// NOTE: Constant buffer views need to be aligned to 256.
			size = (descriptor.size + D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);
		}
	}

	ref<Trident::BaseBuffer> DX12::createBuffer(const Trident::Device* device, const BufferDescriptor& descriptor) {
		return createRef<DX12::BaseBuffer>(device, descriptor);
	}

	void DX12::destroyBuffer(const Trident::Device* device, const Trident::BaseBuffer* buffer) {
		const DX12::BaseBuffer* _buffer = static_cast<const DX12::BaseBuffer*>(buffer);

		_buffer->resource->Release();

		if (_buffer->commandSignature) {
			_buffer->commandSignature->Release();
		}
	}

	void* DX12::mapBuffer(const Trident::Device* device, const Trident::BaseBuffer* buffer, uint64 offset, uint64 size) {
		const DX12::BaseBuffer* _buffer = static_cast<const DX12::BaseBuffer*>(buffer);
		void* data;

		// TODO(graphics/dx12/buffer/map): Use offset and size.
		const HRESULT result = _buffer->resource->Map(0, nullptr, &data);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to map resource.");

		// TODO(graphics/dx12/buffer/map)
		// "When ppData is not NULL, the pointer returned is never offset by any values in pReadRange."
		// Manually offset the data?
		// See https://learn.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12resource-map.
		return data;
	}

	void DX12::unmapBuffer(const Trident::Device* device, const Trident::BaseBuffer* buffer) {
		const DX12::BaseBuffer* _buffer = static_cast<const DX12::BaseBuffer*>(buffer);

		_buffer->resource->Unmap(0, nullptr);
	}
}