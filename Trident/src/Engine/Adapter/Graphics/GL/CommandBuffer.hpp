// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"
#include "Engine/API/Graphics/CommandBuffer.hpp"

namespace Trident {
	class GL::CommandBuffer final : public Trident::CommandBuffer {
		public:
			explicit CommandBuffer(view<ref<GL::Command>> commands);

		public:
			sequence<ref<GL::Command>> commands;
	};
}