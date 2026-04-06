// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Descriptor/AttachmentImageDescriptor.hpp"
#include "Engine/API/Graphics/Descriptor/SubpassDescriptor.hpp"
#include "Engine/API/Graphics/Image.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	struct Subpass {
		public:
			explicit Subpass() = default;

			explicit Subpass(const SubpassDescriptor& descriptor) :
				colorAttachmentReferences(descriptor.colorAttachmentReferences),
				depthStencilAttachmentReference(descriptor.depthStencilAttachmentReference),
				inputAttachmentReferences(descriptor.inputAttachmentReferences)
			{
				DEBUG_DESCRIPTOR_GUARD(descriptor);

				for (const AttachmentReference& colorAttachmentReference : colorAttachmentReferences) {
					if (colorAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
						resolveAttachmentReferenceCount += 1;
					}
				}
			}

			uint32 getColorAttachmentReferenceCount() const {
				return static_cast<uint32>(colorAttachmentReferences.count());
			}

			view<AttachmentReference> getColorAttachmentReferences() const {
				return colorAttachmentReferences;
			}

			bool hasDepthStencilAttachmentReference() const {
				return depthStencilAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX;
			}

			AttachmentReference getDepthStencilAttachmentReference() const {
				return depthStencilAttachmentReference;
			}

			uint32 getInputAttachmentReferenceCount() const {
				return static_cast<uint32>(inputAttachmentReferences.count());
			}

			view<AttachmentReference> getInputAttachmentReferences() const {
				return inputAttachmentReferences;
			}

			uint32 getResolveAttachmentReferenceCount() const {
				return resolveAttachmentReferenceCount;
			}

			uint8 getRasterizationSampleCount() const {
				return rasterizationSampleCount;
			}

			void setRasterizationSampleCount(uint8 rasterizationSampleCount) {
				this->rasterizationSampleCount = rasterizationSampleCount;
			}

		private:
			sequence<AttachmentReference> colorAttachmentReferences;
			AttachmentReference depthStencilAttachmentReference;
			sequence<AttachmentReference> inputAttachmentReferences;
			uint32 resolveAttachmentReferenceCount = 0;
			uint8 rasterizationSampleCount : 7;
	};
}