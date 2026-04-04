// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/CommandEncoder.hpp"

namespace Trident {
	void CommandEncoder::insertMemoryBarriers(view<BufferMemoryBarrier> bufferMemoryBarriers, view<ImageMemoryBarrier> imageMemoryBarriers) {
		#if defined(BUILD_MODE_DEBUG)
			uint32 warningCount = 0;
			uint32 errorCount = 0;
			uint32 bufferMemoryBarrierIndex = 0;
			uint32 imageMemoryBarrierIndex = 0;

			while (bufferMemoryBarrierIndex < bufferMemoryBarriers.count()) {
				const BufferMemoryBarrier& bufferMemoryBarrier = bufferMemoryBarriers[bufferMemoryBarrierIndex];

				if (bufferMemoryBarrier.buffer == nullptr) {
					ENGINE_ERROR("bufferMemoryBarriers[[]].buffer must be a valid Buffer object.", bufferMemoryBarrierIndex);

					errorCount += 1;
				}

				bufferMemoryBarrierIndex += 1;
			}

			while (imageMemoryBarrierIndex < imageMemoryBarriers.count()) {
				const ImageMemoryBarrier& imageMemoryBarrier = imageMemoryBarriers[imageMemoryBarrierIndex];

				if (imageMemoryBarrier.image != nullptr) {
					if (imageMemoryBarrier.newLayout == imageMemoryBarrier.image->getLayout()) {
						ENGINE_WARNING("imageMemoryBarriers[[]].newLayout is the same as the current image layout.", imageMemoryBarrierIndex);

						warningCount += 1;
					}
				}
				else {
					ENGINE_ERROR("imageMemoryBarriers[[]].image must be a valid Image object.", imageMemoryBarrierIndex);

					errorCount += 1;
				}

				imageMemoryBarrierIndex += 1;
			}

			ENGINE_ASSERT(warningCount == 0 && errorCount == 0, "Invalid argument(s) ([] warnings and [] errors).", warningCount, errorCount);
		#endif

		device->adapter->insertMemoryBarriers(this, bufferMemoryBarriers, imageMemoryBarriers);
	}
}