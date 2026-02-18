// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Trident.hpp"

namespace Trident {
	class ProcessDefaultProcess : public Process {
		public:
			sint32 main(view<void*> arguments) override {
				return 0;
			}
	};
}