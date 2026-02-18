// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Access.hpp"

namespace Trident::GL {
	inline GLbitfield getBarrier(Access access) {
		GLbitfield barrier{};

		if (access & Access::SHADER_WRITE) {
			barrier |= GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT | GL_UNIFORM_BARRIER_BIT;
		}

		if (access & Access::TRANSFER_WRITE) {
			barrier |= GL_BUFFER_UPDATE_BARRIER_BIT;
		}

		return barrier;
	}
}