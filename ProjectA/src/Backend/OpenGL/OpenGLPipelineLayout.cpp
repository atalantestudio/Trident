#include "pch.hpp"

#include <glad/glad.h>

#include "Backend/OpenGL/DescriptorType.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"
#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"
#include "Backend/OpenGL/OpenGLSampler.hpp"
#include "Backend/OpenGL/OpenGLTextureView.hpp"

namespace ProjectA {
	OpenGLPipelineLayout::OpenGLPipelineLayout(const PipelineLayoutDescriptor& descriptor) :
		descriptorSetLayouts(descriptor.layouts)
	{
		PROFILE_SCOPE(FUNCSIG);

		// GLSL shaders are required to specify the block index and the binding,
		// saving us the calls to glGetUniformBlockIndex and glUniformBlockBinding.
		for (uint32 layoutIndex = 0; layoutIndex < descriptor.layouts.size(); layoutIndex += 1) {
			const DescriptorSetLayout& descriptorSetLayout = descriptor.layouts[layoutIndex];

			for (uint32 bindingIndex = 0; bindingIndex < descriptorSetLayout.bindings.size(); bindingIndex += 1) {
				const DescriptorSetLayoutBinding& binding = descriptorSetLayout.bindings[bindingIndex];

				for (const ref<Buffer>& buffer : binding.buffers) {
					const OpenGLBuffer* glBuffer = static_cast<const OpenGLBuffer*>(buffer.get());

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

				for (uint32 textureViewIndex = 0; textureViewIndex < binding.textureViews.size(); textureViewIndex += 1) {
					const OpenGLTextureView* textureView = static_cast<const OpenGLTextureView*>(binding.textureViews[textureViewIndex].get());
					const OpenGLSampler* sampler = static_cast<const OpenGLSampler*>(binding.samplers[textureViewIndex].get());

					glBindTextureUnit(bindingIndex, textureView->object);
					glBindSampler(bindingIndex, sampler->object);

					// TODO: Use correct access and format.
					glBindImageTexture(bindingIndex, textureView->object, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
				}
			}
		}
	}
}