// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/GUILayer.hpp"

namespace Trident {
	void GUILayer::focusComponent(uint32 componentIndex) const {
		const GUIContainer& component = components[componentIndex];
		const Window* window = getApplication()->getWindow();

		switch (component.type) {
			case GUIComponentType::TEXT_INPUT: {
				// TODO(gui/component/text-input): Fix text cursor positioning only working when hitting a breakpoint.
				/* const vec2<sint32> position = vec2<sint32>(round(component.origin + vec2<float32>{component.padding.x, component.padding.y}));
				const vec2<sint32> size = vec2<sint32>(round(vec2<float32>{TEXT_CURSOR_WIDTH, component.size.height.value - component.padding.w - component.padding.y}));

				window->updateTextCursor(position, size); */

				break;
			}
			default: {
				break;
			}
		}
	}

	void GUILayer::unfocusComponent(uint32 componentIndex) const {
		const GUIContainer& component = components[componentIndex];

		switch (component.type) {
			case GUIComponentType::TEXT_INPUT: {
				if (component.inputText->hasSelection()) {
					component.inputText->unselect();
				}

				break;
			}
			default: {
				break;
			}
		}
	}
}