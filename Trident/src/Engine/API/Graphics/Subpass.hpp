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
				#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
					uint32 warningCount = 0;
					uint32 errorCount = 0;
					uint32 colorAttachmentReferenceIndex = 0;
					uint32 inputAttachmentReferenceIndex = 0;
					bool anyUsedColorAttachment = false;

					if (descriptor.colorAttachmentReferences.count() == 0) {
						ENGINE_ERROR("descriptor.colorAttachmentReferences must not be empty.");

						errorCount += 1;
					}

					while (colorAttachmentReferenceIndex < descriptor.colorAttachmentReferences.count()) {
						const AttachmentReference& colorAttachmentReference = descriptor.colorAttachmentReferences[colorAttachmentReferenceIndex];

						if (colorAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
							anyUsedColorAttachment = true;

							if (colorAttachmentReference.layout >= ImageLayout::count) {
								ENGINE_ERROR("descriptor.colorAttachmentReferences[[]].layout must be a valid ImageLayout value.", colorAttachmentReferenceIndex);

								errorCount += 1;
							}
						}

						colorAttachmentReferenceIndex += 1;
					}

					if (!anyUsedColorAttachment) {
						ENGINE_ERROR("descriptor.colorAttachmentReferences must contain at least one used attachment.");

						errorCount += 1;
					}

					if (descriptor.depthStencilAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX && descriptor.depthStencilAttachmentReference.layout >= ImageLayout::count) {
						ENGINE_ERROR("descriptor.depthStencilAttachmentReference.layout must be a valid ImageLayout value.");

						errorCount += 1;
					}

					while (inputAttachmentReferenceIndex < descriptor.inputAttachmentReferences.count()) {
						const AttachmentReference& inputAttachmentReference = descriptor.inputAttachmentReferences[inputAttachmentReferenceIndex];

						if (inputAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
							anyUsedColorAttachment = true;

							if (inputAttachmentReference.layout >= ImageLayout::count) {
								ENGINE_ERROR("descriptor.inputAttachmentReferences[[]].layout must be a valid ImageLayout value.", inputAttachmentReferenceIndex);

								errorCount += 1;
							}
						}

						inputAttachmentReferenceIndex += 1;
					}

					ENGINE_ASSERT(warningCount == 0 && errorCount == 0, "Invalid argument(s) ([] warnings and [] errors).", warningCount, errorCount);
				#endif

				for (const AttachmentReference& colorAttachmentReference : colorAttachmentReferences) {
					if (colorAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
						resolveAttachmentReferenceCount += 1;
					}
				}
			}

			inline uint32 getColorAttachmentReferenceCount() const {
				return static_cast<uint32>(colorAttachmentReferences.count());
			}

			inline const sequence<AttachmentReference>& getColorAttachmentReferences() const {
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

			inline const sequence<AttachmentReference>& getInputAttachmentReferences() const {
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