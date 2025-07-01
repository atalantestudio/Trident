#pragma once

#include "Engine/Renderer/GraphicsAPI.hpp"

namespace ProjectA {
	// The properties from this list can be edited in the project's property file (.tpr).
	// Values are used when initializating the application.
	// The property file won't be automatically updated when a setting corresponding to a property is changed.
	// 
	// NOTE: Property values are not guaranteed to be used.
	// If a value is either invalid or not supported at runtime,
	// the engine will fall-back to an internal default without modifying the property file.
	struct ApplicationProperties {
		// Graphics API to use.
		GraphicsAPI GraphicsAPI;

		// Width of the window's client area, in pixels.
		uint32 WindowWidth;

		// Height of the window's client area, in pixels.
		uint32 WindowHeight;

		// Whether to enable V-Sync (vertical synchronization).
		bool VSync;
	};
}