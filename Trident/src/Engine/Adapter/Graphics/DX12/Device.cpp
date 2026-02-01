// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/DescriptorSet.hpp"
#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/ImageView.hpp"
#include "Engine/Adapter/Graphics/DX12/Queue.hpp"
#include "Engine/Adapter/Graphics/DX12/Sampler.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	#if defined(BUILD_MODE_DEBUG) && defined(DEBUG_ENABLE_VALIDATION_LAYERS)
		void DX12::Device::messageCallback(D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID id, LPCSTR description, void* context) {
			switch (severity) {
				case D3D12_MESSAGE_SEVERITY_CORRUPTION:
				case D3D12_MESSAGE_SEVERITY_ERROR: {
					ADAPTER_ERROR(description);

					break;
				}
				case D3D12_MESSAGE_SEVERITY_WARNING: {
					ADAPTER_WARNING(description);

					break;
				}
				case D3D12_MESSAGE_SEVERITY_INFO: {
					ADAPTER_INFO(description);

					break;
				}
				case D3D12_MESSAGE_SEVERITY_MESSAGE: {
					ADAPTER_TRACE(description);

					break;
				}
			}
		}
	#endif

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

			IDXGIAdapter* adapter;

			result = instance.factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to enumerate WARP adapter.");

			result = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create device using the WARP adapter.");

			adapter->Release();
		}

		queues[0] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_DIRECT);
		queues[1] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		queues[2] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_COPY);

		waitEvent = CreateEvent(nullptr, false, false, L"Wait event");

		WIN32_ASSERT(waitEvent != nullptr);

		#if defined(BUILD_MODE_DEBUG) && defined(DEBUG_ENABLE_VALIDATION_LAYERS)
			result = device->QueryInterface(IID_PPV_ARGS(&infoQueue));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to query info queue interface.");

			result = infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set severity breakpoint.");

			result = infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set severity breakpoint.");

			result = infoQueue->RegisterMessageCallback(messageCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &messageCallbackCookie);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to register message callback.");
		#endif
	}

	DX12::Device::~Device() {
		HRESULT result;

		result = CloseHandle(waitEvent);

		WIN32_ASSERT(result != 0);

		#if defined(BUILD_MODE_DEBUG) && defined(DEBUG_ENABLE_VALIDATION_LAYERS)
			result = infoQueue->UnregisterMessageCallback(messageCallbackCookie);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to unregister message callback.");

			infoQueue->Release();
		#endif

		device->Release();
	}

	AcquireNextImageStatus DX12::Device::acquireNextImage(ref<Trident::Swapchain> swapchain, ref<Semaphore> semaphore, uint32& imageIndex) const {
		imageIndex = std::static_pointer_cast<DX12::Swapchain>(swapchain)->swapChain->GetCurrentBackBufferIndex();

		return AcquireNextImageStatus::SUCCESS;
	}

	sequence<ref<DescriptorSet>> DX12::Device::allocateDescriptorSets(view<ref<DescriptorSetLayout>> descriptorSetLayouts) {
		sequence<ref<Trident::DescriptorSet>> descriptorSets(descriptorSetLayouts.count());
		UINT descriptorSetIndex = 0;

		while (descriptorSetIndex < descriptorSetLayouts.count()) {
			const ref<DescriptorSetLayout> descriptorSetLayout = descriptorSetLayouts[descriptorSetIndex];

			ref<DX12::DescriptorSet> descriptorSet = createRef<DX12::DescriptorSet>();
			descriptorSet->bindings = sequence<DX12::DescriptorSet::DescriptorSetBinding>(descriptorSetLayout->bindings.count());
			descriptorSet->viewDescriptorHandleOffset = static_cast<UINT64>(memoryAllocator->getCurrentViewDescriptorHeapAllocation().descriptorOffset);

			UINT bindingIndex = 0;

			while (bindingIndex < descriptorSetLayout->bindings.count()) {
				const DescriptorSetLayoutBinding& binding = descriptorSetLayout->bindings[bindingIndex];

				DX12::DescriptorSet::DescriptorSetBinding& descriptorSetBinding = descriptorSet->bindings[bindingIndex];
				descriptorSetBinding.descriptorType = binding.descriptorType;
				descriptorSetBinding.descriptorCount = binding.descriptorCount;
				descriptorSetBinding.viewDescriptorHandleOffset = memoryAllocator->allocateView(binding.descriptorCount);

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

	ref<Trident::Queue> DX12::Device::getQueue(QueueFamilyCapability queueFamilyCapability) const {
		return queues[static_cast<uint32>(queueFamilyCapability)];
	}

	void DX12::Device::resetFence(ref<Trident::Fence> fence) const {
		const HRESULT result = std::static_pointer_cast<DX12::Fence>(fence)->fence->Signal(0);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to reset fence.");
	}

	void DX12::Device::updateDescriptorSets(view<WriteDescriptorSet> writeDescriptorSets) const {
		DEBUG_DESCRIPTOR_GUARD(writeDescriptorSets);

		for (const WriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
			const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(writeDescriptorSet.descriptorSet);
			const DX12::DescriptorSet::DescriptorSetBinding& binding = descriptorSet->bindings[writeDescriptorSet.bindingIndex];

			switch (binding.descriptorType) {
				case DescriptorType::COMBINED_IMAGE_SAMPLER: {
					D3D12_CPU_DESCRIPTOR_HANDLE viewDescriptorHandle = binding.viewDescriptorHandleOffset;
					UINT descriptorImageIndex = 0;

					while (descriptorImageIndex < writeDescriptorSet.descriptorImages.count()) {
						const DescriptorImage& descriptorImage = writeDescriptorSet.descriptorImages[descriptorImageIndex];
						const ref<DX12::Sampler> sampler = std::static_pointer_cast<DX12::Sampler>(descriptorImage.sampler);

						memoryAllocator->writeImage(viewDescriptorHandle, descriptorImage);
						memoryAllocator->allocateSampler(&sampler->samplerDescriptor);

						viewDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
						descriptorImageIndex += 1;
					}

					break;
				}
				case DescriptorType::STORAGE_IMAGE: {
					D3D12_CPU_DESCRIPTOR_HANDLE viewDescriptorHandle = binding.viewDescriptorHandleOffset;
					UINT descriptorImageIndex = 0;

					while (descriptorImageIndex < writeDescriptorSet.descriptorImages.count()) {
						const DescriptorImage& descriptorImage = writeDescriptorSet.descriptorImages[descriptorImageIndex];

						memoryAllocator->writeStorageImage(viewDescriptorHandle, descriptorImage);

						viewDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
						descriptorImageIndex += 1;
					}

					break;
				}
				case DescriptorType::UNIFORM_BUFFER: {
					D3D12_CPU_DESCRIPTOR_HANDLE viewDescriptorHandle = binding.viewDescriptorHandleOffset;
					UINT descriptorBufferIndex = 0;

					while (descriptorBufferIndex < writeDescriptorSet.descriptorBuffers.count()) {
						const DescriptorBuffer& descriptorBuffer = writeDescriptorSet.descriptorBuffers[descriptorBufferIndex];

						memoryAllocator->writeUniformBuffer(viewDescriptorHandle, descriptorBuffer);

						viewDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
						descriptorBufferIndex += 1;
					}

					break;
				}
				case DescriptorType::READ_ONLY_STORAGE_BUFFER: {
					D3D12_CPU_DESCRIPTOR_HANDLE viewDescriptorHandle = binding.viewDescriptorHandleOffset;
					UINT descriptorBufferIndex = 0;

					while (descriptorBufferIndex < writeDescriptorSet.descriptorBuffers.count()) {
						const DescriptorBuffer& descriptorBuffer = writeDescriptorSet.descriptorBuffers[descriptorBufferIndex];

						memoryAllocator->writeReadOnlyStorageBuffer(viewDescriptorHandle, descriptorBuffer);

						viewDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
						descriptorBufferIndex += 1;
					}

					break;
				}
				case DescriptorType::READ_WRITE_STORAGE_BUFFER: {
					D3D12_CPU_DESCRIPTOR_HANDLE viewDescriptorHandle = binding.viewDescriptorHandleOffset;
					UINT descriptorBufferIndex = 0;

					while (descriptorBufferIndex < writeDescriptorSet.descriptorBuffers.count()) {
						const DescriptorBuffer& descriptorBuffer = writeDescriptorSet.descriptorBuffers[descriptorBufferIndex];

						memoryAllocator->writeReadWriteStorageBuffer(viewDescriptorHandle, descriptorBuffer);

						viewDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
						descriptorBufferIndex += 1;
					}

					break;
				}
				case DescriptorType::INPUT_ATTACHMENT: {
					D3D12_CPU_DESCRIPTOR_HANDLE viewDescriptorHandle = binding.viewDescriptorHandleOffset;
					UINT descriptorImageIndex = 0;

					while (descriptorImageIndex < writeDescriptorSet.descriptorImages.count()) {
						const DescriptorImage& descriptorImage = writeDescriptorSet.descriptorImages[descriptorImageIndex];

						memoryAllocator->writeInputAttachment(viewDescriptorHandle, descriptorImage);

						viewDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
						descriptorImageIndex += 1;
					}

					break;
				}
				default: {
					ADAPTER_ASSERT(false, "Unsupported descriptor type.");

					break;
				}
			}
		}
	}

	void DX12::Device::waitForFence(ref<Trident::Fence> fence) const {
		HRESULT result;

		result = std::static_pointer_cast<DX12::Fence>(fence)->fence->SetEventOnCompletion(1, waitEvent);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set event on fence completion.");

		const DWORD code = WaitForSingleObject(waitEvent, INFINITE);

		WIN32_ASSERT(code == WAIT_OBJECT_0);
	}

	void DX12::Device::waitIdle() const {
		for (const ref<Trident::Queue> queue : queues) {
			queue->waitIdle();
		}
	}

	void DX12::Device::setMemoryAllocator(ref<DX12::MemoryAllocator> memoryAllocator) {
		this->memoryAllocator = memoryAllocator;
	}
}