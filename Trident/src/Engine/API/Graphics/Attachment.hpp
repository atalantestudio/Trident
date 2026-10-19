// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ImageFormat.hpp"
#include "Engine/API/Graphics/ImageLayout.hpp"
#include "Engine/API/Graphics/ImageSampleCount.hpp"
#include "Engine/API/Graphics/LoadOperation.hpp"
#include "Engine/API/Graphics/StoreOperation.hpp"

namespace Trident {
	struct Attachment {
		/// The format of the image view that will be used for the attachment.
		ImageFormat format;

		/// The number of samples of the image.
		ImageSampleCount sampleCount;

		/// Specifies how the attachment's contents are treated at the beginning of the subpass where it is first used.
		LoadOperation loadOperation;

		/// Specifies how the attachment's contents are treated at the end of the subpass where it is last used.
		StoreOperation storeOperation;

		// TODO(graphics/dx12/render-pass)
		// In Vulkan the DEPTH_STENCIL_READ_ONLY_OPTIMAL layout can be used for depth stencil attachment reads and SRV reads.
		// It seems that in DX12 it is not compatible with SRV reads and in this case a specific SRV access must be used.
		/// The layout the attachment image will be transitioned to when the render pass ends.
		ImageLayout finalLayout;
	};
}