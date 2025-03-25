#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/CommandBuffer.hpp"

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLCommandBuffer : public CommandBuffer, public ObjectWrapper<const chain<ref<OpenGLCommand>>> {
		public:
			OpenGLCommandBuffer(const chain<ref<OpenGLCommand>>& commandBuffer);

			chain<ref<OpenGLCommand>>::const_iterator begin() const;

			chain<ref<OpenGLCommand>>::const_iterator end() const;
	};
}