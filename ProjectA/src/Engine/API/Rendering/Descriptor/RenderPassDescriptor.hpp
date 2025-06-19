#pragma once

#include "Engine/API/Rendering/Subpass.hpp"

namespace ProjectA {
	struct RenderPassDescriptor {
		sequence<ref<Subpass>> subpasses;
	};
}