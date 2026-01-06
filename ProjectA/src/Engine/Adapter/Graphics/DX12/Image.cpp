// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::Image::Image(const DX12::Device& device, const DX12::Swapchain& swapchain, uint32 imageIndex, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result;

		result = swapchain.swapChain->GetBuffer(imageIndex, IID_PPV_ARGS(&resource));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to get buffer for render target view.");

		device.device->CreateRenderTargetView(resource.Get(), nullptr, cpuDescriptorHandle);
	}

	ref<ProjectA::ImageView> DX12::Image::createViewInternal(const ProjectA::Image* image, const ImageViewDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}
}