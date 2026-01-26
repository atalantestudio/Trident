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

namespace ProjectA {
	#if defined(DEBUG_ENABLE_VALIDATION_LAYERS)
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

			ComPtr<IDXGIAdapter> adapter;

			result = instance.factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to enumerate WARP adapter.");

			result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create device using the WARP adapter.");
		}

		queues[0] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_DIRECT);
		queues[1] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		queues[2] = createRef<DX12::Queue>(*this, D3D12_COMMAND_LIST_TYPE_COPY);

		waitEvent = CreateEvent(nullptr, false, false, L"Wait event");

		WIN32_ASSERT(waitEvent != nullptr);

		#if defined(DEBUG_ENABLE_VALIDATION_LAYERS)
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

		#if defined(DEBUG_ENABLE_VALIDATION_LAYERS)
			result = infoQueue->UnregisterMessageCallback(messageCallbackCookie);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to unregister message callback.");

			infoQueue->Release();
		#endif
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
		UINT samplerDescriptorCount = 0;
		UINT viewDescriptorCount = 0;

		while (descriptorSetIndex < descriptorSetLayouts.count()) {
			const ref<DescriptorSetLayout> descriptorSetLayout = descriptorSetLayouts[descriptorSetIndex];

			bindingIndex = 0;
			samplerDescriptorCount = 0;
			viewDescriptorCount = 0;

			ref<DX12::DescriptorSet> descriptorSet = createRef<DX12::DescriptorSet>();
			descriptorSet->bindings = sequence<DX12::DescriptorSet::DescriptorSetBinding>(descriptorSetLayout->bindings.count());

			for (const DescriptorSetLayoutBinding& binding : descriptorSetLayout->bindings) {
				switch (binding.descriptorType) {
					case DescriptorType::COMBINED_IMAGE_SAMPLER: {
						viewDescriptorCount += binding.descriptorCount;
						samplerDescriptorCount += binding.descriptorCount;

						break;
					}
					case DescriptorType::STORAGE_IMAGE: {
						viewDescriptorCount += binding.descriptorCount;

						break;
					}
					case DescriptorType::UNIFORM_BUFFER: {
						viewDescriptorCount += binding.descriptorCount;

						break;
					}
					case DescriptorType::STORAGE_BUFFER: {
						viewDescriptorCount += binding.descriptorCount;

						break;
					}
					case DescriptorType::INPUT_ATTACHMENT: {
						viewDescriptorCount += binding.descriptorCount;

						break;
					}
					default: {
						ADAPTER_ASSERT(false, "Unsupported descriptor type.");

						break;
					}
				}
			}

			if (viewDescriptorCount > 0) {
				D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavDescriptorHeapDescriptor{};
				cbvSrvUavDescriptorHeapDescriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				cbvSrvUavDescriptorHeapDescriptor.NumDescriptors = viewDescriptorCount;
				cbvSrvUavDescriptorHeapDescriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				cbvSrvUavDescriptorHeapDescriptor.NodeMask = 0;

				result = device->CreateDescriptorHeap(&cbvSrvUavDescriptorHeapDescriptor, IID_PPV_ARGS(&descriptorSet->viewDescriptorHeap));

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create CBV/SRV/UAV descriptor heap.");
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

			UINT viewDescriptorOffset = 0;
			UINT samplerDescriptorOffset = 0;

			while (bindingIndex < descriptorSetLayout->bindings.count()) {
				const DescriptorSetLayoutBinding& binding = descriptorSetLayout->bindings[bindingIndex];

				DX12::DescriptorSet::DescriptorSetBinding& descriptorSetBinding = descriptorSet->bindings[bindingIndex];
				descriptorSetBinding.descriptorType = binding.descriptorType;
				descriptorSetBinding.descriptorCount = binding.descriptorCount;
				descriptorSetBinding.viewDescriptorOffset = viewDescriptorOffset;
				descriptorSetBinding.samplerDescriptorOffset = samplerDescriptorOffset;

				switch (binding.descriptorType) {
					case DescriptorType::COMBINED_IMAGE_SAMPLER: {
						viewDescriptorOffset += binding.descriptorCount;
						samplerDescriptorOffset += binding.descriptorCount;

						break;
					}
					case DescriptorType::STORAGE_IMAGE: {
						viewDescriptorOffset += binding.descriptorCount;

						break;
					}
					case DescriptorType::UNIFORM_BUFFER: {
						viewDescriptorOffset += binding.descriptorCount;

						break;
					}
					case DescriptorType::STORAGE_BUFFER: {
						viewDescriptorOffset += binding.descriptorCount;

						break;
					}
					case DescriptorType::INPUT_ATTACHMENT: {
						viewDescriptorOffset += binding.descriptorCount;

						break;
					}
					default: {
						ADAPTER_ASSERT(false, "Unsupported descriptor type.");

						break;
					}
				}

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

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to reset fence.");
	}

	void DX12::Device::updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const {
		const UINT viewDescriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		const UINT samplerDescriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		for (const WriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
			const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(writeDescriptorSet.descriptorSet);
			const DX12::DescriptorSet::DescriptorSetBinding& binding = descriptorSet->bindings[writeDescriptorSet.bindingIndex];

			switch (binding.descriptorType) {
				case DescriptorType::COMBINED_IMAGE_SAMPLER: {
					D3D12_CPU_DESCRIPTOR_HANDLE viewDescriptorHandle = descriptorSet->viewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
					viewDescriptorHandle.ptr += binding.viewDescriptorOffset * viewDescriptorHandleIncrementSize;

					D3D12_CPU_DESCRIPTOR_HANDLE samplerDescriptorHandle = descriptorSet->samplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
					samplerDescriptorHandle.ptr += binding.samplerDescriptorOffset * samplerDescriptorHandleIncrementSize;

					UINT descriptorImageIndex = 0;

					while (descriptorImageIndex < writeDescriptorSet.descriptorImages.count()) {
						const DescriptorImage& descriptorImage = writeDescriptorSet.descriptorImages[descriptorImageIndex];
						const DX12::Image* image = static_cast<const DX12::Image*>(descriptorImage.imageView->getImage());

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

						device->CreateShaderResourceView(image->resource.Get(), &shaderResourceViewDescriptor, viewDescriptorHandle);

						const ref<DX12::Sampler> sampler = std::static_pointer_cast<DX12::Sampler>(descriptorImage.sampler);

						device->CreateSampler(&sampler->samplerDescriptor, samplerDescriptorHandle);

						viewDescriptorHandle.ptr += viewDescriptorHandleIncrementSize;
						samplerDescriptorHandle.ptr += samplerDescriptorHandleIncrementSize;
						descriptorImageIndex += 1;
					}

					break;
				}
				case DescriptorType::UNIFORM_BUFFER: {
					D3D12_CPU_DESCRIPTOR_HANDLE viewDescriptorHandle = descriptorSet->viewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
					viewDescriptorHandle.ptr += binding.viewDescriptorOffset * viewDescriptorHandleIncrementSize;

					UINT descriptorBufferIndex = 0;

					while (descriptorBufferIndex < writeDescriptorSet.descriptorBuffers.count()) {
						const DescriptorBuffer& descriptorBuffer = writeDescriptorSet.descriptorBuffers[descriptorBufferIndex];
						const ref<DX12::Buffer> buffer = std::static_pointer_cast<DX12::Buffer>(descriptorBuffer.buffer);

						D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDescriptor{};
						constantBufferViewDescriptor.BufferLocation = buffer->resource->GetGPUVirtualAddress() + descriptorBuffer.offset;

						// NOTE: Constant buffer views need to be aligned to 256.
						constantBufferViewDescriptor.SizeInBytes = (static_cast<UINT>(descriptorBuffer.size) + D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);

						device->CreateConstantBufferView(&constantBufferViewDescriptor, viewDescriptorHandle);

						viewDescriptorHandle.ptr += viewDescriptorHandleIncrementSize;
						descriptorBufferIndex += 1;
					}

					break;
				}
				case DescriptorType::STORAGE_BUFFER: {
					D3D12_CPU_DESCRIPTOR_HANDLE viewDescriptorHandle = descriptorSet->viewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
					viewDescriptorHandle.ptr += binding.viewDescriptorOffset * viewDescriptorHandleIncrementSize;

					UINT descriptorBufferIndex = 0;

					while (descriptorBufferIndex < writeDescriptorSet.descriptorBuffers.count()) {
						const DescriptorBuffer& descriptorBuffer = writeDescriptorSet.descriptorBuffers[descriptorBufferIndex];
						const ref<DX12::Buffer> buffer = std::static_pointer_cast<DX12::Buffer>(descriptorBuffer.buffer);

						D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor{};
						shaderResourceViewDescriptor.Format = DXGI_FORMAT_UNKNOWN;
						shaderResourceViewDescriptor.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
						shaderResourceViewDescriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
						shaderResourceViewDescriptor.Buffer.FirstElement = 0;
						shaderResourceViewDescriptor.Buffer.NumElements = static_cast<UINT>(buffer->size);
						shaderResourceViewDescriptor.Buffer.StructureByteStride = 1;
						shaderResourceViewDescriptor.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

						device->CreateShaderResourceView(buffer->resource.Get(), &shaderResourceViewDescriptor, viewDescriptorHandle);

						viewDescriptorHandle.ptr += viewDescriptorHandleIncrementSize;
						descriptorBufferIndex += 1;
					}

					break;
				}
				case DescriptorType::INPUT_ATTACHMENT: {
					D3D12_CPU_DESCRIPTOR_HANDLE viewDescriptorHandle = descriptorSet->viewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
					viewDescriptorHandle.ptr += binding.viewDescriptorOffset * viewDescriptorHandleIncrementSize;

					UINT descriptorImageIndex = 0;

					while (descriptorImageIndex < writeDescriptorSet.descriptorImages.count()) {
						const DescriptorImage& descriptorImage = writeDescriptorSet.descriptorImages[descriptorImageIndex];
						const DX12::Image* image = static_cast<const DX12::Image*>(descriptorImage.imageView->getImage());

						ADAPTER_ASSERT(image->getType() == ImageType::TEXTURE_2D, "Images other than TEXTURE_2D are not supported.");

						D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor{};
						shaderResourceViewDescriptor.Format = getImageFormat(image->getFormat());

						if (shaderResourceViewDescriptor.Format == DXGI_FORMAT_D32_FLOAT) {
							shaderResourceViewDescriptor.Format = DXGI_FORMAT_R32_FLOAT;
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

						device->CreateShaderResourceView(image->resource.Get(), &shaderResourceViewDescriptor, viewDescriptorHandle);

						viewDescriptorHandle.ptr += viewDescriptorHandleIncrementSize;
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

	void DX12::Device::waitForFence(ref<ProjectA::Fence> fence) const {
		HRESULT result;

		result = std::static_pointer_cast<DX12::Fence>(fence)->fence->SetEventOnCompletion(1, waitEvent);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set event on fence completion.");

		const DWORD code = WaitForSingleObject(waitEvent, INFINITE);

		WIN32_ASSERT(code == WAIT_OBJECT_0);
	}

	void DX12::Device::waitIdle() const {
		for (const ref<ProjectA::Queue> queue : queues) {
			queue->waitIdle();
		}
	}
}