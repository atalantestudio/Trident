#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"
#include "Backend/OpenGL/OpenGLImageView.hpp"
#include "Backend/OpenGL/OpenGLSampler.hpp"

#include "Engine/API/Rendering/DescriptorSetLayout.hpp"

#include "Backend/OpenGL/DescriptorType.hpp"

namespace ProjectA {
	class OpenGLBindAllDescriptorSetsCommand : public OpenGLCommand {
		public:
			OpenGLBindAllDescriptorSetsCommand(const std::vector<DescriptorSetLayout>& descriptorSetLayouts) :
				descriptorSetLayouts(descriptorSetLayouts)
			{}

			void execute() const {
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

							glBindTextureUnit(binding + unit, imageView->object);

							if (unit < descriptorSetLayoutBinding.samplers.size()) {
								const ref<OpenGLSampler>& sampler = std::static_pointer_cast<OpenGLSampler>(descriptorSetLayoutBinding.samplers[unit]);

								glBindSampler(binding + unit, sampler->object);
							}

							GLenum format = imageView->image.internalFormat;

							if (format == GL_SRGB8_ALPHA8) {
								format = GL_RGBA8;
							}

							// TODO: Use correct access.
							// TODO: Only call glBindImageTexture if the pipeline is a compute pipeline.
							glBindImageTexture(binding + unit, imageView->object, 0, GL_FALSE, 0, GL_READ_WRITE, format);
						}
					}
				}
			}

		private:
			const std::vector<DescriptorSetLayout>& descriptorSetLayouts;
	};
}