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
#include "Engine/API/Graphics/DescriptorSet.hpp"

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

				for (const ref<Trident::DescriptorSet>& descriptorSet : descriptorSets) {
					const ref<GL::DescriptorSet> glDescriptorSet = std::static_pointer_cast<GL::DescriptorSet>(descriptorSet);

					uint32 bindingIndex = 0;

					while (bindingIndex < glDescriptorSet->bindings.count()) {
						const GL::DescriptorSet::DescriptorSetBinding& descriptorSetBinding = glDescriptorSet->bindings[bindingIndex];

						if (isBufferDescriptorType(descriptorSetBinding.descriptorType)) {
							#if OPENGL_VERSION < OPENGL_VERSION_4_3
								ADAPTER_ASSERT(descriptorType != DescriptorType::READ_ONLY_STORAGE_BUFFER && descriptorType != DescriptorType::READ_WRITE_STORAGE_BUFFER, "Storage buffers are not supported before OpenGL 4.3.");
							#endif

							const GLenum target = getBufferDescriptorType(descriptorSetBinding.descriptorType);

							uint32 descriptorBufferIndex = 0;

							while (descriptorBufferIndex < descriptorSetBinding.descriptorBuffers.count()) {
								const DescriptorBuffer& descriptorBuffer = descriptorSetBinding.descriptorBuffers[descriptorBufferIndex];
								const GLuint index = descriptorSetLayoutOffset + bindingIndex + descriptorBufferIndex;

								#if OPENGL_VERSION >= OPENGL_VERSION_3_0
									glBindBufferRange(target, index, std::static_pointer_cast<GL::BaseBuffer>(descriptorBuffer.buffer)->buffer, descriptorBuffer.offset, descriptorBuffer.size);
								#else
									ENGINE_ERROR_UNSUPPORTED();
								#endif

								descriptorBufferIndex += 1;
							}
						}
						else if (descriptorSetBinding.descriptorType == DescriptorType::READ_ONLY_STORAGE_IMAGE || descriptorSetBinding.descriptorType == DescriptorType::READ_WRITE_STORAGE_IMAGE) {
							GLuint descriptorImageIndex = 0;

							while (descriptorImageIndex < descriptorSetBinding.descriptorImages.count()) {
								const DescriptorImage& descriptorImage = descriptorSetBinding.descriptorImages[descriptorImageIndex];
								const ref<GL::ImageView> imageView = std::static_pointer_cast<GL::ImageView>(descriptorImage.imageView);
								const GLuint unit = descriptorSetLayoutOffset + bindingIndex + descriptorImageIndex;

								// TODO: Use correct access.
								constexpr GLenum access = GL_READ_WRITE;

								GLenum format = static_cast<const GL::Image*>(imageView->getImage())->internalFormat;

								if (format == GL_SRGB8_ALPHA8) {
									format = GL_RGBA8;
								}

								#if OPENGL_VERSION >= OPENGL_VERSION_4_2
									glBindImageTexture(unit, imageView->texture, imageView->getMipLevelOffset(), GL_FALSE, imageView->getLayerOffset(), access, format);
								#else
									ENGINE_ERROR_UNSUPPORTED();
								#endif

								descriptorImageIndex += 1;
							}
						}
						else if (isImageDescriptorType(descriptorSetBinding.descriptorType)) {
							GLuint descriptorImageIndex = 0;

							while (descriptorImageIndex < descriptorSetBinding.descriptorImages.count()) {
								const DescriptorImage& descriptorImage = descriptorSetBinding.descriptorImages[descriptorImageIndex];
								const ref<GL::ImageView> imageView = std::static_pointer_cast<GL::ImageView>(descriptorImage.imageView);

								#if OPENGL_VERSION >= OPENGL_VERSION_4_5
									glBindTextureUnit(descriptorSetLayoutOffset + bindingIndex + descriptorImageIndex, imageView->texture);
								#else
									glActiveTexture(GL_TEXTURE0 + descriptorSetLayoutOffset + bindingIndex + descriptorImageIndex);
									glBindTexture(static_cast<const GL::Image*>(imageView->getImage())->target, imageView->texture);
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

								descriptorImageIndex += 1;
							}
						}
						else {
							ADAPTER_ASSERT(false, "The descriptor must either have a buffer type or an image type.");
						}

						bindingIndex += 1;
					}

					descriptorSetLayoutOffset += static_cast<uint32>(glDescriptorSet->bindings.count());
				}
			}

		private:
			sequence<ref<Trident::DescriptorSet>> descriptorSets;
	};
}