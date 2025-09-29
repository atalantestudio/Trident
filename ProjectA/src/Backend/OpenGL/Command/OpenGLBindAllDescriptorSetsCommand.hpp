#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"
#include "Backend/OpenGL/OpenGLImage.hpp"
#include "Backend/OpenGL/OpenGLImageView.hpp"
#include "Backend/OpenGL/OpenGLSampler.hpp"

#include "Engine/API/Rendering/DescriptorSetLayout.hpp"

#include "Backend/OpenGL/DescriptorType.hpp"

namespace ProjectA {
	class OpenGLBindAllDescriptorSetsCommand final : public OpenGLCommand {
		public:
			explicit OpenGLBindAllDescriptorSetsCommand(const std::vector<DescriptorSetLayout>& descriptorSetLayouts, bool isCompute) :
				descriptorSetLayouts(descriptorSetLayouts),
				isCompute(isCompute)
			{}

			void execute() const final override {
				uint32 descriptorSetLayoutOffset = 0;

				for (const DescriptorSetLayout& descriptorSetLayout : descriptorSetLayouts) {
					for (uint32 binding = 0; binding < descriptorSetLayout.bindings.size(); binding += 1) {
						const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSetLayout.bindings[binding];

						for (const ref<Buffer>& buffer : descriptorSetLayoutBinding.buffers) {
							const ref<OpenGLBuffer>& glBuffer = std::static_pointer_cast<OpenGLBuffer>(buffer);
							const GLintptr offset = descriptorSetLayoutBinding.bufferOffset;
							const GLsizeiptr size = glBuffer->size - offset;

							#if OPENGL_VERSION >= OPENGL_VERSION_3_0
								glBindBufferRange(getGlDescriptorType(descriptorSetLayoutBinding.descriptorType), descriptorSetLayoutOffset + binding, glBuffer->object, offset, size);
							#else
								ENGINE_ERROR_UNSUPPORTED();
							#endif
						}

						for (GLuint unit = 0; unit < descriptorSetLayoutBinding.imageViews.size(); unit += 1) {
							const ref<OpenGLImageView>& imageView = std::static_pointer_cast<OpenGLImageView>(descriptorSetLayoutBinding.imageViews[unit]);
							const OpenGLImage& image = *static_cast<const OpenGLImage*>(imageView->getImage());

							#if OPENGL_VERSION >= OPENGL_VERSION_4_5
								glBindTextureUnit(descriptorSetLayoutOffset + binding + unit, imageView->object);
							#else
								glActiveTexture(GL_TEXTURE0 + descriptorSetLayoutOffset + binding + unit);
								glBindTexture(image.target, imageView->object);
							#endif

							if (unit < descriptorSetLayoutBinding.samplers.size()) {
								const ref<OpenGLSampler>& sampler = std::static_pointer_cast<OpenGLSampler>(descriptorSetLayoutBinding.samplers[unit]);

								#if OPENGL_VERSION >= OPENGL_VERSION_3_3
									glBindSampler(descriptorSetLayoutOffset + binding + unit, sampler->object);
								#else
									ENGINE_ERROR_UNSUPPORTED();
								#endif
							}

							if (isCompute) {
								GLenum format = image.internalFormat;

								if (format == GL_SRGB8_ALPHA8) {
									format = GL_RGBA8;
								}

								#if OPENGL_VERSION >= OPENGL_VERSION_4_2
									// TODO: Use correct access.
									glBindImageTexture(descriptorSetLayoutOffset + binding + unit, imageView->object, 0, GL_FALSE, 0, GL_READ_WRITE, format);
								#else
									ENGINE_ERROR_UNSUPPORTED();
								#endif
							}
						}
					}

					descriptorSetLayoutOffset += static_cast<uint32>(descriptorSetLayout.bindings.size());
				}
			}

		private:
			const std::vector<DescriptorSetLayout>& descriptorSetLayouts;
			const bool isCompute;
	};
}