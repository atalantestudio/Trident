// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/GUILayer.hpp"

namespace ProjectA {
	class GUIComponent {
		public:
			virtual void render(GUILayer& layer, std::function<void(const ContainerStyle&)> openComponent, std::function<void()> closeComponent, std::function<void(GUILayer&)> renderChildren) const = 0;
	};
}