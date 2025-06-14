#pragma once

#include "Engine/GUI/GUILayer.hpp"

namespace ProjectA {
	enum LayoutDirection;

	struct GUIContainer {
		// Top-left corner of the container, excluding padding.
		Extent2D origin;

		Extent2D padding;

		// Container size, including padding.
		Extent2D size;

		// Container content size, excluding padding.
		Extent2D innerSize;

		Extent2D parentSize;
		Extent2D parentInnerSize;

		// The parent layout direction for root containers is VERTICAL.
		LayoutDirection parentLayoutDirection;

		uint32 parentLayoutSpacing;

		bool root;

		bool iteration = false;
	};
}