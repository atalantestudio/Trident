#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"
#include "Backend/OpenGL/OpenGLDescriptorSet.hpp"
#include "Backend/OpenGL/OpenGLImage.hpp"
#include "Backend/OpenGL/OpenGLImageView.hpp"
#include "Backend/OpenGL/OpenGLSampler.hpp"

#include "Backend/OpenGL/DescriptorType.hpp"

#include "Engine/API/Rendering/DescriptorSet.hpp"

namespace ProjectA {
	class OpenGLBindDescriptorSetsCommand final : public OpenGLCommand {
		public:
			explicit OpenGLBindDescriptorSetsCommand(const sequence<ref<DescriptorSet>>& descriptorSets) :
				descriptorSets(descriptorSets)
			{}

			void execute() const final override {
				uint32 descriptorSetLayoutOffset = 0;

				for (const ref<DescriptorSet>& descriptorSet : descriptorSets) {
					const ref<OpenGLDescriptorSet>& glDescriptorSet = std::static_pointer_cast<OpenGLDescriptorSet>(descriptorSet);

					for (uint32 bindingIndex = 0; bindingIndex < glDescriptorSet->bindings.count(); bindingIndex += 1) {
						const OpenGLDescriptorSet::DescriptorSetBinding& descriptorSetBinding = glDescriptorSet->bindings[bindingIndex];

						if (isBufferDescriptorType(descriptorSetBinding.descriptorType)) {
							#if OPENGL_VERSION < OPENGL_VERSION_4_3
								ENGINE_ASSERT(descriptorType != DescriptorType::STORAGE_BUFFER, "Storage buffers are not supported before OpenGL 4.3.");
							#endif

							const GLenum target = getGlBufferDescriptorType(descriptorSetBinding.descriptorType);

							for (uint32 descriptorBufferIndex = 0; descriptorBufferIndex < descriptorSetBinding.descriptorBuffers.count(); descriptorBufferIndex += 1) {
								const DescriptorBuffer& descriptorBuffer = descriptorSetBinding.descriptorBuffers[descriptorBufferIndex];

								#if OPENGL_VERSION >= OPENGL_VERSION_3_0
									glBindBufferRange(
										target,
										descriptorSetLayoutOffset + bindingIndex + descriptorBufferIndex,
										std::static_pointer_cast<OpenGLBuffer>(descriptorBuffer.buffer)->object,
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
								const ref<OpenGLImageView>& imageView = std::static_pointer_cast<OpenGLImageView>(descriptorImage.imageView);
								const OpenGLImage* image = pointer_cast<const OpenGLImage*>(imageView->getImage());

								#if OPENGL_VERSION >= OPENGL_VERSION_4_5
									glBindTextureUnit(descriptorSetLayoutOffset + bindingIndex + descriptorImageIndex, imageView->object);
								#else
									glActiveTexture(GL_TEXTURE0 + descriptorSetLayoutOffset + bindingIndex + descriptorImageIndex);
									glBindTexture(image->target, imageView->object);
								#endif

								if (descriptorImage.sampler != nullptr) {
									#if OPENGL_VERSION >= OPENGL_VERSION_3_3
										glBindSampler(
											descriptorSetLayoutOffset + bindingIndex + descriptorImageIndex,
											std::static_pointer_cast<OpenGLSampler>(descriptorImage.sampler)->object
										);
									#else
										ENGINE_ERROR_UNSUPPORTED();
									#endif
								}

								if (isFixedOrFloatingPointColorFormat(image->getFormat())) {
									#if OPENGL_VERSION >= OPENGL_VERSION_4_2
										glBindImageTexture(
											descriptorSetLayoutOffset + bindingIndex + descriptorImageIndex,
											imageView->object,
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
							ENGINE_ASSERT(false, "The descriptor must either have a buffer type or an image type.");
						}
					}

					descriptorSetLayoutOffset += static_cast<uint32>(glDescriptorSet->bindings.count());
				}
			}

		private:
			const sequence<ref<DescriptorSet>> descriptorSets;
	};
}