// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/Adapter/Graphics/DX12/MemoryAllocator.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::MemoryAllocator::MemoryAllocator(const DX12::Device* device) :
		Trident::MemoryAllocator(),
		device(device)
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
		DX12::DescriptorHeapAllocation& descriptorHeapAllocation = getCurrentRenderTargetViewDescriptorHeapAllocation();

		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = descriptorHeapAllocation.descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		descriptorHandle.ptr += descriptorHeapAllocation.descriptorOffset * renderTargetViewDescriptorHandleIncrementSize;

		device->device->CreateRenderTargetView(resource, descriptor, descriptorHandle);

		descriptorHeapAllocation.descriptorOffset += 1;

		return descriptorHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DX12::MemoryAllocator::allocateDepthStencilView(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* descriptor) {
		DX12::DescriptorHeapAllocation& descriptorHeapAllocation = getCurrentDepthStencilViewDescriptorHeapAllocation();

		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = descriptorHeapAllocation.descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		descriptorHandle.ptr += descriptorHeapAllocation.descriptorOffset * depthStencilViewDescriptorHandleIncrementSize;

		device->device->CreateDepthStencilView(resource, descriptor, descriptorHandle);

		descriptorHeapAllocation.descriptorOffset += 1;

		return descriptorHandle;
	}

	void DX12::MemoryAllocator::writeImage(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, const DescriptorImage& descriptorImage) const {
		const DX12::Image* image = static_cast<const DX12::Image*>(descriptorImage.imageView->getImage());

		ADAPTER_ASSERT(image->getType() == ImageType::TEXTURE_2D, "Images other than TEXTURE_2D are not supported.");

		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor{};
		shaderResourceViewDescriptor.Format = getImageFormat(image->getFormat());

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
		const ref<DX12::Buffer> buffer = std::static_pointer_cast<DX12::Buffer>(descriptorBuffer.buffer);

		D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDescriptor{};
		constantBufferViewDescriptor.BufferLocation = buffer->resource->GetGPUVirtualAddress() + descriptorBuffer.offset;
		constantBufferViewDescriptor.SizeInBytes = static_cast<UINT>(descriptorBuffer.size);

		device->device->CreateConstantBufferView(&constantBufferViewDescriptor, descriptorHandle);
	}

	void DX12::MemoryAllocator::writeReadOnlyStorageBuffer(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, const DescriptorBuffer& descriptorBuffer) const {
		const ref<DX12::Buffer> buffer = std::static_pointer_cast<DX12::Buffer>(descriptorBuffer.buffer);

		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor{};
		shaderResourceViewDescriptor.Format = DXGI_FORMAT_UNKNOWN;
		shaderResourceViewDescriptor.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		shaderResourceViewDescriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		shaderResourceViewDescriptor.Buffer.FirstElement = descriptorBuffer.offset;
		shaderResourceViewDescriptor.Buffer.NumElements = static_cast<UINT>(descriptorBuffer.size);
		shaderResourceViewDescriptor.Buffer.StructureByteStride = 1;
		shaderResourceViewDescriptor.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		device->device->CreateShaderResourceView(buffer->resource, &shaderResourceViewDescriptor, descriptorHandle);
	}

	void DX12::MemoryAllocator::writeReadWriteStorageBuffer(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, const DescriptorBuffer& descriptorBuffer) const {
		const ref<DX12::Buffer> buffer = std::static_pointer_cast<DX12::Buffer>(descriptorBuffer.buffer);

		D3D12_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDescriptor{};
		unorderedAccessViewDescriptor.Format = DXGI_FORMAT_UNKNOWN;
		unorderedAccessViewDescriptor.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		unorderedAccessViewDescriptor.Buffer.FirstElement = descriptorBuffer.offset;
		unorderedAccessViewDescriptor.Buffer.NumElements = static_cast<UINT>(descriptorBuffer.size);
		unorderedAccessViewDescriptor.Buffer.StructureByteStride = 1;
		unorderedAccessViewDescriptor.Buffer.CounterOffsetInBytes = 0;
		unorderedAccessViewDescriptor.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		device->device->CreateUnorderedAccessView(buffer->resource, nullptr, &unorderedAccessViewDescriptor, descriptorHandle);
	}

	void DX12::MemoryAllocator::writeInputAttachment(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, const DescriptorImage& descriptorImage) const {
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
		device->device->CreateSampler(descriptor, descriptorHandle);
	}

	void DX12::MemoryAllocator::createViewDescriptorHeapAllocation() {
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

	void DX12::MemoryAllocator::createSamplerDescriptorHeapAllocation() {
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
}