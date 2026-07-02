// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/Sampler.hpp"
#include "Engine/API/Graphics/DescriptorSet.hpp"
#include "Engine/API/Graphics/MemoryAllocator.hpp"
#include "Engine/API/Graphics/MemoryRequirements.hpp"

namespace Trident {
	struct DX12::DescriptorHeapAllocation {
		ID3D12DescriptorHeap* descriptorHeap;
		UINT descriptorCount;

		// Increased as the heap is suballocated.
		UINT descriptorOffset;
	};

	class DX12::MemoryAllocator : public Trident::MemoryAllocator {
		private:
			static constexpr UINT UNDEFINED_ALLOCATION_INDEX = max<UINT>();

			// NOTE
			// Only one descriptor heap of each type can be bound at a time.
			// See https://learn.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12graphicscommandlist-setdescriptorheaps. 
			static constexpr UINT maxViewDescriptorCountPerAllocation = 64;
			static constexpr UINT maxSamplerDescriptorCountPerAllocation = 16;
			static constexpr UINT maxRenderTargetViewDescriptorCountPerAllocation = 16;
			static constexpr UINT maxDepthStencilViewDescriptorCountPerAllocation = 16;

		public:
			explicit MemoryAllocator(const DX12::Device* device);

			void destroyDescriptorHeaps();

			DX12::DescriptorHeapAllocation& getCurrentViewDescriptorHeapAllocation();
			DX12::DescriptorHeapAllocation& getCurrentSamplerDescriptorHeapAllocation();

			// Reserves space for a sequence of CBV/SRV/UAV handles.
			D3D12_CPU_DESCRIPTOR_HANDLE allocateViewDescriptors(UINT descriptorCount);

			// Reserves space for a sequence of sampler handles.
			D3D12_CPU_DESCRIPTOR_HANDLE allocateSamplerDescriptors(UINT descriptorCount);

			D3D12_CPU_DESCRIPTOR_HANDLE allocateRenderTargetView(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* descriptor);
			D3D12_CPU_DESCRIPTOR_HANDLE allocateDepthStencilView(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* descriptor);

			void writeUniformBuffer(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, ref<BaseBufferView> bufferView) const;
			void writeReadOnlyStorageBuffer(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, ref<BaseBufferView> bufferView) const;
			void writeReadWriteStorageBuffer(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, ref<BaseBufferView> bufferView) const;
			void writeImage(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, const DescriptorImage& descriptorImage) const;
			void writeReadWriteStorageImage(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, const DescriptorImage& descriptorImage) const;
			void writeInputAttachment(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, const DescriptorImage& descriptorImage) const;
			void writeSampler(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, const D3D12_SAMPLER_DESC* descriptor) const;

		private:
			void createViewDescriptorHeapAllocation();
			void createSamplerDescriptorHeapAllocation();
			void createRenderTargetViewDescriptorHeapAllocation();
			void createDepthStencilViewDescriptorHeapAllocation();

			DX12::DescriptorHeapAllocation& getCurrentRenderTargetViewDescriptorHeapAllocation();
			DX12::DescriptorHeapAllocation& getCurrentDepthStencilViewDescriptorHeapAllocation();

		public:
			UINT viewDescriptorHandleIncrementSize;
			UINT samplerDescriptorHandleIncrementSize;
			UINT renderTargetViewDescriptorHandleIncrementSize;
			UINT depthStencilViewDescriptorHandleIncrementSize;

		private:
			std::vector<DX12::DescriptorHeapAllocation> descriptorHeapAllocations;

			UINT currentViewDescriptorHeapAllocationIndex = UNDEFINED_ALLOCATION_INDEX;
			UINT currentSamplerDescriptorHeapAllocationIndex = UNDEFINED_ALLOCATION_INDEX;
			UINT currentRenderTargetViewDescriptorHeapAllocationIndex = UNDEFINED_ALLOCATION_INDEX;
			UINT currentDepthStencilViewDescriptorHeapAllocationIndex = UNDEFINED_ALLOCATION_INDEX;
	};

	namespace DX12 {
		MemoryAllocation allocateMemory(Trident::MemoryAllocator* memoryAllocator, bool dynamic, uint32 memoryTypeIndex, uint64 size, uint64 alignment);

		uint32 findMemoryTypeIndex(const Trident::MemoryAllocator* memoryAllocator, MemoryProperty memoryProperties, bool transferDestination);

		MemoryRequirements getBufferMemoryRequirements(const Trident::MemoryAllocator* memoryAllocator, ref<Trident::Buffer> buffer);

		MemoryRequirements getImageMemoryRequirements(const Trident::MemoryAllocator* memoryAllocator, ref<Trident::Image> image);

		MemorySuballocation suballocateBufferMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, ref<Trident::Buffer> buffer);

		MemorySuballocation suballocateImageMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, ref<Trident::Image> image);
	}
}