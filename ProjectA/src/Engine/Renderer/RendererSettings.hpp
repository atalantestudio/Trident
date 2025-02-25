#pragma once

#include "Engine/Renderer/GraphicsAPI.hpp"

namespace ProjectA {
	// TODO: Some settings should be global to the application
	// (e.g. the graphics API or the window viewport).
	struct RendererSettings {
		GraphicsAPI graphicsApi;
	};
}