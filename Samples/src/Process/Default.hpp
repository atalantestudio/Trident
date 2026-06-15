// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Process/Process.hpp"

namespace Trident {
	class ProcessDefaultProcess final : public Process {
		public:
			sint32 main(view<void*> arguments) override {
				return 0;
			}
	};
}