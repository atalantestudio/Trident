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

			inline uint32 getColorAttachmentReferenceCount() const {
				return static_cast<uint32>(colorAttachmentReferences.count());
			}

			inline view<AttachmentReference> getColorAttachmentReferences() const {
				return colorAttachmentReferences;
			}

			inline bool hasDepthStencilAttachmentReference() const {
				return depthStencilAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX;
			}

			inline AttachmentReference getDepthStencilAttachmentReference() const {
				return depthStencilAttachmentReference;
			}

			inline uint32 getInputAttachmentReferenceCount() const {
				return static_cast<uint32>(inputAttachmentReferences.count());
			}

			inline view<AttachmentReference> getInputAttachmentReferences() const {
				return inputAttachmentReferences;
			}

			inline uint32 getResolveAttachmentReferenceCount() const {
				return resolveAttachmentReferenceCount;
			}

			inline uint8 getRasterizationSampleCount() const {
				return rasterizationSampleCount;
			}

			inline void setRasterizationSampleCount(uint8 rasterizationSampleCount) {
				this->rasterizationSampleCount = rasterizationSampleCount;
			}

		private:
			sequence<AttachmentReference> colorAttachmentReferences;
			AttachmentReference depthStencilAttachmentReference;
			sequence<AttachmentReference> inputAttachmentReferences;
			uint32 resolveAttachmentReferenceCount = 0;
			uint8 rasterizationSampleCount;
	};
}