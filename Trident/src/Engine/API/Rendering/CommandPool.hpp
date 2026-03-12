// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Device.hpp"

namespace Trident {
	class CommandPool {
		public:
			explicit CommandPool(const Device* device);

			~CommandPool();

		protected:
			const Device* device;
	};
}