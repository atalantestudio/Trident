// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/CommandBuffer.hpp"

namespace Trident {
	CommandBuffer::CommandBuffer(uint32 commandCount) :
		commandCount(commandCount)
	{}

	uint32 CommandBuffer::getCommandCount() const {
		return commandCount;
	}
}