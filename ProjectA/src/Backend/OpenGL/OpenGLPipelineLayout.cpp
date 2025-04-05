#include "pch.hpp"

#include <glad/glad.h>

#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"

namespace ProjectA {
	OpenGLPipelineLayout::OpenGLPipelineLayout(const PipelineLayoutDescriptor& descriptor) :
		descriptorSetLayouts(descriptor.layouts)
	{
		/*for (uint32 layoutIndex = 0; layoutIndex < descriptor.layouts.size(); layoutIndex += 1) {
			const DescriptorSetLayout& descriptorSetLayout = descriptor.layouts[layoutIndex];

			for (uint32 bindingIndex = 0; bindingIndex < descriptorSetLayout.bindings.size(); bindingIndex += 1) {
				const DescriptorSetLayoutBinding& binding = descriptorSetLayout.bindings[bindingIndex];

				for (const ref<Buffer>& buffer : binding.buffers) {
					const ref<OpenGLBuffer> glBuffer = std::static_pointer_cast<OpenGLBuffer>(buffer);

					glBindBufferRange(getGlDescriptorType(binding.descriptorType), bindingIndex, glBuffer->object, 0, glBuffer->size);
				}

				for (uint32 imageViewIndex = 0; imageViewIndex < binding.imageViews.size(); imageViewIndex += 1) {
					const ref<OpenGLImageView> imageView = std::static_pointer_cast<OpenGLImageView>(binding.imageViews[imageViewIndex]);

					glBindTextureUnit(bindingIndex, imageView->object);

					if (imageViewIndex < binding.samplers.size()) {
						const ref<OpenGLSampler> sampler = std::static_pointer_cast<OpenGLSampler>(binding.samplers[imageViewIndex]);

						glBindSampler(bindingIndex, sampler->object);
					}

					GLenum internalFormat = imageView->image.internalFormat;

					// TODO: SRGB format is not in the list of available formats for glBindImageTexture.
					if (internalFormat == GL_SRGB8) {
						internalFormat = GL_RGBA8;
					}

					// TODO: Use correct access.
					glBindImageTexture(bindingIndex, imageView->object, 0, GL_FALSE, 0, GL_READ_WRITE, internalFormat);
				}
			}
		}*/
	}
}