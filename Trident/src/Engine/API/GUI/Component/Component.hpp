// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUIComponent {
		public:
			virtual void render(std::function<void(const ContainerStyle&)> openComponent, std::function<void()> closeComponent, std::function<void()> renderChildren) const = 0;
	};
}