#pragma once

#include "Engine/API/Rendering/Attachment.hpp"

namespace ProjectA {
	struct Subpass {
		sequence<ColorAttachment> colorAttachments;
		DepthStencilAttachment depthStencilAttachment;

		// TODO: Determine `resolve` with the sample count of all color attachments.
		bool resolve;

		bool depthStencil;

		// Sets the index of the attachment that will render to a swapchain image.
		uint32 presentAttachmentIndex = std::numeric_limits<uint32>::max();

		public:
			Subpass() :
				combinedSampleCounts(1)
			{
				for (const ColorAttachment& colorAttachment : colorAttachments) {
					combinedSampleCounts |= colorAttachment.sampleCount;
				}
			}

			uint8 getCombinedSampleCounts() const {
				return combinedSampleCounts;
			}

		private:
			uint8 combinedSampleCounts : 7;
	};

	/* class SubpassOld {
		public:
			Subpass(const SubpassDescriptor& subpassDescriptor, const ref<RenderPass>& renderPass, uint32 index) :
				index(index),
				renderPass(renderPass),
				depthStencilAttachment(subpassDescriptor.depthStencil)
			{
				// TODO

				for (const ColorAttachment& colorAttachment : subpassDescriptor.colorAttachments) {
					sampleCounts |= colorAttachment.sampleCount;
				}
			}

			const ref<RenderPass>& getRenderPass() const {
				return renderPass;
			}

			uint32 getIndex() const {
				return index;
			}

			bool hasDepthStencilAttachment() const {
				return depthStencilAttachment;
			}

			uint32 getSampleCounts() const {
				return sampleCounts;
			}

		private:
			const ref<RenderPass>& renderPass;
			const uint32 index;
			const bool depthStencilAttachment;
			uint32 sampleCounts = 1;
	}; */
}