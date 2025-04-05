#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLBindComputePipelineCommand.hpp"
#include "Backend/OpenGL/DescriptorType.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"
#include "Backend/OpenGL/OpenGLImageView.hpp"
#include "Backend/OpenGL/OpenGLSampler.hpp"

namespace ProjectA {
	OpenGLBindComputePipelineCommand::OpenGLBindComputePipelineCommand(const ref<OpenGLComputePipeline>& computePipeline) :
		computePipeline(computePipeline)
	{}

	void OpenGLBindComputePipelineCommand::execute() const {
		glUseProgram(computePipeline->program);

		// TODO: Merge this code for both pipeline types.
		// GLSL shaders are required to specify the block index and the binding,
		// saving us the calls to glGetUniformBlockIndex and glUniformBlockBinding.
		for (const DescriptorSetLayout& descriptorSetLayout : computePipeline->descriptorSetLayouts) {
			for (uint32 binding = 0; binding < descriptorSetLayout.bindings.size(); binding += 1) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSetLayout.bindings[binding];

				for (const ref<Buffer>& buffer : descriptorSetLayoutBinding.buffers) {
					const ref<OpenGLBuffer> glBuffer = std::static_pointer_cast<OpenGLBuffer>(buffer);

					glBindBufferRange(getGlDescriptorType(descriptorSetLayoutBinding.descriptorType), binding, glBuffer->object, 0, glBuffer->size);
				}

				for (uint32 imageViewIndex = 0; imageViewIndex < descriptorSetLayoutBinding.imageViews.size(); imageViewIndex += 1) {
					const ref<OpenGLImageView> imageView = std::static_pointer_cast<OpenGLImageView>(descriptorSetLayoutBinding.imageViews[imageViewIndex]);

					glBindTextureUnit(binding, imageView->object);

					if (imageViewIndex < descriptorSetLayoutBinding.samplers.size()) {
						const ref<OpenGLSampler> sampler = std::static_pointer_cast<OpenGLSampler>(descriptorSetLayoutBinding.samplers[imageViewIndex]);

						glBindSampler(binding, sampler->object);
					}

					GLenum internalFormat = imageView->image.internalFormat;

					// TODO: SRGB format is not in the list of available formats for glBindImageTexture.
					if (internalFormat == GL_SRGB8) {
						internalFormat = GL_RGBA8;
					}

					// TODO: Use correct access.
					glBindImageTexture(binding, imageView->object, 0, GL_FALSE, 0, GL_READ_WRITE, internalFormat);
				}
			}
		}
	}
}