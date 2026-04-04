// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/CommandEncoder.hpp"
#include "Engine/API/Rendering/ImageFormat.hpp"

namespace Trident {
	void CommandEncoder::copyImageToBuffer(ref<Image> sourceImage, ref<BaseBuffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions) {
		#if defined(BUILD_MODE_DEBUG)
			uint32 warningCount = 0;
			uint32 errorCount = 0;
			uint32 copyRegionIndex = 0;

			if (!(sourceImage->getUsage() & ImageUsage::TRANSFER_SRC)) {
				ENGINE_ERROR("The source image must have the TRANSFER_SRC usage flag.");

				errorCount += 1;
			}

			if (!(destinationBuffer->getUsage() & BufferUsage::TRANSFER_DST)) {
				ENGINE_ERROR("The destination buffer must have the TRANSFER_DST usage flag.");

				errorCount += 1;
			}

			while (copyRegionIndex < copyRegions.count()) {
				const BufferImageCopyRegion& copyRegion = copyRegions[copyRegionIndex];
				const uint64 copyRegionArea = copyRegion.imageExtent.width * copyRegion.imageExtent.height * copyRegion.imageExtent.depth * copyRegion.imageLayerCount * getByteSize(sourceImage->getFormat());

				if (copyRegion.bufferOffset >= destinationBuffer->getSize()) {
					ENGINE_ERROR("descriptor.copyRegions[[]].bufferOffset must be less than the size of the destination buffer.", copyRegionIndex);

					errorCount += 1;
				}

				if (copyRegionArea > destinationBuffer->getSize() - copyRegion.bufferOffset) {
					ENGINE_ERROR("The area formed by descriptor.copyRegions[[]] most be less than or equal to the size of the destination buffer.", copyRegionIndex);

					errorCount += 1;
				}

				if (copyRegion.bufferOffset >= destinationBuffer->getSize()) {
					ENGINE_ERROR("descriptor.copyRegions[[]].bufferOffset must be less than the size of the destination buffer.", copyRegionIndex);

					errorCount += 1;
				}

				if (copyRegion.imageOffsetX >= static_cast<sint32>(sourceImage->getExtent().width)) {
					ENGINE_ERROR("descriptor.copyRegions[[]].imageOffsetX must be less than the width of the source image.", copyRegionIndex);

					errorCount += 1;
				}

				if (copyRegion.imageOffsetY >= static_cast<sint32>(sourceImage->getExtent().height)) {
					ENGINE_ERROR("descriptor.copyRegions[[]].imageOffsetY must be less than the height of the source image.", copyRegionIndex);

					errorCount += 1;
				}

				if (copyRegion.imageOffsetZ >= static_cast<sint32>(sourceImage->getExtent().depth)) {
					ENGINE_ERROR("descriptor.copyRegions[[]].imageOffsetZ must be less than the depth of the source image.", copyRegionIndex);

					errorCount += 1;
				}

				if (copyRegion.imageExtent.width == 0) {
					ENGINE_ERROR("descriptor.copyRegions[[]].imageExtent.width must be greater than 0.", copyRegionIndex);

					errorCount += 1;
				}

				if (copyRegion.imageExtent.height == 0) {
					ENGINE_ERROR("descriptor.copyRegions[[]].imageExtent.height must be greater than 0.", copyRegionIndex);

					errorCount += 1;
				}

				if (copyRegion.imageExtent.depth == 0) {
					ENGINE_ERROR("descriptor.copyRegions[[]].imageExtent.depth must be greater than 0.", copyRegionIndex);

					errorCount += 1;
				}

				if (copyRegion.imageOffsetX + copyRegion.imageExtent.width > sourceImage->getExtent().width) {
					ENGINE_ERROR("The width formed by descriptor.copyRegions[[]] must be less than or equal to the width of the source image.", copyRegionIndex);

					errorCount += 1;
				}

				if (copyRegion.imageOffsetY + copyRegion.imageExtent.height > sourceImage->getExtent().height) {
					ENGINE_ERROR("The height formed by descriptor.copyRegions[[]] must be less than or equal to the height of the source image.", copyRegionIndex);

					errorCount += 1;
				}

				if (copyRegion.imageOffsetZ + copyRegion.imageExtent.depth > sourceImage->getExtent().depth) {
					ENGINE_ERROR("The depth formed by descriptor.copyRegions[[]] must be less than or equal to the depth of the source image.", copyRegionIndex);

					errorCount += 1;
				}

				if (copyRegion.imageMipLevel >= sourceImage->getMipLevelCount()) {
					ENGINE_ERROR("descriptor.copyRegions[[]].imageMipLevel must be less than the number of mip levels in the source image.", copyRegionIndex);

					errorCount += 1;
				}

				if (copyRegion.imageLayerOffset >= sourceImage->getLayerCount()) {
					ENGINE_ERROR("descriptor.copyRegions[[]].imageLayerOffset must be less than the number of layers in the source image.", copyRegionIndex);

					errorCount += 1;
				}

				if (copyRegion.imageLayerCount == 0) {
					ENGINE_ERROR("descriptor.copyRegions[[]].imageLayerCount must be greater than 0.", copyRegionIndex);

					errorCount += 1;
				}

				copyRegionIndex += 1;
			}

			ENGINE_ASSERT(warningCount == 0 && errorCount == 0, "Invalid argument(s) ([] warnings and [] errors).", warningCount, errorCount);
		#endif

		device->adapter->copyImageToBuffer(this, sourceImage, destinationBuffer, copyRegions);
	}
}