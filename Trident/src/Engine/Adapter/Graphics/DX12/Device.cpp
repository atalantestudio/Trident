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
	#if defined(DEBUG_ENABLE_MESSENGER)
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

	DX12::Device::Device(const Trident::GraphicsAdapter* adapter, const DX12::PhysicalDevice* physicalDevice) :
		Trident::Device(adapter),
		physicalDevice(physicalDevice)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result = -1;

		#ifndef PREFER_SOFTWARE_ADAPTER
			result = D3D12CreateDevice(physicalDevice->adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
		#endif

		if (FAILED(result)) {
			// TODO: Restore.
			/* #ifndef PREFER_SOFTWARE_ADAPTER
				ADAPTER_WARNING("Failed to create device using the primary display adapter. Falling back to a WARP adapter.");
			#endif

			IDXGIAdapter* adapter;

			result = instance.factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to enumerate WARP adapter.");

			result = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)); */

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create device using the WARP adapter.");

			// adapter->Release();
		}

		queues[0] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_DIRECT);
		queues[1] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		queues[2] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_COPY);

		waitEvent = CreateEvent(nullptr, false, false, L"Wait event");

		WIN32_ASSERT(waitEvent != nullptr);

		#if defined(DEBUG_ENABLE_MESSENGER)
			result = device->QueryInterface(IID_PPV_ARGS(&infoQueue));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to query info queue interface.");

			static constexpr UINT DENY_MESSAGE_ID_COUNT = 2;
			static D3D12_MESSAGE_ID DENY_MESSAGE_IDS[DENY_MESSAGE_ID_COUNT] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
			};

			D3D12_INFO_QUEUE_FILTER infoQueueFilter{};
			infoQueueFilter.AllowList.NumCategories = 0;
			infoQueueFilter.AllowList.pCategoryList = nullptr;
			infoQueueFilter.AllowList.NumSeverities = 0;
			infoQueueFilter.AllowList.pSeverityList = nullptr;
			infoQueueFilter.AllowList.NumIDs = 0;
			infoQueueFilter.AllowList.pIDList = nullptr;
			infoQueueFilter.DenyList.NumCategories = 0;
			infoQueueFilter.DenyList.pCategoryList = nullptr;
			infoQueueFilter.DenyList.NumSeverities = 0;
			infoQueueFilter.DenyList.pSeverityList = nullptr;
			infoQueueFilter.DenyList.NumIDs = DENY_MESSAGE_ID_COUNT;
			infoQueueFilter.DenyList.pIDList = DENY_MESSAGE_IDS;

			result = infoQueue->AddStorageFilterEntries(&infoQueueFilter);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to add storage filter entries.");

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

		#if defined(DEBUG_ENABLE_MESSENGER)
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
			descriptorSet->samplerDescriptorHandleOffset = static_cast<UINT64>(memoryAllocator->getCurrentSamplerDescriptorHeapAllocation().descriptorOffset);

			UINT bindingIndex = 0;

			while (bindingIndex < descriptorSetLayout->bindings.count()) {
				const DescriptorSetLayoutBinding& binding = descriptorSetLayout->bindings[bindingIndex];

				DX12::DescriptorSet::DescriptorSetBinding& descriptorSetBinding = descriptorSet->bindings[bindingIndex];
				descriptorSetBinding.descriptorType = binding.descriptorType;
				descriptorSetBinding.descriptorCount = binding.descriptorCount;
				descriptorSetBinding.viewDescriptorHandleOffset = memoryAllocator->allocateView(binding.descriptorCount);

				if (isSamplerDescriptorType(binding.descriptorType)) {
					// TODO: The binding might not define the samplers.
					descriptorSetBinding.samplerDescriptorHandleOffset = memoryAllocator->allocateSampler(binding.descriptorCount);
				}

				bindingIndex += 1;
			}

			descriptorSets[descriptorSetIndex] = descriptorSet;
			descriptorSetIndex += 1;
		}

		return descriptorSets;
	}

	ref<PhysicalDevice> DX12::Device::getPhysicalDevice() const {
		return createRef<DX12::PhysicalDevice>(*physicalDevice);
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

			D3D12_CPU_DESCRIPTOR_HANDLE viewDescriptorHandle = binding.viewDescriptorHandleOffset;
			D3D12_CPU_DESCRIPTOR_HANDLE samplerDescriptorHandle = binding.samplerDescriptorHandleOffset;

			switch (binding.descriptorType) {
				case DescriptorType::COMBINED_IMAGE_SAMPLER: {
					for (const DescriptorImage& descriptorImage : writeDescriptorSet.descriptorImages) {
						const ref<DX12::Sampler> sampler = std::static_pointer_cast<DX12::Sampler>(descriptorImage.sampler);

						memoryAllocator->writeImage(viewDescriptorHandle, descriptorImage);
						memoryAllocator->writeSampler(samplerDescriptorHandle, &sampler->samplerDescriptor);

						viewDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
						samplerDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
					}

					break;
				}
				case DescriptorType::READ_ONLY_STORAGE_IMAGE: {
					for (const DescriptorImage& descriptorImage : writeDescriptorSet.descriptorImages) {
						memoryAllocator->writeImage(viewDescriptorHandle, descriptorImage);

						viewDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
						samplerDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
					}

					break;
				}
				case DescriptorType::STORAGE_IMAGE: {
					for (const DescriptorImage& descriptorImage : writeDescriptorSet.descriptorImages) {
						memoryAllocator->writeReadWriteStorageImage(viewDescriptorHandle, descriptorImage);

						viewDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
						samplerDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
					}

					break;
				}
				case DescriptorType::UNIFORM_BUFFER: {
					for (const DescriptorBuffer& descriptorBuffer : writeDescriptorSet.descriptorBuffers) {
						memoryAllocator->writeUniformBuffer(viewDescriptorHandle, descriptorBuffer);

						viewDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
						samplerDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
					}

					break;
				}
				case DescriptorType::READ_ONLY_STORAGE_BUFFER: {
					for (const DescriptorBuffer& descriptorBuffer : writeDescriptorSet.descriptorBuffers) {
						memoryAllocator->writeReadOnlyStorageBuffer(viewDescriptorHandle, descriptorBuffer);

						viewDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
						samplerDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
					}

					break;
				}
				case DescriptorType::READ_WRITE_STORAGE_BUFFER: {
					for (const DescriptorBuffer& descriptorBuffer : writeDescriptorSet.descriptorBuffers) {
						memoryAllocator->writeReadWriteStorageBuffer(viewDescriptorHandle, descriptorBuffer);

						viewDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
						samplerDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
					}

					break;
				}
				case DescriptorType::INPUT_ATTACHMENT: {
					for (const DescriptorImage& descriptorImage : writeDescriptorSet.descriptorImages) {
						memoryAllocator->writeInputAttachment(viewDescriptorHandle, descriptorImage);

						viewDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
						samplerDescriptorHandle.ptr += memoryAllocator->viewDescriptorHandleIncrementSize;
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