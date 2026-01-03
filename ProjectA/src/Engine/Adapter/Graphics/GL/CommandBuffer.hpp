#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/CommandBuffer.hpp"

namespace ProjectA {
	class OpenGLCommandBuffer final : public CommandBuffer, public ObjectWrapper<const std::vector<ref<OpenGLCommand>>> {
		public:
			explicit OpenGLCommandBuffer(const std::vector<ref<OpenGLCommand>>& commandBuffer);

			std::vector<ref<OpenGLCommand>>::const_iterator begin() const;

			std::vector<ref<OpenGLCommand>>::const_iterator end() const;
	};
}