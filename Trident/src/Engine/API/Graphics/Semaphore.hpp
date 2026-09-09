// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"

namespace Trident {
	class Semaphore {
		public:
			~Semaphore();

		public:
			explicit Semaphore(const Device* device);

		protected:
			const Device* device;
	};
}