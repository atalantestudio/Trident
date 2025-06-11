#pragma once

#include "Engine/GUI/GUISize.hpp"
#include "Engine/GUI/GUILayer.hpp"

namespace ProjectA {
	enum LayoutDirection;

	struct GUIContainer {
		// Top-left corner of the container, excluding padding.
		GUISize origin;

		GUISize padding;

		// Container size, including padding.
		GUISize size;

		// Container content size, excluding padding.
		GUISize innerSize;

		GUISize parentSize;
		GUISize parentInnerSize;

		// The parent layout direction for root containers is VERTICAL.
		LayoutDirection parentLayoutDirection;

		uint16 parentLayoutSpacing;

		bool root;

		bool iteration = false;
	};
}