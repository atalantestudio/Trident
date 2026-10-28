// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/GUILayer.hpp"

namespace Trident {
	std::vector<uint32> GUILayer::FilterChildren(const GUIComponent& parent, uint8 axis, FillMode fillMode) const {
		std::vector<uint32> childIndices;

		for (const uint32 childIndex : parent.childIndices) {
			const GUIComponent& component = components[childIndex];

			if (component.size[axis].fillMode == fillMode) {
				childIndices.emplace_back(childIndex);
			}
		}

		return childIndices;
	}

	float32 GUILayer::calculateRemainingSize(const GUIComponent& component, uint8 axis) const {
		float32 remainingSize = component.size[axis].value - component.padding[axis * 2 + 0] - component.padding[axis * 2 + 1];

		for (const uint32 childIndex : component.childIndices) {
			const GUIComponent& child = components[childIndex];

			if (child.floating) {
				continue;
			}

			remainingSize -= child.size[axis].value;
		}

		remainingSize -= component.layoutSpacing * max(static_cast<index>(0), component.childIndices.size() - 1);

		return remainingSize;
	}

	void GUILayer::AdjustWidth(uint32 componentIndex) {
		GUIComponent& component = components[componentIndex];

		const float32 horizontalPadding = component.padding[0] + component.padding[1];

		// Add padding to the component's size.
		component.size.width.value += horizontalPadding;

		// Add layout spacing to the component's size.
		if (component.layoutDirection == LayoutDirection::HORIZONTAL) {
			const index childCount = max(static_cast<index>(0), component.childIndices.size() - 1);

			component.size.width.value += component.layoutSpacing * childCount;
		}

		if (component.floating) {
			component.minimumSize.x += horizontalPadding;

			return;
		}

		GUIComponent& parent = components[component.parentIndex];

		if (parent.size.width.fillMode != FillMode::FIXED) {
			if (parent.layoutDirection == LayoutDirection::HORIZONTAL) {
				parent.size.width.value += component.size.width.value;
				parent.minimumSize.x += component.minimumSize.x;
			}
			else {
				parent.size.width.value = max(parent.size.width.value, component.size.width.value);
				parent.minimumSize.x = max(parent.minimumSize.x, component.minimumSize.x);
			}
		}

		parent.size.width.value = min(parent.size.width.value, parent.maximumSize.x);
		parent.maximumSize.x = max(parent.maximumSize.x, parent.minimumSize.x);

		component.minimumSize.x += horizontalPadding;
	}

	void GUILayer::AdjustHeight(uint32 componentIndex) {
		GUIComponent& component = components[componentIndex];

		const float32 verticalPadding = component.padding[2] + component.padding[3];

		component.size.height.value += verticalPadding;
		component.minimumSize.y += verticalPadding;

		if (component.layoutDirection == LayoutDirection::VERTICAL) {
			const index childCount = max(static_cast<index>(0), component.childIndices.size() - 1);

			component.size.height.value += component.layoutSpacing * childCount;
			component.minimumSize.y += component.layoutSpacing * childCount;
		}

		if (component.floating) {
			return;
		}

		GUIComponent& parent = components[component.parentIndex];

		if (parent.type != GUIComponentType::VERTICAL_LIST && parent.size.height.fillMode != FillMode::FIXED) {
			if (parent.layoutDirection == LayoutDirection::VERTICAL) {
				parent.size.height.value += component.size.height.value;
				parent.minimumSize.y += component.minimumSize.y;
			}
			else {
				parent.size.height.value = max(parent.size.height.value, component.size.height.value);
				parent.minimumSize.y = max(parent.minimumSize.y, component.minimumSize.y);
			}
		}

		parent.size.height.value = min(parent.size.height.value, parent.maximumSize.y);
		parent.maximumSize.y = max(parent.minimumSize.y, parent.maximumSize.y);
	}

	void GUILayer::expandAndFitComponent(uint32 componentIndex, uint8 axis) {
		GUIComponent& component = components[componentIndex];

		if (component.type != GUIComponentType::VERTICAL_LIST && component.layoutDirection == static_cast<LayoutDirection>(axis)) {
			std::vector<uint32> expandableChildIndices = FilterChildren(component, axis, FillMode::EXPAND);
			std::vector<uint32> fittableChildIndices = FilterChildren(component, axis, FillMode::FIT);
			float32 remainingSize = calculateRemainingSize(component, axis);

			// TODO(gui/expand-and-fit): Merge expand and fit passes.
			while (remainingSize > 0.0f && expandableChildIndices.size() > 0) {
				float32 smallestSize = components[expandableChildIndices[0]].size[axis].value;
				float32 secondSmallestSize = max<float32>();
				float32 sizeToAdd = remainingSize;

				for (const uint32 childIndex : expandableChildIndices) {
					const GUIComponent& child = components[childIndex];
					const float32 size = child.size[axis].value;

					if (size > smallestSize) {
						// If we have found a child larger than the smallest registered size, replace it with the child's size.
						secondSmallestSize = smallestSize;
						smallestSize = size;

						continue;
					}

					if (size < smallestSize) {
						// This child is not the smallest.
						secondSmallestSize = min(secondSmallestSize, size);
						sizeToAdd = secondSmallestSize - smallestSize;
					}
				}

				// Double-check that the size to add is >= than the equal size distribution among the children.
				sizeToAdd = min(sizeToAdd, remainingSize / expandableChildIndices.size());

				sint32 index = static_cast<sint32>(expandableChildIndices.size()) - 1;

				while (index >= 0) {
					const uint32 childIndex = expandableChildIndices[index];

					GUIComponent& child = components[childIndex];

					if (child.size[axis].value == smallestSize) {
						const float32 previousSize = child.size[axis].value;

						child.size[axis].value += sizeToAdd;

						if (child.size[axis].value > child.maximumSize[axis]) {
							child.size[axis].value = child.maximumSize[axis];

							expandableChildIndices.erase(expandableChildIndices.begin() + index);
						}

						remainingSize -= child.size[axis].value - previousSize;
					}

					index -= 1;
				}
			}

			// TODO(gui/expand-and-fit): Merge expand and fit passes.
			while (remainingSize < 0.0f && fittableChildIndices.size() > 0) {
				float32 largestSize = components[fittableChildIndices[0]].size[axis].value;
				float32 secondLargestSize = 0.0f;
				float32 sizeToRemove = remainingSize;

				for (const uint32 childIndex : fittableChildIndices) {
					const GUIComponent& child = components[childIndex];
					const float32 size = child.size[axis].value;

					if (size > largestSize) {
						// If we have found a child larger than the largest registered size, replace it with the child's size.
						secondLargestSize = largestSize;
						largestSize = size;

						continue;
					}

					if (size < largestSize) {
						// This child is not the largest.
						secondLargestSize = max(secondLargestSize, size);
						sizeToRemove = secondLargestSize - largestSize;
					}
				}

				// Double-check that the size to remove is >= than the equal size distribution among the children.
				sizeToRemove = max(sizeToRemove, remainingSize / fittableChildIndices.size());

				sint32 index = static_cast<sint32>(fittableChildIndices.size()) - 1;

				while (index >= 0) {
					const uint32 childIndex = fittableChildIndices[index];

					GUIComponent& child = components[childIndex];

					if (child.size[axis].value == largestSize) {
						const float32 previousSize = child.size[axis].value;

						child.size[axis].value += sizeToRemove;

						if (child.size[axis].value < child.minimumSize[axis]) {
							child.size[axis].value = child.minimumSize[axis];

							fittableChildIndices.erase(fittableChildIndices.begin() + index);
						}

						remainingSize -= child.size[axis].value - previousSize;
					}

					index -= 1;
				}
			}
		}
		else {
			for (const uint32 childIndex : component.childIndices) {
				GUIComponent& child = components[childIndex];

				if (child.size[axis].fillMode != FillMode::EXPAND) {
					continue;
				}

				child.size[axis].value = component.size[axis].value - component.padding[axis * 2 + 0] - component.padding[axis * 2 + 1];
			}
		}

		for (const uint32 childIndex : component.childIndices) {
			if (components[childIndex].type == GUIComponentType::TEXT) {
				continue;
			}

			expandAndFitComponent(childIndex, axis);
		}
	}
}