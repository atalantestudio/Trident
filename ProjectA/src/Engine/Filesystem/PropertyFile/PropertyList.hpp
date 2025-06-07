#pragma once

#include "Engine/Renderer/GraphicsAPI.hpp"

namespace ProjectA {
	struct PropertyList {
		// The graphics API to create an adapter for.
		GraphicsAPI GraphicsAPI;

		// The width of the window's client area.
		uint32 WindowWidth;

		// The height of the window's client area.
		uint32 WindowHeight;

		// Whether to enable V-Sync (vertical synchronization).
		bool VSync;
	};
}