#include "pch.hpp"

#include "Engine/API/Event/EventDispatcher.hpp"
#include "Engine/Application.hpp"
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

		// 7. Register component for rendering.
		for (const GUIContainer& component : components) {
			registerSubcomponent(component.origin, component.size, component.image, component.color, component.radius);
		}

		// TODO: Register individual text characters.
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
		const uint32 componentIndex = static_cast<uint32>(components.size());

		GUIContainer component;

		if (!openedComponents.empty()) {
			component.parentIndex = openedComponents.back();
			component.origin = components[openedComponents.back()].origin;

			GUIContainer& parent = components[component.parentIndex];

			parent.childIndices.emplace_back(componentIndex);
		}
		else {
			component.parentIndex = ~0;
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

		openedComponents.emplace_back(componentIndex);
		components.emplace_back(component);
	}

	void GUILayer::GUIText(const std::string& string, const TextStyle& textStyle) {
		const uint32 componentIndex = static_cast<uint32>(textComponents.size());

		TextComponent textComponent;

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

		textComponent.size = {0_px, 0_px};
		textComponent.text = string.data();
		textComponent.characterCount = static_cast<uint32>(string.size());
		textComponent.color = textStyle.color;

		textComponents.emplace_back(textComponent);

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

		for (const char character : string) {
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

		origin[layoutDirection] += (pointer_cast_const<const uint32*>(&computedSize))[layoutDirection]; */
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

			if (component.size.width.fillMode == FillMode::FIXED) {
				parent.remainingSize.x -= component.size.width;
			}

			if (component.size.height.fillMode == FillMode::FIXED) {
				parent.remainingSize.y -= component.size.height;
			}

			// 1. Update parent fit width.
			if (parent.size.width.fillMode == FillMode::FIT) {
				if (parent.layoutDirection == HORIZONTAL) {
					parent.size.width.value += component.size.width.value;
				}
				else {
					// TODO: lyah::max
					parent.size.width.value = std::max(parent.size.width.value, component.size.width.value);
				}
			}
		}

		// 2. Grow & Shrink Sizing Widths
		{
			std::vector<uint32> widthExpandChildIndices;

			for (uint32 index = 0; index < component.childIndices.size(); index += 1) {
				const uint32 childIndex = component.childIndices[index];
				const GUIContainer& child = components[childIndex];

				if (child.size.width.fillMode == FillMode::EXPAND) {
					widthExpandChildIndices.emplace_back(childIndex);
				}
			}

			if (!widthExpandChildIndices.empty()) {
				if (component.layoutDirection == HORIZONTAL) {
					const float32 distributedWidth = component.remainingSize.x / static_cast<float32>(widthExpandChildIndices.size());

					for (uint32 index = 0; index < widthExpandChildIndices.size(); index += 1) {
						const uint32 childIndex = widthExpandChildIndices[index];
						GUIContainer& child = components[childIndex];

						child.size.width.value = distributedWidth;
					}
				}
				else {
					for (uint32 index = 0; index < widthExpandChildIndices.size(); index += 1) {
						const uint32 childIndex = widthExpandChildIndices[index];
						GUIContainer& child = components[childIndex];

						DEBUG_ASSERT(component.size.width.value >= component.padding.x * 2.0f);

						child.size.width.value = component.size.width.value - component.padding.x * 2.0f;
					}
				}
			}
		}

		// TODO: 3. Wrap Text

		// 4. Update parent fit height.
		if (component.parentIndex != ~0) {
			GUIContainer& parent = components[component.parentIndex];

			if (parent.size.height.fillMode == FillMode::FIT) {
				if (parent.layoutDirection == VERTICAL) {
					parent.size.height.value += component.size.height.value;
				}
				else {
					// TODO: lyah::max
					parent.size.height.value = std::max(parent.size.height.value, component.size.height.value);
				}
			}
		}

		// 5. Grow & Shrink Sizing Heights
		{
			std::vector<uint32> heightExpandChildIndices;

			for (uint32 index = 0; index < component.childIndices.size(); index += 1) {
				const uint32 childIndex = component.childIndices[index];
				const GUIContainer& child = components[childIndex];

				if (child.size.height.fillMode == FillMode::EXPAND) {
					heightExpandChildIndices.emplace_back(childIndex);
				}
			}

			if (!heightExpandChildIndices.empty()) {
				if (component.layoutDirection == VERTICAL) {
					const float32 distributedHeight = component.remainingSize.y / static_cast<float32>(heightExpandChildIndices.size());

					for (uint32 index = 0; index < heightExpandChildIndices.size(); index += 1) {
						const uint32 childIndex = heightExpandChildIndices[index];
						GUIContainer& child = components[childIndex];

						child.size.height.value = distributedHeight;
					}
				}
				else {
					for (uint32 index = 0; index < heightExpandChildIndices.size(); index += 1) {
						const uint32 childIndex = heightExpandChildIndices[index];
						GUIContainer& child = components[childIndex];

						DEBUG_ASSERT(component.size.height.value >= component.padding.y * 2.0f);

						child.size.height.value = component.size.height.value - component.padding.y * 2.0f;
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

	/* void GUILayer::GUIComponent(Extent2D size, const vec4<float32>& color, float32 radius) {
		const Extent2D fixedSize = computeFixedSize(size);

		registerSubcomponent(fixedSize, DEFAULT_IMAGE_INDEX, color, radius);

		componentOrigin[layoutDirection] += (pointer_cast_const<const uint32*>(&fixedSize))[layoutDirection] + layoutSpacing;
	} */

	/* void GUILayer::GUIImage(Extent2D size, ImageID imageId, float32 radius) {
		const Extent2D fixedSize = computeFixedSize(size);

		registerSubcomponent(fixedSize, imageId, 0xffffffff, radius);

		componentOrigin[layoutDirection] += (pointer_cast_const<const uint32*>(&fixedSize))[layoutDirection] + layoutSpacing;
	} */

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

	Extent2D GUILayer::calculateTextSize(const std::string& string, const TextStyle& textStyle) const {
		const TrueTypeFont& font = *fonts[textStyle.font].font;
		const sint32 lineHeight = font.getLineHeight();

		Extent2D extent;
		extent.width = 0;
		extent.height = 0;

		if (!string.empty()) {
			extent.height = static_cast<uint32>(textStyle.fontSize);
		}

		float32 advanceWidth;
		bool firstLineGlyph = true;

		for (const char character : string) {
			if (character < 0) {
				// TODO: Handle UTF-16 characters.

				continue;
			}

			if (character == '\n') {
				extent.height += static_cast<uint32>(textStyle.fontSize);
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

			advanceWidth = std::ceil(glyph.advanceWidth / 1210.0f * textStyle.fontSize);

			extent.width += static_cast<uint32>(advanceWidth);
		}

		return extent;
	}
}