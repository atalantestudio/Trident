// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Fence.hpp"

namespace Trident {
	class GL::Fence final : public Trident::Fence {
		public:
			explicit Fence(const FenceDescriptor& descriptor);

			void create();

			void reset();

		public:
			GLsync sync;
	};
}