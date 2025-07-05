#pragma once

#include "Engine/API/Rendering/Subpass.hpp"
#include "Engine/API/Rendering/SubpassDependency.hpp"

namespace ProjectA {
	struct RenderPassDescriptor {
		sequence<ref<Subpass>> subpasses;
		sequence<SubpassDependency> subpassDependencies;
	};
}