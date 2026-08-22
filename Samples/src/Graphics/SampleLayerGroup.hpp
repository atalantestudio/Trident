// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Window/LayerGroup/RenderPassLayerGroup.hpp"

namespace Trident {
	class SampleLayerGroup : public RenderPassLayerGroup {
		public:
			explicit SampleLayerGroup(range<uint32, uint32> layerRange) :
				RenderPassLayerGroup(layerRange)
			{}

			RenderPassDescriptor getRenderPassDescriptor() const override {
				return {
					.attachments = {
						{
							.format = Renderer::getSurface()->getFormat(),
							.sampleCount = 1,
							.loadOperation = LoadOperation::CLEAR,
							.storeOperation = StoreOperation::STORE,
							.finalLayout = ImageLayout::PRESENT_SRC,
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
						},
					},
				};
			}

			sequence<ClearValue> getClearValues() const override {
				return {
					{0.0f, 0.0f, 0.0f, 1.0f},
				};
			}
	};
}