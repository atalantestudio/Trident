// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Backend/DX12/Image.hpp"

namespace ProjectA {
	DX12::Image::Image(const DX12::Device& device, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		device.device->CreateRenderTargetView(resource.Get(), nullptr, cpuDescriptorHandle);
	}

	DX12::Image::~Image() {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	ref<ProjectA::ImageView> DX12::Image::createViewInternal(const ProjectA::Image* image, const ImageViewDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}
}