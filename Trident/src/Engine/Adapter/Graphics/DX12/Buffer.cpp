// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/HeapType.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceState.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::Buffer::Buffer(const BufferDescriptor& descriptor, const DX12::Device& device) :
		Trident::Buffer(descriptor)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		if (descriptor.usage & BufferUsage::UNIFORM) {
			// NOTE: Constant buffer views need to be aligned to 256.
			size = (descriptor.size + D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);
		}

		HRESULT result;

		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = getHeapType(descriptor.memoryProperties);
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDescriptor{};
		resourceDescriptor.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDescriptor.Alignment = 0;
		resourceDescriptor.Width = size;
		resourceDescriptor.Height = 1;
		resourceDescriptor.DepthOrArraySize = 1;
		resourceDescriptor.MipLevels = 1;
		resourceDescriptor.Format = DXGI_FORMAT_UNKNOWN;
		resourceDescriptor.SampleDesc.Count = 1;
		resourceDescriptor.SampleDesc.Quality = 0;
		resourceDescriptor.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDescriptor.Flags = D3D12_RESOURCE_FLAG_NONE;

		if (descriptor.usage & BufferUsage::READ_WRITE_STORAGE) {
			resourceDescriptor.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		const D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;

		D3D12_RESOURCE_STATES initialResourceStates = getBufferResourceState(descriptor.usage);

		if (heapProperties.Type == D3D12_HEAP_TYPE_READBACK) {
			initialResourceStates = D3D12_RESOURCE_STATE_COPY_DEST;
		}

		result = device.device->CreateCommittedResource(&heapProperties, heapFlags, &resourceDescriptor, initialResourceStates, nullptr, IID_PPV_ARGS(&resource));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create committed buffer resource.");

		heapType = heapProperties.Type;

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				const LPWSTR name = ConvertUTF8ToUTF16(&descriptor.name[0]);

				result = resource->SetName(name);

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set resource name.");

				free(name);
			}
		#endif
	}

	DX12::Buffer::~Buffer() {
		resource->Release();

		if (commandSignature) {
			commandSignature->Release();
		}
	}

	void DX12::Buffer::unmap() const {
		resource->Unmap(0, nullptr);
	}

	void* DX12::Buffer::getMappedRange(uint64 offset, uint64 size) const {
		HRESULT result;
		void* mappedRange;

		result = resource->Map(0, nullptr, &mappedRange);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to map resource.");

		return mappedRange;
	}
}