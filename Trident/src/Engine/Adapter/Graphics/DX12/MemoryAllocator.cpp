// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/Adapter/Graphics/DX12/MemoryAllocator.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceDimension.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceFlags.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::MemoryAllocator::MemoryAllocator(const DX12::Device* device) :
		Trident::MemoryAllocator(device)
	{
		viewDescriptorHandleIncrementSize = device->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		samplerDescriptorHandleIncrementSize = device->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		renderTargetViewDescriptorHandleIncrementSize = device->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		depthStencilViewDescriptorHandleIncrementSize = device->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}

	void DX12::MemoryAllocator::destroyDescriptorHeaps() {
		INT descriptorHeapAllocationIndex = static_cast<INT>(descriptorHeapAllocations.size()) - 1;

		while (descriptorHeapAllocationIndex >= 0) {
			descriptorHeapAllocations[descriptorHeapAllocationIndex].descriptorHeap->Release();

			descriptorHeapAllocationIndex -= 1;
		}
	}

	DX12::DescriptorHeapAllocation& DX12::MemoryAllocator::getCurrentViewDescriptorHeapAllocation() {
		if (currentViewDescriptorHeapAllocationIndex >= descriptorHeapAllocations.size()) {
			createViewDescriptorHeapAllocation();
		}

		const DX12::DescriptorHeapAllocation& descriptorHeapAllocation = descriptorHeapAllocations[currentViewDescriptorHeapAllocationIndex];

		if (descriptorHeapAllocation.descriptorOffset == descriptorHeapAllocation.descriptorCount) {
			createViewDescriptorHeapAllocation();
		}

		return descriptorHeapAllocations[currentViewDescriptorHeapAllocationIndex];
	}

	DX12::DescriptorHeapAllocation& DX12::MemoryAllocator::getCurrentSamplerDescriptorHeapAllocation() {
		if (currentSamplerDescriptorHeapAllocationIndex >= descriptorHeapAllocations.size()) {
			createSamplerDescriptorHeapAllocation();
		}

		const DX12::DescriptorHeapAllocation& descriptorHeapAllocation = descriptorHeapAllocations[currentSamplerDescriptorHeapAllocationIndex];

		if (descriptorHeapAllocation.descriptorOffset == descriptorHeapAllocation.descriptorCount) {
			createSamplerDescriptorHeapAllocation();
		}

		return descriptorHeapAllocations[currentSamplerDescriptorHeapAllocationIndex];
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DX12::MemoryAllocator::allocateViewDescriptors(UINT descriptorCount) {
		DX12::DescriptorHeapAllocation& descriptorHeapAllocation = getCurrentViewDescriptorHeapAllocation();

		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = descriptorHeapAllocation.descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		descriptorHandle.ptr += descriptorHeapAllocation.descriptorOffset * viewDescriptorHandleIncrementSize;

		ADAPTER_ASSERT(descriptorHeapAllocation.descriptorOffset + descriptorCount < descriptorHeapAllocation.descriptorCount, "Not enough space to reserve for [] view descriptor(s).", descriptorCount);

		descriptorHeapAllocation.descriptorOffset += descriptorCount;

		return descriptorHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DX12::MemoryAllocator::allocateSamplerDescriptors(UINT descriptorCount) {
		DX12::DescriptorHeapAllocation& descriptorHeapAllocation = getCurrentSamplerDescriptorHeapAllocation();

		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = descriptorHeapAllocation.descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		descriptorHandle.ptr += descriptorHeapAllocation.descriptorOffset * samplerDescriptorHandleIncrementSize;

		ADAPTER_ASSERT(descriptorHeapAllocation.descriptorOffset + descriptorCount < descriptorHeapAllocation.descriptorCount, "Not enough space to reserve for [] sampler descriptor(s).", descriptorCount);

		descriptorHeapAllocation.descriptorOffset += descriptorCount;

		return descriptorHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DX12::MemoryAllocator::allocateRenderTargetView(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* descriptor) {
		const DX12::Device* device = static_cast<const DX12::Device*>(this->device);

		DX12::DescriptorHeapAllocation& descriptorHeapAllocation = getCurrentRenderTargetViewDescriptorHeapAllocation();

		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = descriptorHeapAllocation.descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		descriptorHandle.ptr += descriptorHeapAllocation.descriptorOffset * renderTargetViewDescriptorHandleIncrementSize;

		device->device->CreateRenderTargetView(resource, descriptor, descriptorHandle);

		descriptorHeapAllocation.descriptorOffset += 1;

		return descriptorHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DX12::MemoryAllocator::allocateDepthStencilView(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* descriptor) {
		const DX12::Device* device = static_cast<const DX12::Device*>(this->device);

		DX12::DescriptorHeapAllocation& descriptorHeapAllocation = getCurrentDepthStencilViewDescriptorHeapAllocation();

		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = descriptorHeapAllocation.descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		descriptorHandle.ptr += descriptorHeapAllocation.descriptorOffset * depthStencilViewDescriptorHandleIncrementSize;

		device->device->CreateDepthStencilView(resource, descriptor, descriptorHandle);

		descriptorHeapAllocation.descriptorOffset += 1;

		return descriptorHandle;
	}

	void DX12::MemoryAllocator::writeImage(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, const DescriptorImage& descriptorImage) const {
		const DX12::Device* device = static_cast<const DX12::Device*>(this->device);
		const DX12::Image* image = static_cast<const DX12::Image*>(descriptorImage.imageView->getImage());

		ADAPTER_ASSERT(image->getType() == ImageType::TEXTURE_2D, "Images other than TEXTURE_2D are not supported.");

		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor{};

		if (isDepthFormat(image->getFormat())) {
			if (image->getFormat() == ImageFormat::D32_SFLOAT) {
				shaderResourceViewDescriptor.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else {
				ENGINE_ERROR_UNSUPPORTED();
			}
		}
		else {
			shaderResourceViewDescriptor.Format = getImageFormat(image->getFormat());
		}

		if (image->getLayerCount() > 1) {
			shaderResourceViewDescriptor.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			shaderResourceViewDescriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			shaderResourceViewDescriptor.Texture2DArray.MostDetailedMip = descriptorImage.imageView->getMipLevelOffset();
			shaderResourceViewDescriptor.Texture2DArray.MipLevels = descriptorImage.imageView->getMipLevelCount();
			shaderResourceViewDescriptor.Texture2DArray.FirstArraySlice = descriptorImage.imageView->getLayerOffset();
			shaderResourceViewDescriptor.Texture2DArray.ArraySize = descriptorImage.imageView->getLayerCount();
			shaderResourceViewDescriptor.Texture2DArray.PlaneSlice = 0;
			shaderResourceViewDescriptor.Texture2DArray.ResourceMinLODClamp = 0.0f;
		}
		else {
			shaderResourceViewDescriptor.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDescriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			shaderResourceViewDescriptor.Texture2D.MostDetailedMip = descriptorImage.imageView->getMipLevelOffset();
			shaderResourceViewDescriptor.Texture2D.MipLevels = descriptorImage.imageView->getMipLevelCount();
			shaderResourceViewDescriptor.Texture2D.PlaneSlice = 0;
			shaderResourceViewDescriptor.Texture2D.ResourceMinLODClamp = 0.0f;
		}

		device->device->CreateShaderResourceView(image->resource, &shaderResourceViewDescriptor, descriptorHandle);
	}

	void DX12::MemoryAllocator::writeReadWriteStorageImage(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, const DescriptorImage& descriptorImage) const {
		const DX12::Device* device = static_cast<const DX12::Device*>(this->device);
		const DX12::Image* image = static_cast<const DX12::Image*>(descriptorImage.imageView->getImage());

		ADAPTER_ASSERT(image->getType() == ImageType::TEXTURE_2D, "Images other than TEXTURE_2D are not supported.");

		D3D12_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDescriptor{};
		unorderedAccessViewDescriptor.Format = getImageFormat(image->getFormat());

		if (image->getLayerCount() > 1) {
			unorderedAccessViewDescriptor.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			unorderedAccessViewDescriptor.Texture2DArray.MipSlice = descriptorImage.imageView->getMipLevelOffset();
			unorderedAccessViewDescriptor.Texture2DArray.FirstArraySlice = descriptorImage.imageView->getLayerOffset();
			unorderedAccessViewDescriptor.Texture2DArray.ArraySize = descriptorImage.imageView->getLayerCount();
			unorderedAccessViewDescriptor.Texture2DArray.PlaneSlice = 0;
		}
		else {
			unorderedAccessViewDescriptor.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			unorderedAccessViewDescriptor.Texture2D.MipSlice = descriptorImage.imageView->getMipLevelOffset();
			unorderedAccessViewDescriptor.Texture2D.PlaneSlice = 0;
		}

		device->device->CreateUnorderedAccessView(image->resource, nullptr, &unorderedAccessViewDescriptor, descriptorHandle);
	}

	void DX12::MemoryAllocator::writeUniformBuffer(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, const DescriptorBuffer& descriptorBuffer) const {
		const DX12::Device* device = static_cast<const DX12::Device*>(this->device);
		const ref<DX12::BaseBuffer> buffer = std::static_pointer_cast<DX12::BaseBuffer>(descriptorBuffer.buffer);

		D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDescriptor{};
		constantBufferViewDescriptor.BufferLocation = buffer->resource->GetGPUVirtualAddress() + descriptorBuffer.offset;
		constantBufferViewDescriptor.SizeInBytes = static_cast<UINT>(descriptorBuffer.size);

		device->device->CreateConstantBufferView(&constantBufferViewDescriptor, descriptorHandle);
	}

	void DX12::MemoryAllocator::writeReadOnlyStorageBuffer(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, const DescriptorBuffer& descriptorBuffer) const {
		const DX12::Device* device = static_cast<const DX12::Device*>(this->device);
		const ref<DX12::BaseBuffer> buffer = std::static_pointer_cast<DX12::BaseBuffer>(descriptorBuffer.buffer);

		// TODO(graphics/buffer/stride): Use the size of the template parameter in BufferView.
		ENGINE_ERROR_UNIMPLEMENTED();

		constexpr UINT stride = 1;

		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor{};
		shaderResourceViewDescriptor.Format = DXGI_FORMAT_UNKNOWN;
		shaderResourceViewDescriptor.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		shaderResourceViewDescriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		shaderResourceViewDescriptor.Buffer.FirstElement = descriptorBuffer.offset / stride;
		shaderResourceViewDescriptor.Buffer.NumElements = static_cast<UINT>(descriptorBuffer.size) / stride;
		shaderResourceViewDescriptor.Buffer.StructureByteStride = stride;
		shaderResourceViewDescriptor.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		device->device->CreateShaderResourceView(buffer->resource, &shaderResourceViewDescriptor, descriptorHandle);
	}

	void DX12::MemoryAllocator::writeReadWriteStorageBuffer(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, const DescriptorBuffer& descriptorBuffer) const {
		const DX12::Device* device = static_cast<const DX12::Device*>(this->device);
		const ref<DX12::BaseBuffer> buffer = std::static_pointer_cast<DX12::BaseBuffer>(descriptorBuffer.buffer);

		// TODO(graphics/buffer/stride): Use the size of the template parameter in BufferView instead.
		ENGINE_ERROR_UNIMPLEMENTED();

		constexpr UINT stride = 1;

		D3D12_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDescriptor{};
		unorderedAccessViewDescriptor.Format = DXGI_FORMAT_UNKNOWN;
		unorderedAccessViewDescriptor.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		unorderedAccessViewDescriptor.Buffer.FirstElement = descriptorBuffer.offset / stride;
		unorderedAccessViewDescriptor.Buffer.NumElements = static_cast<UINT>(descriptorBuffer.size) / stride;
		unorderedAccessViewDescriptor.Buffer.StructureByteStride = stride;
		unorderedAccessViewDescriptor.Buffer.CounterOffsetInBytes = 0;
		unorderedAccessViewDescriptor.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		device->device->CreateUnorderedAccessView(buffer->resource, nullptr, &unorderedAccessViewDescriptor, descriptorHandle);
	}

	void DX12::MemoryAllocator::writeInputAttachment(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, const DescriptorImage& descriptorImage) const {
		const DX12::Device* device = static_cast<const DX12::Device*>(this->device);
		const DX12::Image* image = static_cast<const DX12::Image*>(descriptorImage.imageView->getImage());

		ADAPTER_ASSERT(image->getType() == ImageType::TEXTURE_2D, "Images other than TEXTURE_2D are not supported.");

		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor{};
		shaderResourceViewDescriptor.Format = getImageFormat(image->getFormat());

		if (isDepthFormat(image->getFormat())) {
			if (shaderResourceViewDescriptor.Format == DXGI_FORMAT_D32_FLOAT) {
				// NOTE: Depth formats are not supported when creating a shader resource view. Use a color format of the same size instead.
				shaderResourceViewDescriptor.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else {
				ENGINE_ERROR_UNIMPLEMENTED();
			}
		}

		if (image->getLayerCount() > 1) {
			shaderResourceViewDescriptor.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			shaderResourceViewDescriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			shaderResourceViewDescriptor.Texture2DArray.MostDetailedMip = descriptorImage.imageView->getMipLevelOffset();
			shaderResourceViewDescriptor.Texture2DArray.MipLevels = descriptorImage.imageView->getMipLevelCount();
			shaderResourceViewDescriptor.Texture2DArray.FirstArraySlice = descriptorImage.imageView->getLayerOffset();
			shaderResourceViewDescriptor.Texture2DArray.ArraySize = descriptorImage.imageView->getLayerCount();
			shaderResourceViewDescriptor.Texture2DArray.PlaneSlice = 0;
			shaderResourceViewDescriptor.Texture2DArray.ResourceMinLODClamp = 0.0f;
		}
		else {
			shaderResourceViewDescriptor.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDescriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			shaderResourceViewDescriptor.Texture2D.MostDetailedMip = descriptorImage.imageView->getMipLevelOffset();
			shaderResourceViewDescriptor.Texture2D.MipLevels = descriptorImage.imageView->getMipLevelCount();
			shaderResourceViewDescriptor.Texture2D.PlaneSlice = 0;
			shaderResourceViewDescriptor.Texture2D.ResourceMinLODClamp = 0.0f;
		}

		device->device->CreateShaderResourceView(image->resource, &shaderResourceViewDescriptor, descriptorHandle);
	}

	void DX12::MemoryAllocator::writeSampler(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, const D3D12_SAMPLER_DESC* descriptor) const {
		const DX12::Device* device = static_cast<const DX12::Device*>(this->device);

		device->device->CreateSampler(descriptor, descriptorHandle);
	}

	void DX12::MemoryAllocator::createViewDescriptorHeapAllocation() {
		const DX12::Device* device = static_cast<const DX12::Device*>(this->device);

		DX12::DescriptorHeapAllocation descriptorHeapAllocation{};
		descriptorHeapAllocation.descriptorCount = maxViewDescriptorCountPerAllocation;
		descriptorHeapAllocation.descriptorOffset = 0;

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDescriptor{};
		descriptorHeapDescriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDescriptor.NumDescriptors = descriptorHeapAllocation.descriptorCount;
		descriptorHeapDescriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDescriptor.NodeMask = 0;

		const HRESULT result = device->device->CreateDescriptorHeap(&descriptorHeapDescriptor, IID_PPV_ARGS(&descriptorHeapAllocation.descriptorHeap));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create view descriptor heap.");

		descriptorHeapAllocations.emplace_back(descriptorHeapAllocation);

		currentViewDescriptorHeapAllocationIndex = static_cast<UINT>(descriptorHeapAllocations.size() - 1);
	}

	void DX12::MemoryAllocator::writeUniformBuffer(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, ref<BaseBufferView> bufferView) const {
		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		const DX12::BaseBuffer* _buffer = static_cast<const DX12::BaseBuffer*>(bufferView->getBuffer());

		D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDescriptor{};
		constantBufferViewDescriptor.BufferLocation = _buffer->resource->GetGPUVirtualAddress() + bufferView->getOffset();
		constantBufferViewDescriptor.SizeInBytes = static_cast<UINT>(bufferView->getSize());

		_device->device->CreateConstantBufferView(&constantBufferViewDescriptor, descriptorHandle);
	}

	void DX12::MemoryAllocator::writeReadOnlyStorageBuffer(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, ref<BaseBufferView> bufferView) const {
		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		const DX12::BaseBuffer* _buffer = static_cast<const DX12::BaseBuffer*>(bufferView->getBuffer());

		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor{};
		shaderResourceViewDescriptor.Format = DXGI_FORMAT_UNKNOWN;
		shaderResourceViewDescriptor.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		shaderResourceViewDescriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		// TODO(graphics/dx12/memory-allocator/shader-resource-view): The offset is not guaranteed to be a multiple of the stride.
		shaderResourceViewDescriptor.Buffer.FirstElement = bufferView->getOffset() / bufferView->getStride();

		shaderResourceViewDescriptor.Buffer.NumElements = static_cast<UINT>(bufferView->getSize()) / bufferView->getStride();
		shaderResourceViewDescriptor.Buffer.StructureByteStride = bufferView->getStride();
		shaderResourceViewDescriptor.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		_device->device->CreateShaderResourceView(_buffer->resource, &shaderResourceViewDescriptor, descriptorHandle);
	}

	void DX12::MemoryAllocator::writeReadWriteStorageBuffer(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, ref<BaseBufferView> bufferView) const {
		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		const DX12::BaseBuffer* _buffer = static_cast<const DX12::BaseBuffer*>(bufferView->getBuffer());

		D3D12_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDescriptor{};
		unorderedAccessViewDescriptor.Format = DXGI_FORMAT_UNKNOWN;
		unorderedAccessViewDescriptor.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;

		// TODO(graphics/dx12/memory-allocator/shader-resource-view): The offset is not guaranteed to be a multiple of the stride.
		unorderedAccessViewDescriptor.Buffer.FirstElement = bufferView->getOffset() / bufferView->getStride();

		unorderedAccessViewDescriptor.Buffer.NumElements = static_cast<UINT>(bufferView->getSize()) / bufferView->getStride();
		unorderedAccessViewDescriptor.Buffer.StructureByteStride = bufferView->getStride();
		unorderedAccessViewDescriptor.Buffer.CounterOffsetInBytes = 0;
		unorderedAccessViewDescriptor.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		_device->device->CreateUnorderedAccessView(_buffer->resource, nullptr, &unorderedAccessViewDescriptor, descriptorHandle);
	}

	void DX12::MemoryAllocator::createSamplerDescriptorHeapAllocation() {
		const DX12::Device* device = static_cast<const DX12::Device*>(this->device);

		DX12::DescriptorHeapAllocation descriptorHeapAllocation{};
		descriptorHeapAllocation.descriptorCount = maxSamplerDescriptorCountPerAllocation;
		descriptorHeapAllocation.descriptorOffset = 0;

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDescriptor{};
		descriptorHeapDescriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		descriptorHeapDescriptor.NumDescriptors = descriptorHeapAllocation.descriptorCount;
		descriptorHeapDescriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDescriptor.NodeMask = 0;

		const HRESULT result = device->device->CreateDescriptorHeap(&descriptorHeapDescriptor, IID_PPV_ARGS(&descriptorHeapAllocation.descriptorHeap));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create sampler descriptor heap.");

		descriptorHeapAllocations.emplace_back(descriptorHeapAllocation);

		currentSamplerDescriptorHeapAllocationIndex = static_cast<UINT>(descriptorHeapAllocations.size() - 1);
	}

	void DX12::MemoryAllocator::createRenderTargetViewDescriptorHeapAllocation() {
		const DX12::Device* device = static_cast<const DX12::Device*>(this->device);

		DX12::DescriptorHeapAllocation descriptorHeapAllocation{};
		descriptorHeapAllocation.descriptorCount = maxRenderTargetViewDescriptorCountPerAllocation;
		descriptorHeapAllocation.descriptorOffset = 0;

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDescriptor{};
		descriptorHeapDescriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		descriptorHeapDescriptor.NumDescriptors = descriptorHeapAllocation.descriptorCount;
		descriptorHeapDescriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descriptorHeapDescriptor.NodeMask = 0;

		const HRESULT result = device->device->CreateDescriptorHeap(&descriptorHeapDescriptor, IID_PPV_ARGS(&descriptorHeapAllocation.descriptorHeap));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create render target view descriptor heap.");

		descriptorHeapAllocations.emplace_back(descriptorHeapAllocation);

		currentRenderTargetViewDescriptorHeapAllocationIndex = static_cast<UINT>(descriptorHeapAllocations.size() - 1);
	}

	void DX12::MemoryAllocator::createDepthStencilViewDescriptorHeapAllocation() {
		const DX12::Device* device = static_cast<const DX12::Device*>(this->device);

		DX12::DescriptorHeapAllocation descriptorHeapAllocation{};
		descriptorHeapAllocation.descriptorCount = maxDepthStencilViewDescriptorCountPerAllocation;
		descriptorHeapAllocation.descriptorOffset = 0;

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDescriptor{};
		descriptorHeapDescriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		descriptorHeapDescriptor.NumDescriptors = descriptorHeapAllocation.descriptorCount;
		descriptorHeapDescriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descriptorHeapDescriptor.NodeMask = 0;

		const HRESULT result = device->device->CreateDescriptorHeap(&descriptorHeapDescriptor, IID_PPV_ARGS(&descriptorHeapAllocation.descriptorHeap));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create depth stencil view descriptor heap.");

		descriptorHeapAllocations.emplace_back(descriptorHeapAllocation);

		currentDepthStencilViewDescriptorHeapAllocationIndex = static_cast<UINT>(descriptorHeapAllocations.size() - 1);
	}

	DX12::DescriptorHeapAllocation& DX12::MemoryAllocator::getCurrentRenderTargetViewDescriptorHeapAllocation() {
		if (currentRenderTargetViewDescriptorHeapAllocationIndex >= descriptorHeapAllocations.size()) {
			createRenderTargetViewDescriptorHeapAllocation();
		}

		const DX12::DescriptorHeapAllocation& descriptorHeapAllocation = descriptorHeapAllocations[currentRenderTargetViewDescriptorHeapAllocationIndex];

		if (descriptorHeapAllocation.descriptorOffset == descriptorHeapAllocation.descriptorCount) {
			createRenderTargetViewDescriptorHeapAllocation();
		}

		return descriptorHeapAllocations[currentRenderTargetViewDescriptorHeapAllocationIndex];
	}

	DX12::DescriptorHeapAllocation& DX12::MemoryAllocator::getCurrentDepthStencilViewDescriptorHeapAllocation() {
		if (currentDepthStencilViewDescriptorHeapAllocationIndex >= descriptorHeapAllocations.size()) {
			createDepthStencilViewDescriptorHeapAllocation();
		}

		const DX12::DescriptorHeapAllocation& descriptorHeapAllocation = descriptorHeapAllocations[currentDepthStencilViewDescriptorHeapAllocationIndex];

		if (descriptorHeapAllocation.descriptorOffset == descriptorHeapAllocation.descriptorCount) {
			createDepthStencilViewDescriptorHeapAllocation();
		}

		return descriptorHeapAllocations[currentDepthStencilViewDescriptorHeapAllocationIndex];
	}

	MemoryAllocation DX12::allocateMemory(Trident::MemoryAllocator* memoryAllocator, bool dynamic, uint32 memoryTypeIndex, uint64 size, uint64 alignment) {
		const DX12::Device* _device = static_cast<const DX12::Device*>(memoryAllocator->device);

		D3D12_HEAP_DESC heapDescriptor{};
		heapDescriptor.SizeInBytes = size;
		heapDescriptor.Properties.Type = static_cast<D3D12_HEAP_TYPE>(memoryTypeIndex + 1);
		heapDescriptor.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapDescriptor.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapDescriptor.Properties.CreationNodeMask = 1;
		heapDescriptor.Properties.VisibleNodeMask = 1;
		heapDescriptor.Alignment = alignment;
		heapDescriptor.Flags = D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;

		ID3D12Heap* heap;

		const HRESULT result = _device->device->CreateHeap(&heapDescriptor, IID_PPV_ARGS(&heap));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create heap.");

		MemoryAllocation memoryAllocation{};
		memoryAllocation.dynamic = dynamic;
		memoryAllocation.memory = reinterpret_cast<uint64>(heap);
		memoryAllocation.memoryTypeIndex = memoryTypeIndex;
		memoryAllocation.size = size;
		memoryAllocation.usedOffset = 0;
		memoryAllocation.suballocationCount = 0;
		memoryAllocation.mappedSuballocationCount = 0;
		memoryAllocation.mappedRange = nullptr;

		return memoryAllocation;
	}

	uint32 DX12::findMemoryTypeIndex(const Trident::MemoryAllocator* memoryAllocator, MemoryProperty memoryProperties, bool transferDestination) {
		D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT;

		if (memoryProperties & MemoryProperty::HOST_VISIBLE) {
			heapType = D3D12_HEAP_TYPE_UPLOAD;

			if (!(memoryProperties & MemoryProperty::DEVICE_LOCAL) && transferDestination) {
				heapType = D3D12_HEAP_TYPE_READBACK;
			}
		}

		return static_cast<uint32>(heapType) - 1;
	}

	MemoryRequirements DX12::getBufferMemoryRequirements(const Trident::MemoryAllocator* memoryAllocator, ref<Trident::BaseBuffer> buffer) {
		const DX12::Device* _device = static_cast<const DX12::Device*>(memoryAllocator->device);

		D3D12_RESOURCE_DESC resourceDescriptor{};
		resourceDescriptor.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDescriptor.Alignment = 0;
		resourceDescriptor.Width = buffer->getSize();
		resourceDescriptor.Height = 1;
		resourceDescriptor.DepthOrArraySize = 1;
		resourceDescriptor.MipLevels = 1;
		resourceDescriptor.Format = DXGI_FORMAT_UNKNOWN;
		resourceDescriptor.SampleDesc.Count = 1;
		resourceDescriptor.SampleDesc.Quality = 0;
		resourceDescriptor.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDescriptor.Flags = D3D12_RESOURCE_FLAG_NONE;

		const D3D12_RESOURCE_ALLOCATION_INFO resourceAllocationInfo = _device->device->GetResourceAllocationInfo(0, 1, &resourceDescriptor);

		MemoryRequirements memoryRequirements{};
		memoryRequirements.size = resourceAllocationInfo.SizeInBytes;
		memoryRequirements.alignment = resourceAllocationInfo.Alignment;
		memoryRequirements.memoryTypeBits = 1;

		return memoryRequirements;
	}

	MemoryRequirements DX12::getImageMemoryRequirements(const Trident::MemoryAllocator* memoryAllocator, ref<Trident::Image> image) {
		const DX12::Device* _device = static_cast<const DX12::Device*>(memoryAllocator->device);
		const Extent3D extent = image->getExtent();
		const ImageUsage usage = image->getUsage();

		D3D12_RESOURCE_DESC resourceDescriptor{};
		resourceDescriptor.Dimension = getImageResourceDimension(image->getType());
		resourceDescriptor.Alignment = 0;
		resourceDescriptor.Width = extent.width;
		resourceDescriptor.Height = extent.height;
		resourceDescriptor.DepthOrArraySize = extent.depth * image->getLayerCount();
		resourceDescriptor.MipLevels = image->getMipLevelCount();
		resourceDescriptor.Format = getImageFormat(image->getFormat());
		resourceDescriptor.SampleDesc.Count = image->getSampleCount();
		resourceDescriptor.SampleDesc.Quality = 0;
		resourceDescriptor.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDescriptor.Flags = getImageResourceFlags(usage);

		const D3D12_RESOURCE_ALLOCATION_INFO resourceAllocationInfo = _device->device->GetResourceAllocationInfo(0, 1, &resourceDescriptor);

		MemoryRequirements memoryRequirements{};
		memoryRequirements.size = resourceAllocationInfo.SizeInBytes;
		memoryRequirements.alignment = resourceAllocationInfo.Alignment;
		memoryRequirements.memoryTypeBits = 1;

		return memoryRequirements;
	}

	MemorySuballocation DX12::suballocateBufferMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, ref<Trident::BaseBuffer> buffer) {
		const DX12::Device* _device = static_cast<const DX12::Device*>(memoryAllocator->device);
		const ref<DX12::BaseBuffer> _buffer = std::static_pointer_cast<DX12::BaseBuffer>(buffer);
		const MemoryAllocation& memoryAllocation = memoryAllocator->allocations[allocationIndex];

		D3D12_RESOURCE_DESC resourceDescriptor{};
		resourceDescriptor.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDescriptor.Alignment = 0;
		resourceDescriptor.Width = buffer->getSize();
		resourceDescriptor.Height = 1;
		resourceDescriptor.DepthOrArraySize = 1;
		resourceDescriptor.MipLevels = 1;
		resourceDescriptor.Format = DXGI_FORMAT_UNKNOWN;
		resourceDescriptor.SampleDesc.Count = 1;
		resourceDescriptor.SampleDesc.Quality = 0;
		resourceDescriptor.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDescriptor.Flags = D3D12_RESOURCE_FLAG_NONE;

		const D3D12_RESOURCE_ALLOCATION_INFO resourceAllocationInfo = _device->device->GetResourceAllocationInfo(0, 1, &resourceDescriptor);

		D3D12_RESOURCE_STATES initialResourceStates = D3D12_RESOURCE_STATE_COMMON;

		if (buffer->getMemorySuballocation().memoryProperties & MemoryProperty::DEVICE_LOCAL && buffer->getUsage() & BufferUsage::READ_WRITE_STORAGE) {
			resourceDescriptor.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		if (buffer->getMemorySuballocation().memoryProperties & MemoryProperty::HOST_VISIBLE) {
			initialResourceStates = D3D12_RESOURCE_STATE_GENERIC_READ;

			if (buffer->getUsage() == BufferUsage::TRANSFER_DST) {
				initialResourceStates = D3D12_RESOURCE_STATE_COPY_DEST;
			}
		}

		MemorySuballocation memorySuballocation = memoryAllocator->suballocate(allocationIndex, resourceAllocationInfo.SizeInBytes, resourceAllocationInfo.Alignment);
		memorySuballocation.memoryProperties = buffer->getMemorySuballocation().memoryProperties;

		HRESULT result;

		{
			PROFILE_SCOPE("ID3D12Device::CreatePlacedResource (buffer)");

			result = _device->device->CreatePlacedResource(reinterpret_cast<ID3D12Heap*>(memoryAllocation.memory), memorySuballocation.offset, &resourceDescriptor, initialResourceStates, nullptr, IID_PPV_ARGS(&_buffer->resource));
		}

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create placed buffer resource.");

		#if defined(DEBUG_OBJECT_NAME)
			if (_buffer->name.count() > 0) {
				const LPWSTR name = ConvertUTF8ToUTF16(&_buffer->name[0]);

				result = _buffer->resource->SetName(name);

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set resource name.");

				free(name);

				_buffer->name = {};
			}
		#endif

		return memorySuballocation;
	}

	MemorySuballocation DX12::suballocateImageMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, ref<Trident::Image> image) {
		const DX12::Device* _device = static_cast<const DX12::Device*>(memoryAllocator->device);
		const ref<DX12::Image> _image = std::static_pointer_cast<DX12::Image>(image);
		const Extent3D extent = image->getExtent();
		const ImageUsage usage = image->getUsage();
		const MemoryAllocation& memoryAllocation = memoryAllocator->allocations[allocationIndex];

		D3D12_RESOURCE_DESC resourceDescriptor{};
		resourceDescriptor.Dimension = getImageResourceDimension(image->getType());
		resourceDescriptor.Alignment = 0;
		resourceDescriptor.Width = extent.width;
		resourceDescriptor.Height = extent.height;
		resourceDescriptor.DepthOrArraySize = extent.depth * image->getLayerCount();
		resourceDescriptor.MipLevels = image->getMipLevelCount();
		resourceDescriptor.Format = getImageFormat(image->getFormat());
		resourceDescriptor.SampleDesc.Count = image->getSampleCount();
		resourceDescriptor.SampleDesc.Quality = 0;
		resourceDescriptor.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDescriptor.Flags = getImageResourceFlags(usage);

		const D3D12_RESOURCE_ALLOCATION_INFO resourceAllocationInfo = _device->device->GetResourceAllocationInfo(0, 1, &resourceDescriptor);

		const D3D12_HEAP_TYPE heapType = static_cast<D3D12_HEAP_TYPE>(memoryAllocation.memoryTypeIndex + 1);

		D3D12_RESOURCE_STATES initialResourceStates = D3D12_RESOURCE_STATE_COMMON;

		if (heapType == D3D12_HEAP_TYPE_UPLOAD) {
			initialResourceStates = D3D12_RESOURCE_STATE_GENERIC_READ;
		}
		else if (heapType == D3D12_HEAP_TYPE_READBACK) {
			initialResourceStates = D3D12_RESOURCE_STATE_COPY_DEST;
		}

		const D3D12_CLEAR_VALUE* optimizedClearValuePointer = nullptr;

		D3D12_CLEAR_VALUE optimizedClearValue{};
		optimizedClearValue.Format = resourceDescriptor.Format;

		if (usage & ImageUsage::COLOR_ATTACHMENT) {
			optimizedClearValue.Color[0] = 0.0f;
			optimizedClearValue.Color[1] = 0.0f;
			optimizedClearValue.Color[2] = 0.0f;
			optimizedClearValue.Color[3] = 1.0f;

			optimizedClearValuePointer = &optimizedClearValue;
		}
		else if (usage & ImageUsage::DEPTH_STENCIL_ATTACHMENT) {
			optimizedClearValue.DepthStencil.Depth = 1.0f;
			optimizedClearValue.DepthStencil.Stencil = 0;

			optimizedClearValuePointer = &optimizedClearValue;
		}

		MemorySuballocation memorySuballocation = memoryAllocator->suballocate(allocationIndex, resourceAllocationInfo.SizeInBytes, resourceAllocationInfo.Alignment);
		memorySuballocation.memoryProperties = image->getMemorySuballocation().memoryProperties;

		HRESULT result;

		{
			PROFILE_SCOPE("ID3D12Device::CreatePlacedResource (image)");

			// TODO(graphics/dx12/placed-rtv-dsv): Fix initialization error when first clearing render targets and/or depth stencil images created with CreatePlacedResource.
			if (usage & (ImageUsage::COLOR_ATTACHMENT | ImageUsage::DEPTH_STENCIL_ATTACHMENT)) {
				const D3D12_HEAP_DESC heapDescriptor = ((ID3D12Heap*) memoryAllocation.memory)->GetDesc();

				result = _device->device->CreateCommittedResource(&heapDescriptor.Properties, heapDescriptor.Flags, &resourceDescriptor, initialResourceStates, optimizedClearValuePointer, IID_PPV_ARGS(&_image->resource));
			}
			else {
				result = _device->device->CreatePlacedResource(reinterpret_cast<ID3D12Heap*>(memoryAllocation.memory), memorySuballocation.offset, &resourceDescriptor, initialResourceStates, optimizedClearValuePointer, IID_PPV_ARGS(&_image->resource));
			}
		}

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create placed image resource.");

		#if defined(DEBUG_OBJECT_NAME)
			if (_image->name.count() > 0) {
				const LPWSTR name = ConvertUTF8ToUTF16(&_image->name[0]);

				result = _image->resource->SetName(name);

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set resource name.");

				free(name);

				_image->name = {};
			}
		#endif

		return memorySuballocation;
	}
}