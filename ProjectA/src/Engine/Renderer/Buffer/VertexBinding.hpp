#pragma once

#include "Engine/Renderer/InputRate.hpp"

namespace ProjectA {
	struct VertexBinding {
		uint32 index;
		uint32 stride;
		InputRate inputRate;
	};
}