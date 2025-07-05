#pragma once

#include "Engine/API/Rendering/Attachment.hpp"

namespace ProjectA {
	struct SubpassDescriptor {
		sequence<ColorAttachment> colorAttachments;
		bool depthStencil;
		DepthStencilAttachment depthStencilAttachment;
	};
}