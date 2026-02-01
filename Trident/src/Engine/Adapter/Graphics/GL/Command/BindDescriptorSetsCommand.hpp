// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Buffer.hpp"
#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"
#include "Engine/Adapter/Graphics/GL/DescriptorSet.hpp"
#include "Engine/Adapter/Graphics/GL/DescriptorType.hpp"
#include "Engine/Adapter/Graphics/GL/Image.hpp"
#include "Engine/Adapter/Graphics/GL/ImageView.hpp"
#include "Engine/Adapter/Graphics/GL/Sampler.hpp"
#include "Engine/API/Rendering/DescriptorSet.hpp"

namespace Trident {
	class GL::BindDescriptorSetsCommand final : public GL::Command {
		public:
			explicit BindDescriptorSetsCommand(view<ref<Trident::DescriptorSet>> descriptorSets) :
				descriptorSets(descriptorSets.count())
			{
				std::copy(descriptorSets.begin(), descriptorSets.end(), &this->descriptorSets[0]);
			}

			void execute() const final override {
				uint32 descriptorSetLayoutOffset = 0;

				for (const ref<Trident::DescriptorSet> descriptorSet : descriptorSets) {
					const ref<GL::DescriptorSet> glDescriptorSet = std::static_pointer_cast<GL::DescriptorSet>(descriptorSet);

					for (uint32 bindingIndex = 0; bindingIndex < glDescriptorSet->bindings.count(); bindingIndex += 1) {
						const GL::DescriptorSet::DescriptorSetBinding& descriptorSetBinding = glDescriptorSet->bindings[bindingIndex];

						if (isBufferDescriptorType(descriptorSetBinding.descriptorType)) {
							#if OPENGL_VERSION < OPENGL_VERSION_4_3
								ADAPTER_ASSERT(descriptorType != DescriptorType::READ_ONLY_STORAGE_BUFFER && descriptorType != DescriptorType::READ_WRITE_STORAGE_BUFFER, "Storage buffers are not supported before OpenGL 4.3.");
							#endif

							const GLenum target = getBufferDescriptorType(descriptorSetBinding.descriptorType);

							for (uint32 descriptorBufferIndex = 0; descriptorBufferIndex < descriptorSetBinding.descriptorBuffers.count(); descriptorBufferIndex += 1) {
								const DescriptorBuffer& descriptorBuffer = descriptorSetBinding.descriptorBuffers[descriptorBufferIndex];

								#if OPENGL_VERSION >= OPENGL_VERSION_3_0
									glBindBufferRange(
										target,
										descriptorSetLayoutOffset + bindingIndex + descriptorBufferIndex,
										std::static_pointer_cast<GL::Buffer>(descriptorBuffer.buffer)->buffer,
										descriptorBuffer.offset,
										descriptorBuffer.size
									);
								#else
									ENGINE_ERROR_UNSUPPORTED();
								#endif
							}
						}
						else if (isImageDescriptorType(descriptorSetBinding.descriptorType)) {
							for (GLuint descriptorImageIndex = 0; descriptorImageIndex < descriptorSetBinding.descriptorImages.count(); descriptorImageIndex += 1) {
								const DescriptorImage& descriptorImage = descriptorSetBinding.descriptorImages[descriptorImageIndex];
								const ref<GL::ImageView> imageView = std::static_pointer_cast<GL::ImageView>(descriptorImage.imageView);
								const GL::Image* image = static_cast<const GL::Image*>(imageView->getImage());

								#if OPENGL_VERSION >= OPENGL_VERSION_4_5
									glBindTextureUnit(descriptorSetLayoutOffset + bindingIndex + descriptorImageIndex, imageView->texture);
								#else
									glActiveTexture(GL_TEXTURE0 + descriptorSetLayoutOffset + bindingIndex + descriptorImageIndex);
									glBindTexture(image->target, imageView->texture);
								#endif

								if (descriptorImage.sampler != nullptr) {
									#if OPENGL_VERSION >= OPENGL_VERSION_3_3
										glBindSampler(
											descriptorSetLayoutOffset + bindingIndex + descriptorImageIndex,
											std::static_pointer_cast<GL::Sampler>(descriptorImage.sampler)->sampler
										);
									#else
										ENGINE_ERROR_UNSUPPORTED();
									#endif
								}

								if (isFixedOrFloatingPointColorFormat(image->getFormat())) {
									#if OPENGL_VERSION >= OPENGL_VERSION_4_2
										glBindImageTexture(
											descriptorSetLayoutOffset + bindingIndex + descriptorImageIndex,
											imageView->texture,
											0,
											GL_FALSE,
											0,

											// TODO: Use correct access.
											GL_READ_WRITE,

											// TODO: Remove condition.
											image->internalFormat == GL_SRGB8_ALPHA8 ? GL_RGBA8 : image->internalFormat
										);
									#else
										ENGINE_ERROR_UNSUPPORTED();
									#endif
								}
							}
						}
						else {
							ADAPTER_ASSERT(false, "The descriptor must either have a buffer type or an image type.");
						}
					}

					descriptorSetLayoutOffset += static_cast<uint32>(glDescriptorSet->bindings.count());
				}
			}

		private:
			sequence<ref<Trident::DescriptorSet>> descriptorSets;
	};
}