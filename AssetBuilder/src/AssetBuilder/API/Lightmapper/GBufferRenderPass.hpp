// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<RenderPass> Lightmapper::createGBufferRenderPass() const {
		return Renderer::createRenderPass({
			.attachments = {
				// G-Buffer position
				{
					.format = ImageFormat::R32G32B32A32_SFLOAT,
					.sampleCount = 1,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::GENERAL,
				},

				// TODO(asset-builder/lightmapper): Smooth position.

				// G-Buffer triangle normal
				{
					.format = ImageFormat::R32G32B32A32_SFLOAT,
					.sampleCount = 1,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::GENERAL,
				},

				// G-Buffer vertex normal
				{
					.format = ImageFormat::R32G32B32A32_SFLOAT,
					.sampleCount = 1,
					.loadOperation = LoadOperation::CLEAR,
					.storeOperation = StoreOperation::STORE,
					.finalLayout = ImageLayout::GENERAL,
				},
			},
			.subpasses = {
				{
					.colorAttachmentReferences = {
						{
							.index = 0,
							.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
						{
							.index = 1,
							.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
						{
							.index = 2,
							.layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
						},
					},
				},
			},
		});
	}
}