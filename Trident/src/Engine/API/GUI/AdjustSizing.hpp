// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/GUILayer.hpp"

namespace Trident {
	std::vector<uint32> GUILayer::FilterChildren(const GUIContainer& parent, uint8 axis, FillMode fillMode) const {
		std::vector<uint32> childIndices;

		for (const uint32 childIndex : parent.childIndices) {
			const GUIContainer& component = components[childIndex];

			if (component.size[axis].fillMode == fillMode) {
				childIndices.emplace_back(childIndex);
			}
		}

		return childIndices;
	}

	void GUILayer::AdjustWidth(uint32 componentIndex) {
		GUIContainer& component = components[componentIndex];

		// Add padding to the component's size.
		component.size.width.value += component.padding.x * 2.0f;

		// Add layout spacing to the component's size.
		if (component.layoutDirection == LayoutDirection::HORIZONTAL) {
			const index childCount = max(static_cast<index>(0), component.childIndices.size() - 1);

			component.size.width.value += component.layoutSpacing * childCount;
		}

		if (component.floating) {
			component.minimumSize.x += component.padding.x * 2.0f;

			return;
		}

		DEBUG_ASSERT(component.parentIndex != max<uint32>());

		GUIContainer& parent = components[component.parentIndex];

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

		component.minimumSize.x += component.padding.x * 2.0f;
	}

	void GUILayer::AdjustHeight(uint32 componentIndex) {
		GUIContainer& component = components[componentIndex];

		component.size.height.value += component.padding.y * 2.0f;
		component.minimumSize.y += component.padding.y * 2.0f;

		if (component.layoutDirection == LayoutDirection::VERTICAL) {
			const index childCount = max(static_cast<index>(0), component.childIndices.size() - 1);

			component.size.height.value += component.layoutSpacing * childCount;
			component.minimumSize.y += component.layoutSpacing * childCount;
		}

		if (component.floating) {
			return;
		}

		DEBUG_ASSERT(component.parentIndex != max<uint32>());

		GUIContainer& parent = components[component.parentIndex];

		if (parent.size.height.fillMode != FillMode::FIXED) {
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

	void GUILayer::GrowAndShrinkSizingWidthsRecursive(uint32 parentIndex) {
		GUIContainer& parent = components[parentIndex];

		const std::vector<uint32> growableChildIndices = FilterChildren(parent, 0, FillMode::EXPAND);

		if (parent.layoutDirection == LayoutDirection::HORIZONTAL) {
			float32 remainingWidth = parent.size.width.value - parent.padding.x * 2.0f;

			for (const uint32 childIndex : parent.childIndices) {
				const GUIContainer& child = components[childIndex];

				if (!child.floating) {
					remainingWidth -= child.size.width.value;
				}
			}

			remainingWidth -= parent.layoutSpacing * (parent.childIndices.size() - 1);

			if (growableChildIndices.size() > 0) {
				std::vector<uint32> growableChildIndexCopy = growableChildIndices;

				// Grow children.
				while (remainingWidth > 0.0f) {
					if (growableChildIndexCopy.size() == 0) {
						break;
					}

					float32 smallestWidth = components[growableChildIndexCopy[0]].size.width.value;
					float32 secondSmallestWidth = max<float32>();
					float32 widthToAdd = remainingWidth;

					for (const uint32 childIndex : growableChildIndexCopy) {
						const GUIContainer& child = components[childIndex];
						const float32 width = child.size.width.value;

						// If we have found a child smaller than the smallest width registered, replace it with the child's width.
						if (width < smallestWidth) {
							secondSmallestWidth = smallestWidth;
							smallestWidth = width;
						}

						// This child not the smallest.
						if (width > smallestWidth) {
							secondSmallestWidth = min(secondSmallestWidth, width);
							widthToAdd = secondSmallestWidth - smallestWidth;
						}
					}

					// Double-check that the width to add is <= than the equal distribution of the width among children.
					widthToAdd = min(widthToAdd, remainingWidth / growableChildIndexCopy.size());

					sint32 index = static_cast<sint32>(growableChildIndexCopy.size()) - 1;

					while (index >= 0) {
						const uint32 childIndex = growableChildIndexCopy[index];
						GUIContainer& child = components[childIndex];

						const float32 previousWidth = child.size.width.value;

						if (child.size.width.value == smallestWidth) {
							child.size.width.value += widthToAdd;

							if (child.size.width.value > child.maximumSize.x) {
								child.size.width.value = child.maximumSize.x;

								growableChildIndexCopy.erase(growableChildIndexCopy.begin() + index);
							}

							remainingWidth -= child.size.width.value - previousWidth;
						}

						index -= 1;
					}
				}
			}

			const std::vector<uint32> shrinkableChildIndices = FilterChildren(parent, 0, FillMode::FIT);

			if (shrinkableChildIndices.size() > 0) {
				std::vector<uint32> shrinkableChildIndexCopy = shrinkableChildIndices;

				// Shrink children.
				while (remainingWidth < 0.0f) {
					if (shrinkableChildIndexCopy.size() == 0) {
						break;
					}

					float32 largestWidth = components[shrinkableChildIndexCopy[0]].size.width.value;
					float32 secondLargestWidth = 0.0f;
					float32 widthToRemove = remainingWidth;

					for (const uint32 childIndex : shrinkableChildIndexCopy) {
						const GUIContainer& child = components[childIndex];
						const float32 width = child.size.width.value;

						// If we have found a child larger than the largest width registered, replace it with the child's width.
						if (width > largestWidth) {
							secondLargestWidth = largestWidth;
							largestWidth = width;
						}

						// This child is not the largest.
						if (width < largestWidth) {
							secondLargestWidth = max(secondLargestWidth, width);
							widthToRemove = secondLargestWidth - largestWidth;
						}
					}

					// Double-check that the width to remove is >= than the equal distribution of the width among children.
					widthToRemove = max(widthToRemove, remainingWidth / shrinkableChildIndexCopy.size());

					sint32 index = static_cast<sint32>(shrinkableChildIndexCopy.size()) - 1;

					while (index >= 0) {
						const uint32 childIndex = shrinkableChildIndexCopy[index];
						GUIContainer& child = components[childIndex];

						const float32 previousWidth = child.size.width.value;

						if (child.size.width.value == largestWidth) {
							child.size.width.value += widthToRemove;

							if (child.size.width.value < child.minimumSize.x) {
								child.size.width.value = child.minimumSize.x;

								shrinkableChildIndexCopy.erase(shrinkableChildIndexCopy.begin() + index);
							}

							remainingWidth -= child.size.width.value - previousWidth;
						}

						index -= 1;
					}
				}
			}
		}
		else {
			for (const uint32 childIndex : growableChildIndices) {
				GUIContainer& child = components[childIndex];

				child.size.width.value = parent.size.width.value - parent.padding.x * 2.0f;
			}
		}

		for (const uint32 childIndex : parent.childIndices) {
			if (!components[childIndex].textComponent) {
				GrowAndShrinkSizingWidthsRecursive(childIndex);
			}
		}
	}

	void GUILayer::GrowAndShrinkSizingHeightsRecursive(uint32 parentIndex) {
		GUIContainer& parent = components[parentIndex];

		const std::vector<uint32> growableChildIndices = FilterChildren(parent, 1, FillMode::EXPAND);

		if (parent.layoutDirection == LayoutDirection::VERTICAL) {
			float32 remainingHeight = parent.size.height.value - parent.padding.y * 2.0f;

			for (const uint32 childIndex : parent.childIndices) {
				const GUIContainer& child = components[childIndex];

				if (!child.floating) {
					remainingHeight -= child.size.height.value;
				}
			}

			remainingHeight -= parent.layoutSpacing * (parent.childIndices.size() - 1);

			if (growableChildIndices.size() > 0) {
				std::vector<uint32> growableChildIndexCopy = growableChildIndices;

				// Grow children.
				while (remainingHeight > 0.0f) {
					if (growableChildIndexCopy.size() == 0) {
						break;
					}

					float32 smallestHeight = components[growableChildIndexCopy[0]].size.height.value;
					float32 secondSmallestHeight = max<float32>();
					float32 heightToAdd = remainingHeight;

					for (const uint32 childIndex : growableChildIndexCopy) {
						const GUIContainer& child = components[childIndex];
						const float32 height = child.size.height.value;

						// If we have found a child smaller than the smallest height registered, replace it with the child's height.
						if (height < smallestHeight) {
							secondSmallestHeight = smallestHeight;
							smallestHeight = height;
						}

						// This child not the smallest.
						if (height > smallestHeight) {
							secondSmallestHeight = min(secondSmallestHeight, height);
							heightToAdd = secondSmallestHeight - smallestHeight;
						}
					}

					// Double-check that the height to add is <= than the equal distribution of the height among expanding children.
					heightToAdd = min(heightToAdd, remainingHeight / growableChildIndexCopy.size());

					sint32 index = static_cast<sint32>(growableChildIndexCopy.size()) - 1;

					while (index >= 0) {
						const uint32 childIndex = growableChildIndexCopy[index];

						GUIContainer& child = components[childIndex];

						const float32 previousHeight = child.size.height.value;

						if (child.size.height.value == smallestHeight) {
							child.size.height.value += heightToAdd;

							if (child.size.height.value > child.maximumSize.y) {
								child.size.height.value = child.maximumSize.y;

								growableChildIndexCopy.erase(growableChildIndexCopy.begin() + index);
							}

							remainingHeight -= child.size.height.value - previousHeight;
						}

						index -= 1;
					}
				}
			}

			const std::vector<uint32> shrinkableChildIndices = FilterChildren(parent, 1, FillMode::FIT);

			if (shrinkableChildIndices.size() > 0) {
				std::vector<uint32> shrinkableChildIndexCopy = shrinkableChildIndices;

				// Shrink children.
				while (remainingHeight < 0.0f) {
					if (shrinkableChildIndexCopy.size() == 0) {
						break;
					}

					float32 largestHeight = components[shrinkableChildIndexCopy[0]].size.height.value;
					float32 secondLargestHeight = 0.0f;
					float32 heightToRemove = remainingHeight;

					for (const uint32 childIndex : shrinkableChildIndexCopy) {
						const GUIContainer& child = components[childIndex];
						const float32 height = child.size.height.value;

						// If we have found a child larger than the largest height registered, replace it with the child's height.
						if (height > largestHeight) {
							secondLargestHeight = largestHeight;
							largestHeight = height;
						}

						// This child is not the largest.
						if (height < largestHeight) {
							secondLargestHeight = max(secondLargestHeight, height);
							heightToRemove = secondLargestHeight - largestHeight;
						}
					}

					// Double-check that the height to remove is >= than the equal distribution of the height among expanding children.
					heightToRemove = max(heightToRemove, remainingHeight / shrinkableChildIndexCopy.size());

					sint32 index = static_cast<sint32>(shrinkableChildIndexCopy.size()) - 1;

					while (index >= 0) {
						const uint32 childIndex = shrinkableChildIndexCopy[index];
						GUIContainer& child = components[childIndex];

						const float32 previousHeight = child.size.height.value;

						if (child.size.height.value == largestHeight) {
							child.size.height.value += heightToRemove;

							if (child.size.height.value < child.minimumSize.y) {
								child.size.height.value = child.minimumSize.y;

								shrinkableChildIndexCopy.erase(shrinkableChildIndexCopy.begin() + index);
							}

							remainingHeight -= child.size.height.value - previousHeight;
						}

						index -= 1;
					}
				}
			}
		}
		else {
			for (const uint32 childIndex : growableChildIndices) {
				GUIContainer& child = components[childIndex];
				child.size.height.value = parent.size.height.value - parent.padding.y * 2.0f;
			}
		}

		for (const uint32 childIndex : parent.childIndices) {
			if (!components[childIndex].textComponent) {
				GrowAndShrinkSizingHeightsRecursive(childIndex);
			}
		}
	}

	void GUILayer::PositionsAndAlignment(GUIContainer& component, svec2<float32> origin) {
		const svec2<float32> previousOrigin = origin;

		component.origin = origin;

		if (component.floating) {
			component.origin += component.offset;
		}

		if (component.childIndices.size() == 0) {
			const GUIContainer& parent = components[component.parentIndex];

			// Skip over the component towards the layout direction (this includes the padding).
			origin[parent.layoutDirection] += component.size[parent.layoutDirection].value;

			return;
		}

		// Add top/left padding.
		origin.x += component.padding.x;
		origin.y += component.padding.y;

		svec2<float32> remainingSize = {component.size.width.value - component.padding.x * 2.0f, component.size.height.value - component.padding.y * 2.0f};
		float32 remainingAcross = 0.0f;

		for (const uint32 childIndex : component.childIndices) {
			const GUIContainer& child = components[childIndex];

			remainingSize[component.layoutDirection] -= child.size[component.layoutDirection].value;
			remainingAcross = max(remainingAcross, child.size[!component.layoutDirection].value);
		}

		remainingSize[component.layoutDirection] -= component.layoutSpacing * (static_cast<uint32>(component.childIndices.size()) - 1);
		remainingSize[!component.layoutDirection] -= remainingAcross;

		// Snap to anchor point along layout direction.
		const uint8 shiftAmountAlong = 4 - component.layoutDirection * 3;
		const uint8 maskAlong = 0b111000 >> component.layoutDirection * 3;
		const uint8 anchorRangeAlong = (static_cast<uint8>(component.anchorPoint) & maskAlong) >> shiftAmountAlong;
		const float32 fAlong = static_cast<float32>(anchorRangeAlong) * 0.5f;

		origin[component.layoutDirection] += remainingSize[component.layoutDirection] * fAlong;

		// Snap to anchor point across layout direction.
		const uint8 shiftAmountAcross = 1 + component.layoutDirection * 3;
		const uint8 maskAcross = 0b111000 >> (!component.layoutDirection) * 3;
		const uint8 anchorRangeAcross = (static_cast<uint8>(component.anchorPoint) & maskAcross) >> shiftAmountAcross;
		const float32 fAcross = static_cast<float32>(anchorRangeAcross) * 0.5f;
		const float32 componentAcross = component.size[!component.layoutDirection].value - component.padding[!component.layoutDirection] * 2.0f;
		const float32 originAcross = origin[!component.layoutDirection];

		index index = 0;

		while (index < component.childIndices.size()) {
			GUIContainer& child = components[component.childIndices[index]];

			if (!child.floating) {
				const float32 remainingAcross = componentAcross - child.size[!component.layoutDirection].value;

				origin[!component.layoutDirection] = originAcross + remainingAcross * fAcross;
			}

			PositionsAndAlignment(child, origin);

			if (!child.floating) {
				origin[component.layoutDirection] += child.size[component.layoutDirection].value;
			}

			if (index + 1 == component.childIndices.size()) {
				break;
			}

			// Add layout spacing (only if it's not the last child).
			origin[component.layoutDirection] += component.layoutSpacing;

			index += 1;
		}

		// Add padding along the layout direction.
		origin[component.layoutDirection] += component.padding[component.layoutDirection];

		// Remove padding across the layout direction.
		origin[!component.layoutDirection] -= component.padding[!component.layoutDirection];
	}
}