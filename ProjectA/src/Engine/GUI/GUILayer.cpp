#include "pch.hpp"

#include "Engine/API/Event/EventDispatcher.hpp"
#include "Engine/Application/Application.hpp"
#include "Engine/Filesystem/Decoder/Decoder.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/GUI/GUILayer.hpp"

namespace ProjectA {
	void GUILayer::build() {
		renderer.build();

		TextureArrayLayer defaultImageLayer;
		defaultImageLayer.width = 1;
		defaultImageLayer.height = 1;
		defaultImageLayer.data = {255, 255, 255, 255};

		renderer.registerTextureArrayLayer(defaultImageLayer);

		registerAssets();
	}

	void GUILayer::postConstruct() {
		renderer.flushTextureArrayStagingBuffer();

		textureArrayFlushed = true;
	}

	FontID GUILayer::loadFont(std::string_view path) {
		ENGINE_ASSERT(fontCount < MAX_FONT_COUNT, "Too many fonts loaded.");
		ENGINE_ASSERT(!textureArrayFlushed, "Images can only be added when constructing the layer.");

		/* defaultFont->textureIndex = static_cast<uint32>(images.size());
		defaultFont->glyphMap.map = QUIVER_BITMAP_GLYPH_MAP;
		defaultFont->glyphMap.base = 32;
		defaultFont->glyphMap.count = 220;
		defaultFont->tileHeight = 12;
		defaultFont->tileSpacing = 1; */

		fonts[fontCount].font = createRef<TrueTypeFont>(path);

		const SDFGlyphMap& glyphMap = fonts[fontCount].font->getGlyphMap();

		TextureArrayLayer glyphMapImageLayer;
		glyphMapImageLayer.width = glyphMap.extent.width;
		glyphMapImageLayer.height = glyphMap.extent.height;
		glyphMapImageLayer.data = sequence<uint8>(glyphMap.data.count * 4);

		// Map R to RGBA.
		for (uint32 texelIndex = 0; texelIndex < glyphMap.data.count; texelIndex += 1) {
			glyphMapImageLayer.data[texelIndex * 4 + 0] = glyphMap.data[texelIndex];
			glyphMapImageLayer.data[texelIndex * 4 + 1] = 0;
			glyphMapImageLayer.data[texelIndex * 4 + 2] = 0;
			glyphMapImageLayer.data[texelIndex * 4 + 3] = 255;
		}

		const FontID fontId = fontCount;

		fonts[fontCount].glyphMapImageId = renderer.registerTextureArrayLayer(glyphMapImageLayer);

		fontCount += 1;

		return fontId;
	}

	ImageID GUILayer::loadImage(std::string_view path) {
		ENGINE_ASSERT(!textureArrayFlushed, "Images can only be added when constructing the layer.");

		const Stream stream = Filesystem::openStream(path);
		const File<FileFormat::PNG> file = Filesystem::decode<FileFormat::PNG>(stream);

		TextureArrayLayer imageLayer;
		imageLayer.width = file.extent.width;
		imageLayer.height = file.extent.height;
		imageLayer.data = file.data;

		return renderer.registerTextureArrayLayer(imageLayer);
	}

	void GUILayer::preRender() {
		components.clear();

		DEBUG_ASSERT(openedComponents.empty());

		GUI();

		// 7. Register components for rendering.
		for (const GUIContainer& component : components) {
			if (component.textComponent) {
				// TODO: Register individual text characters.

				Size2D size = component.size;
				size.height.value = 100_px;

				registerSubcomponent(component.origin, size, DEFAULT_IMAGE_INDEX, 0xff9800ff, 0.0f);
			}
			else {
				// Validate radius.
				// TODO: lyah::min
				const float32 minHalfDimension = std::min(component.size.width.value, component.size.height.value) / 2.0f;

				ENGINE_ASSERT(component.radius >= 0.0f && component.radius <= minHalfDimension, "Invalid radius.");

				registerSubcomponent(component.origin, component.size, component.image, component.color, component.radius);
			}
		}
	}

	void GUILayer::render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const {
		renderer.render(commandEncoder);
	}

	void GUILayer::postRender() {
		renderer.resetCounters();
	}

	void GUILayer::onEvent(Event& event) {
		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));
	}

	bool GUILayer::onWindowResize(WindowResizeEvent& event) {
		renderer.resize(event);

		return true;
	}

	void GUILayer::openComponent(const ContainerStyle& style) {
		GUIContainer* parent = nullptr;

		GUIContainer component;
		component.parentIndex = ~0;

		if (!openedComponents.empty()) {
			component.parentIndex = openedComponents.back();

			parent = &components[component.parentIndex];
		}

		// TODO: Move these asserts after closing the component.
		// ENGINE_ASSERT(parent == nullptr || (parent->size.width.fillMode == FillMode::FIT && style.size.width.fillMode == FillMode::EXPAND), "An expanding component cannot be a child of a fit component in the same direction.");
		// ENGINE_ASSERT(parent == nullptr || (parent->size.height.fillMode == FillMode::FIT && style.size.height.fillMode == FillMode::EXPAND), "An expanding component cannot be a child of a fit component in the same direction.");

		const uint32 componentIndex = static_cast<uint32>(components.size());

		if (parent != nullptr) {
			component.origin = components[component.parentIndex].origin;

			parent->childIndices.emplace_back(componentIndex);
		}
		else {
			component.origin = {0.0f, 0.0f};
		}

		component.size = style.size;
		component.remainingSize = component.size;
		component.padding = style.padding;
		component.layoutDirection = style.layoutDirection;
		component.layoutSpacing = style.layoutSpacing;
		component.color = style.color;
		component.image = style.image;
		component.radius = style.radius;
		component.textComponent = false;

		openedComponents.emplace_back(componentIndex);
		components.emplace_back(component);
	}

	void GUILayer::GUIText(const std::string& string, const TextStyle& textStyle) {
		const uint32 componentIndex = static_cast<uint32>(components.size());

		GUIContainer textComponent;

		if (!openedComponents.empty()) {
			textComponent.parentIndex = openedComponents.back();
			textComponent.origin = components[openedComponents.back()].origin;

			GUIContainer& parent = components[textComponent.parentIndex];

			parent.childIndices.emplace_back(componentIndex);
		}
		else {
			textComponent.parentIndex = ~0;
			textComponent.origin = {0.0f, 0.0f};
		}

		const float32 preferredWidth = static_cast<float32>(calculateTextPreferredWidth(string, textStyle));
		const float32 minimumWidth = static_cast<float32>(calculateTextMinimumWidth(string, textStyle));

		textComponent.size = {preferredWidth, 0_px};
		textComponent.padding = {0_px, 0_px};
		textComponent.layoutDirection = HORIZONTAL;
		textComponent.layoutSpacing = 0_px;
		textComponent.color = textStyle.color;
		textComponent.image = fonts[textStyle.font].glyphMapImageId;
		textComponent.radius = 0.0f;
		textComponent.textComponent = true;
		textComponent.nullTerminatedText = string.c_str();
		textComponent.preferredSize = {preferredWidth, 0_px};
		textComponent.minimumSize = {minimumWidth, 0_px};
		textComponent.font = textStyle.font;
		textComponent.fontSize = textStyle.fontSize;

		components.emplace_back(textComponent);

		/* const FontData& fontData = fonts[textStyle.font];
		const TrueTypeFont& font = *fontData.font;
		const Extent2D imageExtent = renderer.getTextureArrayLayerExtent(fontData.glyphMapImageId);
		const float32 lineHeight = static_cast<float32>(font.getLineHeight());
		const float32 scaledHeight = 1.0f / lineHeight * textStyle.fontSize;

		float32 positionX = static_cast<float32>(origin[0]);
		float32 positionY = static_cast<float32>(origin[1]);

		float32 width = 0;
		float32 height = textStyle.fontSize;

		GUIGlyphSubcomponent glyphSubcomponent;
		glyphSubcomponent.layerIndex = fontData.glyphMapImageId;
		glyphSubcomponent.color = textStyle.color.value;

		for (const char8 character : string) {
			if (character < 0) {
				// TODO: Handle UTF-16 characters.

				continue;
			}

			if (character == '\n') {
				positionX = static_cast<float32>(origin[0]);
				positionY += textStyle.fontSize;
				height += textStyle.fontSize;

				continue;
			}

			// if (positionX > currentContainerInnerSize.width) {
				// positionX = static_cast<float32>(componentOrigin[0]);
				// positionY += textStyle.fontSize;
				// height += textStyle.fontSize;
			// }

			// if (positionY > currentContainerInnerSize.height) {
				// ENGINE_WARN("A text component is overflowing the current container.");
			// }

			const uint16 codePoint = static_cast<uint16>(character);
			const Glyph& glyph = font[codePoint];

			// Don't register spaces on the start of the text.
			if (glyph.invisible && positionX == static_cast<float32>(origin[0])) {
				continue;
			}

			const float32 advanceWidth = std::ceil(glyph.advanceWidth * scaledHeight);

			if (!glyph.invisible) {
				// For a line height (ascent - descent) of 1210 FUnits,
				// the 'A' glyph's width (not including bearing or advance) is 631 FUnits.
				// The width should be 62.13_px.
				// TODO: float32 lyah::ceil(float32)
				const float32 width = glyph.width * scaledHeight;
				const float32 leftSideBearing = glyph.leftSideBearing * scaledHeight;

				const vec2<float32> position = {positionX + leftSideBearing, positionY};
				const vec2<float32> size = {width, textStyle.fontSize};
				const vec2<float32> uvStart = {glyph.minCoordinate.x, glyph.minCoordinate.y};
				const vec2<float32> uvEnd = {glyph.maxCoordinate.x, glyph.maxCoordinate.y};
				const vec2<float32> uvDiff = uvEnd - uvStart;

				glyphSubcomponent.world = mat3x3<float32>::scaling(size) * mat3x3<float32>::translation(position);
				glyphSubcomponent.textureWorld = mat3x3<float32>::scaling({
					uvDiff[0] * imageExtent.width / renderer.TEXTURE_ARRAY_MAX_WIDTH,
					uvDiff[1] * imageExtent.height / renderer.TEXTURE_ARRAY_MAX_HEIGHT,
				}) * mat3x3<float32>::translation(uvStart);

				renderer.registerGlyphSubcomponent(glyphSubcomponent);
			}

			positionX += advanceWidth;
			width += advanceWidth;
		}

		const Extent2D computedSize = {static_cast<uint32>(width), static_cast<uint32>(height)};

		origin[layoutDirection] += (pointer_cast<const uint32*>(&computedSize))[layoutDirection]; */
	}

	void GUILayer::closeComponent() {
		DEBUG_ASSERT(!openedComponents.empty());

		GUIContainer& component = components[openedComponents.back()];

		component.size.width.value += component.padding.x * 2.0f;
		component.size.height.value += component.padding.y * 2.0f;

		const sint32 m = static_cast<sint32>(component.childIndices.size());

		// TODO: lyah::max
		const float32 layoutSpacing = component.layoutSpacing * static_cast<float32>(std::max(0, m - 1));

		component.size[component.layoutDirection].value += layoutSpacing;

		if (component.parentIndex != ~0) {
			GUIContainer& parent = components[component.parentIndex];

			// Instead of calculating the size of the current "fit" component by summing the size of its children,
			// take advantage of the macro's open-close loop and only add the size of the current component to the "fit" parent.
			// To avoid the null parent check, we could create a root component before lending the control to the user.
			// That way all user-defined components will have a parent.

			// 1. Update parent fit width.
			if (parent.size.width.fillMode == FillMode::FIT) {
				DEBUG_ASSERT(component.size.width.value > 0.0f);

				if (parent.layoutDirection == HORIZONTAL) {
					parent.size.width.value += component.size.width.value;
					parent.minimumSize.x += component.minimumSize.x;
				}
				else {
					// TODO: lyah::max
					parent.size.width.value = std::max(parent.size.width.value, component.size.width.value);
					parent.minimumSize.x = std::max(parent.minimumSize.x, component.minimumSize.x);
				}
			}

			if (component.size.width.fillMode == FillMode::FIXED) {
				parent.remainingSize.x -= component.size.width;
			}

			if (component.size.height.fillMode == FillMode::FIXED) {
				parent.remainingSize.y -= component.size.height;
			}
		}

		// 2. Grow & Shrink Sizing Widths
		{
			// Calculate width to be distributed among expanding children.
			float32 remainingWidth = component.size.width.value - component.padding.x * 2.0f;

			if (component.layoutDirection == HORIZONTAL) {
				remainingWidth -= (component.childIndices.size() - 1) * component.layoutSpacing;

				for (uint32 index = 0; index < component.childIndices.size(); index += 1) {
					const uint32 childIndex = component.childIndices[index];
					const GUIContainer& child = components[childIndex];

					remainingWidth -= child.size.width.value;
				}
			}

			std::vector<uint32> widthExpandChildIndices;

			for (uint32 index = 0; index < component.childIndices.size(); index += 1) {
				const uint32 childIndex = component.childIndices[index];
				const GUIContainer& child = components[childIndex];

				if (child.size.width.fillMode == FillMode::FIT || child.size.width.fillMode == FillMode::EXPAND) {
					widthExpandChildIndices.emplace_back(childIndex);
				}
			}

			if (!widthExpandChildIndices.empty()) {
				// BREAKPOINT();

				if (component.layoutDirection == HORIZONTAL) {
					// Expand components.
					while (remainingWidth > 0.0f) {
						float32 smallestWidth = components[widthExpandChildIndices[0]].size.width.value;
						float32 secondSmallestWidth = lyah::infinity<float32>();
						float32 widthToAdd = remainingWidth;

						for (uint32 index = 1; index < widthExpandChildIndices.size(); index += 1) {
							const uint32 childIndex = widthExpandChildIndices[index];
							const GUIContainer& child = components[childIndex];
							const float32 width = child.size.width.value;

							// If we have found a child smaller than the smallest width registered, replace it with the child's width.
							if (width < smallestWidth) {
								secondSmallestWidth = smallestWidth;
								smallestWidth = width;
							}

							// This child not the smallest.
							if (width > smallestWidth) {
								// TODO: lyah::min
								secondSmallestWidth = std::min(secondSmallestWidth, width);

								// Calculate difference between the smallest and second smallest widths.
								widthToAdd = secondSmallestWidth - smallestWidth;
							}
						}

						// Double-check that the width to add is <= than the equal distribution of the width among expanding children.
						// TODO: lyah::min
						widthToAdd = std::min(widthToAdd, remainingWidth / widthExpandChildIndices.size());

						// Add the width to each expanding child.
						for (uint32 index = 0; index < widthExpandChildIndices.size(); index += 1) {
							const uint32 childIndex = widthExpandChildIndices[index];
							GUIContainer& child = components[childIndex];

							if (child.size.width.value == smallestWidth) {
								// Resize the child to reach the second smallest width, then repeat.
								child.size.width.value += widthToAdd;

								remainingWidth -= widthToAdd;
							}
						}
					}

					// Shrink components.
					while (remainingWidth < 0.0f) {
						float32 largestWidth = components[widthExpandChildIndices[0]].size.width.value;
						float32 secondLargestWidth = 0.0f;
						float32 widthToRemove = remainingWidth;

						for (uint32 index = 1; index < widthExpandChildIndices.size(); index += 1) {
							const uint32 childIndex = widthExpandChildIndices[index];
							const GUIContainer& child = components[childIndex];
							const float32 width = child.size.width.value;

							// If we have found a child larger than the largest width registered, replace it with the child's width.
							if (width > largestWidth) {
								secondLargestWidth = largestWidth;
								largestWidth = width;
							}

							// This child is not the largest.
							if (width < largestWidth) {
								// TODO: lyah::max
								secondLargestWidth = std::max(secondLargestWidth, width);

								// Calculate difference between the largest and second largest widths.
								widthToRemove = secondLargestWidth - largestWidth;
							}
						}

						// Double-check that the width to remove is >= than the equal distribution of the width among expanding children.
						// TODO: lyah::max
						widthToRemove = std::max(widthToRemove, remainingWidth / widthExpandChildIndices.size());

						// Remove the width from each expanding child.
						for (uint32 index = 0; index < widthExpandChildIndices.size(); index += 1) {
							const uint32 childIndex = widthExpandChildIndices[index];
							GUIContainer& child = components[childIndex];

							if (child.size.width.value == largestWidth) {
								// Resize the child to reach the second largest width, then repeat.
								child.size.width.value += widthToRemove;
								remainingWidth -= widthToRemove;
							}
						}
					}
				}
				else {
					for (uint32 index = 0; index < widthExpandChildIndices.size(); index += 1) {
						const uint32 childIndex = widthExpandChildIndices[index];
						GUIContainer& child = components[childIndex];

						child.size.width.value = remainingWidth;
					}
				}
			}
		}

		// 3. Wrap Text
		// TODO

		// 4. Update parent fit height.
		if (component.parentIndex != ~0) {
			GUIContainer& parent = components[component.parentIndex];

			if (parent.size.height.fillMode == FillMode::FIT) {
				if (parent.layoutDirection == VERTICAL) {
					parent.size.height.value += component.size.height.value;
					parent.minimumSize.y += component.minimumSize.y;
				}
				else {
					// TODO: lyah::max
					parent.size.height.value = std::max(parent.size.height.value, component.size.height.value);
					parent.minimumSize.y = std::max(parent.minimumSize.y, component.minimumSize.y);
				}
			}
		}

		// 5. Grow & Shrink Sizing Heights
		{
			// Calculate height to be distributed among expanding children.
			float32 remainingHeight = component.size.height.value - component.padding.y * 2.0f;

			if (component.layoutDirection == VERTICAL) {					
				remainingHeight -= (component.childIndices.size() - 1) * component.layoutSpacing;

				for (uint32 index = 0; index < component.childIndices.size(); index += 1) {
					const uint32 childIndex = component.childIndices[index];
					const GUIContainer& child = components[childIndex];

					remainingHeight -= child.size.height.value;
				}
			}

			std::vector<uint32> heightExpandChildIndices;

			for (uint32 index = 0; index < component.childIndices.size(); index += 1) {
				const uint32 childIndex = component.childIndices[index];
				const GUIContainer& child = components[childIndex];

				if (child.size.height.fillMode == FillMode::FIT || child.size.height.fillMode == FillMode::EXPAND) {
					heightExpandChildIndices.emplace_back(childIndex);
				}
			}

			if (!heightExpandChildIndices.empty()) {
				if (component.layoutDirection == VERTICAL) {
					// Expand components.
					while (remainingHeight > 0.0f) {
						float32 smallestHeight = components[heightExpandChildIndices[0]].size.height.value;
						float32 secondSmallestHeight = lyah::infinity<float32>();
						float32 heightToAdd = remainingHeight;

						for (uint32 index = 1; index < heightExpandChildIndices.size(); index += 1) {
							const uint32 childIndex = heightExpandChildIndices[index];
							const GUIContainer& child = components[childIndex];
							const float32 height = child.size.height.value;

							// If we have found a child smaller than the smallest height registered, replace it with the child's height.
							if (height < smallestHeight) {
								secondSmallestHeight = smallestHeight;
								smallestHeight = height;
							}

							// This child is larger.
							if (height > smallestHeight) {
								// TODO: lyah::min
								secondSmallestHeight = std::min(secondSmallestHeight, height);

								// Calculate difference between the smallest and second smallest heights.
								heightToAdd = secondSmallestHeight - smallestHeight;
							}
						}

						// Double-check that the height to add is smaller than the equal distribution of the height among expanding children.
						// TODO: lyah::min
						heightToAdd = std::min(heightToAdd, remainingHeight / heightExpandChildIndices.size());

						// Add the height to each expanding child.
						for (uint32 index = 0; index < heightExpandChildIndices.size(); index += 1) {
							const uint32 childIndex = heightExpandChildIndices[index];
							GUIContainer& child = components[childIndex];

							if (child.size.height.value == smallestHeight) {
								// Resize the child to reach the second smallest height, then repeat.
								child.size.height.value += heightToAdd;

								remainingHeight -= heightToAdd;
							}
						}
					}

					// Shrink components.
					while (remainingHeight < 0.0f) {
						float32 largestHeight = components[heightExpandChildIndices[0]].size.height.value;
						float32 secondLargestHeight = 0.0f;
						float32 heightToRemove = remainingHeight;

						for (uint32 index = 1; index < heightExpandChildIndices.size(); index += 1) {
							const uint32 childIndex = heightExpandChildIndices[index];
							const GUIContainer& child = components[childIndex];
							const float32 height = child.size.height.value;

							// If we have found a child larger than the largest height registered, replace it with the child's height.
							if (height > largestHeight) {
								secondLargestHeight = largestHeight;
								largestHeight = height;
							}

							// This child is not the largest.
							if (height < largestHeight) {
								// TODO: lyah::max
								secondLargestHeight = std::max(secondLargestHeight, height);

								// Calculate difference between the largest and second largest heights.
								heightToRemove = secondLargestHeight - largestHeight;
							}
						}

						// Double-check that the height to remove is >= than the equal distribution of the height among expanding children.
						// TODO: lyah::max
						heightToRemove = std::max(heightToRemove, remainingHeight / heightExpandChildIndices.size());

						// Remove the height from each expanding child.
						for (uint32 index = 0; index < heightExpandChildIndices.size(); index += 1) {
							const uint32 childIndex = heightExpandChildIndices[index];
							GUIContainer& child = components[childIndex];

							if (child.size.height.value == largestHeight) {
								// Resize the child to reach the second largest height, then repeat.
								child.size.height.value += heightToRemove;
								remainingHeight -= heightToRemove;
							}
						}
					}
				}
				else {
					for (uint32 index = 0; index < heightExpandChildIndices.size(); index += 1) {
						const uint32 childIndex = heightExpandChildIndices[index];
						GUIContainer& child = components[childIndex];

						child.size.height.value = remainingHeight;
					}
				}
			}
		}

		// 6. Positioning
		{
			svec2<float32> origin = component.origin;
			origin.x += component.padding.x;
			origin.y += component.padding.y;

			for (uint32 index = 0; index < component.childIndices.size(); index += 1) {
				const uint32 childIndex = component.childIndices[index];
				GUIContainer& child = components[childIndex];

				child.origin = origin;

				origin[component.layoutDirection] += child.size[component.layoutDirection].value + component.layoutSpacing;
			}
		}

		openedComponents.pop_back();
	}

	void GUILayer::registerSubcomponent(svec2<float32> origin, const Size2D& size, ImageID imageId, const Color& color, float32 radius) {
		const vec2<float32> decimalOrigin = {origin[0], origin[1]};
		const vec2<float32> decimalSize = {size.width.value, size.height.value};
		const Extent2D imageExtent = renderer.getTextureArrayLayerExtent(imageId);

		GUISubcomponent subcomponent;
		subcomponent.minBounds = {decimalOrigin[0] + radius, decimalOrigin[1] + radius};
		subcomponent.maxBounds = {decimalOrigin[0] + decimalSize[0] - radius, decimalOrigin[1] + decimalSize[1] - radius};
		subcomponent.worldTransform = mat3x3<float32>::scaling(decimalSize) * mat3x3<float32>::translation(decimalOrigin);
		subcomponent.textureTransform = mat3x3<float32>::scaling({
			static_cast<float32>(imageExtent.width) / renderer.TEXTURE_ARRAY_MAX_WIDTH,
			static_cast<float32>(imageExtent.height) / renderer.TEXTURE_ARRAY_MAX_HEIGHT,
		});
		subcomponent.layerIndex = imageId;
		subcomponent.color = color.value;
		subcomponent.radius = radius;

		renderer.registerSubcomponent(subcomponent);
	}

	bool GUILayer::intersects(Extent2D origin, Extent2D size, Extent2D point) {
		return (
			point.width >= origin.width &&
			point.width < origin.width + size.width &&
			point.height >= origin.height &&
			point.height < origin.height + size.height
		);
	}

	uint32 GUILayer::calculateTextPreferredWidth(const std::string& string, const TextStyle& textStyle) const {
		const TrueTypeFont& font = *fonts[textStyle.font].font;

		uint32 preferredWidth = 0;
		uint32 lineWidth = 0;

		bool firstLineGlyph = true;

		for (const char8 character : string) {
			if (character < 0) {
				// TODO: Handle UTF-16 characters.

				continue;
			}

			if (character == '\n') {
				// TODO: lyah::max
				preferredWidth = std::max(preferredWidth, lineWidth);

				lineWidth = 0;
				firstLineGlyph = true;

				continue;
			}

			const uint16 codePoint = static_cast<uint16>(character);
			const Glyph& glyph = font[codePoint];

			if (firstLineGlyph) {
				firstLineGlyph = false;

				if (glyph.invisible) {
					continue;
				}
			}

			const float32 advanceWidth = std::ceil(glyph.advanceWidth / static_cast<float32>(font.getLineHeight()) * textStyle.fontSize);

			lineWidth += static_cast<uint32>(advanceWidth);
		}

		// TODO: lyah::max
		preferredWidth = std::max(preferredWidth, lineWidth);

		return preferredWidth;
	}

	uint32 GUILayer::calculateTextMinimumWidth(const std::string& string, const TextStyle& textStyle) const {
		const TrueTypeFont& font = *fonts[textStyle.font].font;

		uint32 minimumWidth = 0;
		uint32 wordWidth = 0;

		bool firstLineGlyph = true;

		for (const char8 character : string) {
			if (character < 0) {
				// TODO: Handle UTF-16 characters.

				continue;
			}

			if (character == '\n') {
				// TODO: lyah::max
				minimumWidth = std::max(minimumWidth, wordWidth);

				wordWidth = 0;

				break;
			}

			const uint16 codePoint = static_cast<uint16>(character);
			const Glyph& glyph = font[codePoint];

			if (firstLineGlyph) {
				firstLineGlyph = false;

				if (glyph.invisible) {
					continue;
				}
			}

			if (glyph.invisible) {
				// TODO: lyah::max
				minimumWidth = std::max(minimumWidth, wordWidth);

				wordWidth = 0;

				continue;
			}

			const float32 advanceWidth = std::ceil(glyph.advanceWidth / static_cast<float32>(font.getLineHeight()) * textStyle.fontSize);

			wordWidth += static_cast<uint32>(advanceWidth);
		}

		// TODO: lyah::max
		minimumWidth = std::max(minimumWidth, wordWidth);

		return minimumWidth;
	}
}