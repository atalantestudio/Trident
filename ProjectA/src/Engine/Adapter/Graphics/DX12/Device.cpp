// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/DescriptorSet.hpp"
#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/Queue.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::Device::Device(const DX12::Instance& instance, const DX12::PhysicalDevice& physicalDevice) :
		physicalDevice(physicalDevice)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result = -1;

		#ifndef PREFER_SOFTWARE_ADAPTER
			result = D3D12CreateDevice(physicalDevice.adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
		#endif

		if (FAILED(result)) {
			#ifndef PREFER_SOFTWARE_ADAPTER
				ADAPTER_WARNING("Failed to create device using the primary display adapter. Falling back to a WARP adapter.");
			#endif

			ComPtr<IDXGIAdapter> adapter;

			result = instance.factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to enumerate WARP adapter.");

			result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create device using the WARP adapter.");
		}

		queues[0] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_DIRECT);
		queues[1] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		queues[2] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_COPY);

		fenceEvent = CreateEvent(nullptr, false, false, nullptr);

		WIN32_ASSERT(fenceEvent != nullptr);
	}

	DX12::Device::~Device() {
		const BOOL result = CloseHandle(fenceEvent);

		WIN32_ASSERT(result != 0);
	}

	AcquireNextImageStatus DX12::Device::acquireNextImage(ref<ProjectA::Swapchain> swapchain, ref<Semaphore> semaphore, uint32& imageIndex) const {
		imageIndex = std::static_pointer_cast<DX12::Swapchain>(swapchain)->swapChain->GetCurrentBackBufferIndex();

		return AcquireNextImageStatus::SUCCESS;
	}

	sequence<ref<DescriptorSet>> DX12::Device::allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) {
		HRESULT result;
		sequence<ref<ProjectA::DescriptorSet>> descriptorSets(descriptorSetLayouts.count());
		UINT descriptorSetIndex = 0;
		UINT bindingIndex = 0;
		UINT shaderResourceViewDescriptorCount = 0;
		UINT samplerDescriptorCount = 0;

		while (descriptorSetIndex < descriptorSetLayouts.count()) {
			const ref<DescriptorSetLayout> descriptorSetLayout = descriptorSetLayouts[descriptorSetIndex];

			bindingIndex = 0;
			samplerDescriptorCount = 0;
			shaderResourceViewDescriptorCount = 0;

			const ref<DX12::DescriptorSet> descriptorSet = createRef<DX12::DescriptorSet>();
			descriptorSet->bindings = sequence<DX12::DescriptorSet::DescriptorSetBinding>(descriptorSetLayout->bindings.count());

			for (const DescriptorSetLayoutBinding& binding : descriptorSetLayout->bindings) {
				if (binding.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER) {
					shaderResourceViewDescriptorCount += binding.descriptorCount;
					samplerDescriptorCount += binding.descriptorCount;
				}
				else {
					ENGINE_ERROR_UNIMPLEMENTED();
				}
			}

			if (shaderResourceViewDescriptorCount > 0) {
				D3D12_DESCRIPTOR_HEAP_DESC shaderResourceViewDescriptorHeapDescriptor{};
				shaderResourceViewDescriptorHeapDescriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				shaderResourceViewDescriptorHeapDescriptor.NumDescriptors = shaderResourceViewDescriptorCount;
				shaderResourceViewDescriptorHeapDescriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				shaderResourceViewDescriptorHeapDescriptor.NodeMask = 0;

				result = device->CreateDescriptorHeap(&shaderResourceViewDescriptorHeapDescriptor, IID_PPV_ARGS(&descriptorSet->shaderResourceViewDescriptorHeap));

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create shader resource view descriptor heap.");
			}

			if (samplerDescriptorCount > 0) {
				D3D12_DESCRIPTOR_HEAP_DESC samplerDescriptorHeapDescriptor{};
				samplerDescriptorHeapDescriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
				samplerDescriptorHeapDescriptor.NumDescriptors = samplerDescriptorCount;
				samplerDescriptorHeapDescriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				samplerDescriptorHeapDescriptor.NodeMask = 0;

				result = device->CreateDescriptorHeap(&samplerDescriptorHeapDescriptor, IID_PPV_ARGS(&descriptorSet->samplerDescriptorHeap));

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create sampler descriptor heap.");
			}

			while (bindingIndex < descriptorSetLayout->bindings.count()) {
				const DescriptorSetLayoutBinding& binding = descriptorSetLayout->bindings[bindingIndex];

				DX12::DescriptorSet::DescriptorSetBinding& descriptorSetBinding = descriptorSet->bindings[bindingIndex];
				descriptorSetBinding.descriptorType = binding.descriptorType;
				descriptorSetBinding.descriptorCount = binding.descriptorCount;

				bindingIndex += 1;
			}

			descriptorSets[descriptorSetIndex] = descriptorSet;

			descriptorSetIndex += 1;
		}

		return descriptorSets;
	}

	ref<PhysicalDevice> DX12::Device::getPhysicalDevice() const {
		return createRef<DX12::PhysicalDevice>(physicalDevice);
	}

	ref<ProjectA::Queue> DX12::Device::getQueue(QueueFamilyCapability queueFamilyCapability) const {
		return queues[static_cast<uint32>(queueFamilyCapability)];
	}

	void DX12::Device::resetFence(ref<ProjectA::Fence> fence) const {
		const HRESULT result = std::static_pointer_cast<DX12::Fence>(fence)->fence->Signal(0);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to add signal to fence.");
	}

	void DX12::Device::updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const {
		UINT writeDescriptorSetIndex = 0;

		while (writeDescriptorSetIndex < writeDescriptorSets.count()) {
			const WriteDescriptorSet& writeDescriptorSet = writeDescriptorSets[writeDescriptorSetIndex];
			const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(writeDescriptorSet.descriptorSet);
			const DX12::DescriptorSet::DescriptorSetBinding& binding = descriptorSet->bindings[writeDescriptorSet.bindingIndex];

			if (isBufferDescriptorType(binding.descriptorType)) {
				UINT descriptorBufferIndex = 0;

				while (descriptorBufferIndex < writeDescriptorSet.descriptorBuffers.count()) {
					const DescriptorBuffer& descriptorBuffer = writeDescriptorSet.descriptorBuffers[descriptorBufferIndex];

					ENGINE_ERROR_UNIMPLEMENTED();

					descriptorBufferIndex += 1;
				}
			}
			else if (isImageDescriptorType(binding.descriptorType)) {
				static const UINT descriptorHeapIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				D3D12_CPU_DESCRIPTOR_HANDLE descriptorHeapOffset = descriptorSet->shaderResourceViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				UINT descriptorImageIndex = 0;

				while (descriptorImageIndex < writeDescriptorSet.descriptorImages.count()) {
					const DescriptorImage& descriptorImage = writeDescriptorSet.descriptorImages[descriptorImageIndex];
					const DX12::Image* image = static_cast<const DX12::Image*>(descriptorImage.imageView->getImage());

					ADAPTER_ASSERT(image->getType() == ImageType::TEXTURE_2D, "Images other than TEXTURE_2D are not supported.");

					D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor = {};
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

					device->CreateShaderResourceView(image->resource.Get(), &shaderResourceViewDescriptor, descriptorHeapOffset);

					descriptorHeapOffset.ptr += descriptorHeapIncrementSize;
					descriptorImageIndex += 1;
				}
			}
			else {
				ADAPTER_ASSERT(false, "The descriptor must either have a buffer type or an image type.");
			}

			writeDescriptorSetIndex += 1;
		}
	}

	void DX12::Device::waitForFence(ref<ProjectA::Fence> fence) const {
		// TODO: Move this inside Queue::submit().
		const ref<DX12::Queue> queue = std::static_pointer_cast<DX12::Queue>(getQueue(QueueFamilyCapability::GRAPHICS));

		HRESULT result;

		result = queue->commandQueue->Signal(std::static_pointer_cast<DX12::Fence>(fence)->fence.Get(), 1);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to add signal to fence.");

		result = ResetEvent(fenceEvent);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to reset event.");

		result = std::static_pointer_cast<DX12::Fence>(fence)->fence->SetEventOnCompletion(1, fenceEvent);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set event on fence completion.");

		const DWORD code = WaitForSingleObject(fenceEvent, INFINITE);

		ADAPTER_ASSERT(code == WAIT_OBJECT_0, "Failed to wait for event.");
	}

	void DX12::Device::waitIdle() const {
		for (const ref<ProjectA::Queue> queue : queues) {
			queue->waitIdle();
		}
	}
}