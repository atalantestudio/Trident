// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"
#include "Engine/API/Rendering/CommandBuffer.hpp"

namespace ProjectA {
	class GL::CommandBuffer final : public ProjectA::CommandBuffer {
		public:
			explicit CommandBuffer(const std::vector<ref<GL::Command>>& commands);

		public:
			sequence<ref<GL::Command>> commands;
	};
}