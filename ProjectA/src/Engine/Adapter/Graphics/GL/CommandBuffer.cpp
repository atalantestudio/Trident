// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/CommandBuffer.hpp"

namespace ProjectA {
	GL::CommandBuffer::CommandBuffer(const std::vector<ref<GL::Command>>& commands) :
		ProjectA::CommandBuffer(static_cast<uint32>(commands.size())),
		commands(commands.size())
	{
		std::copy(commands.begin(), commands.end(), &this->commands[0]);
	}
}