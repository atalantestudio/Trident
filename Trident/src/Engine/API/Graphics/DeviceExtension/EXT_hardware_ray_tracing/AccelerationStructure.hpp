// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"

namespace Trident {
	class AccelerationStructure {
		public:
			~AccelerationStructure();

			uint64 getDeviceAddress() const;

		public:
			ref<Device> device;
	};
}