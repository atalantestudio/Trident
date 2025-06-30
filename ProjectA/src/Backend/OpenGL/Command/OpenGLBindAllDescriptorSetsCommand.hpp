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
				for (const DescriptorSetLayout& descriptorSetLayout : descriptorSetLayouts) {
					for (uint32 binding = 0; binding < descriptorSetLayout.bindings.size(); binding += 1) {
						const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSetLayout.bindings[binding];

						for (const ref<Buffer>& buffer : descriptorSetLayoutBinding.buffers) {
							const ref<OpenGLBuffer>& glBuffer = std::static_pointer_cast<OpenGLBuffer>(buffer);
							const GLintptr offset = descriptorSetLayoutBinding.bufferOffset;
							const GLsizeiptr size = glBuffer->size - offset;

							glBindBufferRange(getGlDescriptorType(descriptorSetLayoutBinding.descriptorType), binding, glBuffer->object, offset, size);
						}

						for (GLuint unit = 0; unit < descriptorSetLayoutBinding.imageViews.size(); unit += 1) {
							const ref<OpenGLImageView>& imageView = std::static_pointer_cast<OpenGLImageView>(descriptorSetLayoutBinding.imageViews[unit]);
							const OpenGLImage& image = *static_cast<const OpenGLImage*>(imageView->getImage());

							#ifdef OPENGL_USE_DSA
								glBindTextureUnit(binding + unit, imageView->object);
							#else
								glActiveTexture(GL_TEXTURE0 + binding + unit);
								glBindTexture(image.target, imageView->object);
							#endif

							if (unit < descriptorSetLayoutBinding.samplers.size()) {
								const ref<OpenGLSampler>& sampler = std::static_pointer_cast<OpenGLSampler>(descriptorSetLayoutBinding.samplers[unit]);

								glBindSampler(binding + unit, sampler->object);
							}

							if (isCompute) {
								GLenum format = image.internalFormat;

								if (format == GL_SRGB8_ALPHA8) {
									format = GL_RGBA8;
								}

								// TODO: Use correct access.
								glBindImageTexture(binding + unit, imageView->object, 0, GL_FALSE, 0, GL_READ_WRITE, format);
							}
						}
					}
				}
			}

		private:
			const std::vector<DescriptorSetLayout>& descriptorSetLayouts;
			const bool isCompute;
	};
}