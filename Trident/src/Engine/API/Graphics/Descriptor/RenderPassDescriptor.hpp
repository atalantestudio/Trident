// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Subpass.hpp"
#include "Engine/API/Graphics/SubpassDependency.hpp"

namespace Trident {
	struct RenderPassDescriptor {
		sequence<Attachment> attachments;
		sequence<ref<Subpass>> subpasses;
		sequence<SubpassDependency> subpassDependencies;
	};
}