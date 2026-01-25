// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/MemoryAllocator.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::MemoryAllocator::MemoryAllocator(const DX12::Device& device) :
		device(device)
	{
		viewDescriptorHandleIncrementSize = device.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		samplerDescriptorHandleIncrementSize = device.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		renderTargetViewDescriptorHandleIncrementSize = device.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		depthStencilViewDescriptorHandleIncrementSize = device.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}

	DX12::MemoryAllocator::~MemoryAllocator() {
		INT descriptorHeapAllocationIndex = static_cast<INT>(descriptorHeapAllocations.size()) - 1;

		while (descriptorHeapAllocationIndex >= descriptorHeapAllocations.size()) {
			descriptorHeapAllocations[descriptorHeapAllocationIndex].samplerDescriptorHeap->Release();

			descriptorHeapAllocationIndex -= 1;
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DX12::MemoryAllocator::allocateSampler(const D3D12_SAMPLER_DESC* descriptor) {
		DX12::DescriptorHeapAllocation& descriptorHeapAllocation = getCurrentSamplerDescriptorHeapAllocation();

		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = descriptorHeapAllocation.samplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		descriptorHandle.ptr += descriptorHeapAllocation.descriptorOffset * samplerDescriptorHandleIncrementSize;

		device.device->CreateSampler(descriptor, descriptorHandle);

		descriptorHeapAllocation.descriptorOffset += 1;

		return descriptorHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DX12::MemoryAllocator::allocateRenderTargetView(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* descriptor) {
		DX12::DescriptorHeapAllocation& descriptorHeapAllocation = getCurrentRenderTargetViewDescriptorHeapAllocation();

		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = descriptorHeapAllocation.samplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		descriptorHandle.ptr += descriptorHeapAllocation.descriptorOffset * renderTargetViewDescriptorHandleIncrementSize;

		device.device->CreateRenderTargetView(resource, descriptor, descriptorHandle);

		descriptorHeapAllocation.descriptorOffset += 1;

		return descriptorHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DX12::MemoryAllocator::allocateDepthStencilView(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* descriptor) {
		DX12::DescriptorHeapAllocation& descriptorHeapAllocation = getCurrentDepthStencilViewDescriptorHeapAllocation();

		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = descriptorHeapAllocation.samplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		descriptorHandle.ptr += descriptorHeapAllocation.descriptorOffset * depthStencilViewDescriptorHandleIncrementSize;

		device.device->CreateDepthStencilView(resource, descriptor, descriptorHandle);

		descriptorHeapAllocation.descriptorOffset += 1;

		return descriptorHandle;
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

		const HRESULT result = device.device->CreateDescriptorHeap(&descriptorHeapDescriptor, IID_PPV_ARGS(&descriptorHeapAllocation.samplerDescriptorHeap));

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

		const HRESULT result = device.device->CreateDescriptorHeap(&descriptorHeapDescriptor, IID_PPV_ARGS(&descriptorHeapAllocation.samplerDescriptorHeap));

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

		const HRESULT result = device.device->CreateDescriptorHeap(&descriptorHeapDescriptor, IID_PPV_ARGS(&descriptorHeapAllocation.samplerDescriptorHeap));

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

		const HRESULT result = device.device->CreateDescriptorHeap(&descriptorHeapDescriptor, IID_PPV_ARGS(&descriptorHeapAllocation.samplerDescriptorHeap));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create depth stencil view descriptor heap.");

		descriptorHeapAllocations.emplace_back(descriptorHeapAllocation);

		currentDepthStencilViewDescriptorHeapAllocationIndex = static_cast<UINT>(descriptorHeapAllocations.size() - 1);
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