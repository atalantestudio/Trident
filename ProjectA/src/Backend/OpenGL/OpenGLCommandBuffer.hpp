#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/CommandBuffer.hpp"

namespace ProjectA {
	class OpenGLCommandBuffer : public CommandBuffer, public ObjectWrapper<const std::vector<ref<OpenGLCommand>>> {
		public:
			OpenGLCommandBuffer(const std::vector<ref<OpenGLCommand>>& commandBuffer);

			std::vector<ref<OpenGLCommand>>::const_iterator begin() const;

			std::vector<ref<OpenGLCommand>>::const_iterator end() const;
	};
}