// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct WindowProperties {
		/// Width of the client area in pixels.
		uint32 Width;

		/// Height of the client area in pixels.
		uint32 Height;

		/// Whether the window is maximized.
		bool Maximized;
	};
}