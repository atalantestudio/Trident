// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Fence.hpp"

namespace ProjectA {
	class GL::Fence final : public ProjectA::Fence {
		public:
			explicit Fence(bool signaled);

			void create();

			void reset();

		public:
			GLsync sync;
	};
}