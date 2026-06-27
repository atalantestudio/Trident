// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/DX12/HeapType.hpp"
#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/Adapter/Graphics/DX12/ImageView.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceDimension.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceFlags.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Graphics/Descriptor/ImageDescriptor.hpp"

namespace Trident {
	static ImageDescriptor createSwapchainImageDescriptor(Extent2D extent, UINT index) {
		ImageDescriptor imageDescriptor{};
		imageDescriptor.extent.width = extent.width;
		imageDescriptor.extent.height = extent.height;
		imageDescriptor.extent.depth = 1;
		imageDescriptor.mipLevelCount = 1;
		imageDescriptor.layerCount = 1;

		// These properties aren't used when creating a swapchain image.
		imageDescriptor.type = ImageType::TEXTURE_2D;
		imageDescriptor.format = ImageFormat::B8G8R8A8_UNORM;
		imageDescriptor.sampleCount = 1;
		imageDescriptor.usage = static_cast<ImageUsage>(0);
		imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;

		return imageDescriptor;
	}

	DX12::Image::Image(const Trident::Device* device, const ImageDescriptor& descriptor) :
		#if defined(DEBUG_OBJECT_NAME)
			name(descriptor.name),
		#endif
		Trident::Image(device, descriptor)
	{}

	ref<Trident::Image> DX12::createImage(const Trident::Device* device, const ImageDescriptor& descriptor) {
		return createRef<DX12::Image>(device, descriptor);
	}

	ref<Trident::Image> DX12::createImage(const Trident::Device* device, const DX12::Swapchain* swapchain, UINT index) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Swapchain* _swapchain = static_cast<const DX12::Swapchain*>(swapchain);
		HRESULT result;

		ImageDescriptor descriptor = createSwapchainImageDescriptor(_swapchain->getExtent(), index);

		#if defined(DEBUG_OBJECT_NAME)
			descriptor.name = Logger::format("Swapchain image []", index);
		#endif

		ref<DX12::Image> image = createRef<DX12::Image>(device, descriptor);
		image->usage = static_cast<ImageUsage>(0);

		result = _swapchain->swapChain->GetBuffer(index, IID_PPV_ARGS(&image->resource));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to get buffer for render target view.");

		#if defined(DEBUG_OBJECT_NAME)
			const sequence<char8> descriptorName = Logger::format("Swapchain image []", index);
			const LPWSTR name = ConvertUTF8ToUTF16(&descriptorName[0]);

			result = image->resource->SetName(name);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set name on image resource.");

			free(name);
		#endif

		return image;
	}

	void DX12::destroyImage(const Trident::Device* device, const Trident::Image* image) {
		const DX12::Image* _image = static_cast<const DX12::Image*>(image);

		_image->resource->Release();
	}
}