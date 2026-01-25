// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/Sampler.hpp"

namespace ProjectA {
	struct DX12::DescriptorHeapAllocation {
		ID3D12DescriptorHeap* samplerDescriptorHeap;
		UINT descriptorCount;

		// Increased as the heap is suballocated.
		UINT descriptorOffset;
	};

	class DX12::MemoryAllocator {
		private:
			static constexpr UINT UNDEFINED_ALLOCATION_INDEX = max<UINT>();

			static constexpr UINT maxViewDescriptorCountPerAllocation = 16;
			static constexpr UINT maxSamplerDescriptorCountPerAllocation = 16;
			static constexpr UINT maxRenderTargetViewDescriptorCountPerAllocation = 16;
			static constexpr UINT maxDepthStencilViewDescriptorCountPerAllocation = 16;

		public:
			explicit MemoryAllocator(const DX12::Device& device);

			~MemoryAllocator();

			D3D12_CPU_DESCRIPTOR_HANDLE allocateSampler(const D3D12_SAMPLER_DESC* descriptor);
			D3D12_CPU_DESCRIPTOR_HANDLE allocateRenderTargetView(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* descriptor);
			D3D12_CPU_DESCRIPTOR_HANDLE allocateDepthStencilView(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* descriptor);

		private:
			void createViewDescriptorHeapAllocation();
			void createSamplerDescriptorHeapAllocation();
			void createRenderTargetViewDescriptorHeapAllocation();
			void createDepthStencilViewDescriptorHeapAllocation();

			DX12::DescriptorHeapAllocation& getCurrentViewDescriptorHeapAllocation();
			DX12::DescriptorHeapAllocation& getCurrentSamplerDescriptorHeapAllocation();
			DX12::DescriptorHeapAllocation& getCurrentRenderTargetViewDescriptorHeapAllocation();
			DX12::DescriptorHeapAllocation& getCurrentDepthStencilViewDescriptorHeapAllocation();

		private:
			const DX12::Device& device;

			UINT viewDescriptorHandleIncrementSize;
			UINT samplerDescriptorHandleIncrementSize;
			UINT renderTargetViewDescriptorHandleIncrementSize;
			UINT depthStencilViewDescriptorHandleIncrementSize;

			std::vector<DX12::DescriptorHeapAllocation> descriptorHeapAllocations;

			UINT currentViewDescriptorHeapAllocationIndex = UNDEFINED_ALLOCATION_INDEX;
			UINT currentSamplerDescriptorHeapAllocationIndex = UNDEFINED_ALLOCATION_INDEX;
			UINT currentRenderTargetViewDescriptorHeapAllocationIndex = UNDEFINED_ALLOCATION_INDEX;
			UINT currentDepthStencilViewDescriptorHeapAllocationIndex = UNDEFINED_ALLOCATION_INDEX;
	};
}