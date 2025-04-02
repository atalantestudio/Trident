#include "pch.hpp"

#include <glad/glad.h>

#include "Backend/OpenGL/DescriptorType.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"
#include "Backend/OpenGL/OpenGLImageView.hpp"
#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"
#include "Backend/OpenGL/OpenGLSampler.hpp"

namespace ProjectA {
	OpenGLPipelineLayout::OpenGLPipelineLayout(const PipelineLayoutDescriptor& descriptor) :
		descriptorSetLayouts(descriptor.layouts)
	{
		PROFILE_SCOPE(FUNC_SIG);

		// GLSL shaders are required to specify the block index and the binding,
		// saving us the calls to glGetUniformBlockIndex and glUniformBlockBinding.
		for (uint32 layoutIndex = 0; layoutIndex < descriptor.layouts.size(); layoutIndex += 1) {
			const DescriptorSetLayout& descriptorSetLayout = descriptor.layouts[layoutIndex];

			for (uint32 bindingIndex = 0; bindingIndex < descriptorSetLayout.bindings.size(); bindingIndex += 1) {
				const DescriptorSetLayoutBinding& binding = descriptorSetLayout.bindings[bindingIndex];

				for (const ref<Buffer>& buffer : binding.buffers) {
					const ref<OpenGLBuffer> glBuffer = std::static_pointer_cast<OpenGLBuffer>(buffer);

					/*switch (glBuffer->target) {
						case GL_UNIFORM_BUFFER:
							glUniformBlockBinding(program, layoutIndex, bindingIndex);

							break;
						case GL_SHADER_STORAGE_BUFFER:
							glShaderStorageBlockBinding(program, layoutIndex, bindingIndex);

							break;
					}*/

					glBindBufferRange(getGlDescriptorType(binding.descriptorType), bindingIndex, glBuffer->object, 0, glBuffer->size);
				}

				for (uint32 imageViewIndex = 0; imageViewIndex < binding.imageViews.size(); imageViewIndex += 1) {
					const ref<OpenGLImageView> imageView = std::static_pointer_cast<OpenGLImageView>(binding.imageViews[imageViewIndex]);
					const ref<OpenGLSampler> sampler = std::static_pointer_cast<OpenGLSampler>(binding.samplers[imageViewIndex]);

					glBindTextureUnit(bindingIndex, imageView->object);
					glBindSampler(bindingIndex, sampler->object);

					GLenum internalFormat = imageView->image.internalFormat;

					// TODO: SRGB format is not in the list of available formats for glBindImageTexture.
					if (internalFormat == GL_SRGB8) {
						internalFormat = GL_RGBA8;
					}

					// TODO: Use correct access.
					glBindImageTexture(bindingIndex, imageView->object, 0, GL_FALSE, 0, GL_READ_WRITE, internalFormat);
				}
			}
		}
	}
}