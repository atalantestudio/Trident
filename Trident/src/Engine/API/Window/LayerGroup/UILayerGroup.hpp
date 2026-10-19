// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/API/Window/LayerGroup/RenderPassLayerGroup.hpp"

namespace Trident {
	class UILayerGroup : public RenderPassLayerGroup {
		public:
			explicit UILayerGroup(range<uint32, uint32> layerRange) :
				RenderPassLayerGroup(layerRange)
			{}

			RenderPassDescriptor getRenderPassDescriptor() const override {
				return {
					.attachments = {
						{
							.format = Renderer::getSurface()->getFormat(),
							.sampleCount = ImageSampleCount::x1,
							.loadOperation = LoadOperation::CLEAR,
							.storeOperation = StoreOperation::STORE,
							.finalLayout = ImageLayout::PRESENT_SRC,
						},
						{
							.format = ImageFormat::D32_SFLOAT,
							.sampleCount = ImageSampleCount::x1,
							.loadOperation = LoadOperation::CLEAR,
							.storeOperation = StoreOperation::UNDEFINED,
							.finalLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
						},
					},
					.subpasses = {
						{
							.colorAttachmentReferences = {
								{
									.index = 0,
									.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
								},
							},
							.depthStencilAttachmentReference = {
								.index = 1,
								.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
							},
						},
					},
				};
			}

			sequence<ClearValue> getClearValues() const override {
				return {
					{0.0f, 0.0f, 0.0f, 0.0f},
					{
						.depth = 0.0f,
						.stencil = 0,
					},
				};
			}
	};
}