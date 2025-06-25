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
		const ref<Swapchain>& swapchain = Renderer::getSwapchain();

		/* ContainerStyle root;
		root.size = swapchain->getExtent();
		root.padding = {0px, 0px};
		root.layoutDirection = VERTICAL;
		root.layoutSpacing = 0px;
		root.color = 0xffffffff; */

		components.clear();

		DEBUG_ASSERT(openedComponents.empty());

		GUI();
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

	void GUILayer::openContainer(const ContainerStyle& style) {
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
			component.origin = style.padding;
		}

		component.size = style.size;
		component.padding = style.padding;
		component.layoutDirection = style.layoutDirection;
		component.layoutSpacing = style.layoutSpacing;
		component.color = style.color;

		openedComponents.emplace_back(componentIndex);
		components.emplace_back(component);
	}

	void GUILayer::closeContainer() {
		DEBUG_ASSERT(!openedComponents.empty());

		GUIContainer& component = components[openedComponents.back()];

		Extent2D origin = {0px, 0px};

		if (component.parentIndex != ~0) {
			GUIContainer& parent = components[component.parentIndex];

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

			// TODO: 2. Grow & Shrink Sizing Widths
			// TODO: 3. Wrap Text

			// 4. Update parent fit height.
			if (parent.size.height.fillMode == FillMode::FIT) {
				if (parent.layoutDirection == VERTICAL) {
					parent.size.height.value += component.size.height.value;
				}
				else {
					// TODO: lyah::max
					parent.size.height.value = std::max(parent.size.height.value, component.size.height.value);
				}
			}

			// BREAKPOINT();

			// TODO: 5. Grow & Shrink Sizing Heights

			// 6. Positions
			origin = parent.origin;

			parent.origin[parent.layoutDirection] += component.size[parent.layoutDirection];
		}

		// TODO
		{
			for (uint32 childIndex = 0; childIndex < component.childIndices.size(); childIndex += 1) {
				const GUIContainer& child = components[component.childIndices[childIndex]];

				// ENGINE_DEBUG("[] []", child.size.width, child.size.height);
			}
		}

		// 7. Register component for rendering.
		registerSubcomponent(origin, component.size, DEFAULT_IMAGE_INDEX, component.color, 0.0f);

		openedComponents.pop_back();

		// TODO: Remove parent children?
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

	/* void GUILayer::GUIText(const std::string& string, const TextStyle& textStyle) {
		const FontData& fontData = fonts[textStyle.font];
		const TrueTypeFont& font = *fontData.font;
		const Extent2D imageExtent = renderer.getTextureArrayLayerExtent(fontData.glyphMapImageId);
		const float32 lineHeight = static_cast<float32>(font.getLineHeight());

		const float32 scaledHeight = 1.0f / lineHeight * textStyle.fontSize;

		float32 positionX = static_cast<float32>(componentOrigin[0]);
		float32 positionY = static_cast<float32>(componentOrigin[1]);

		float32 width = 0;
		float32 height = textStyle.fontSize;

		GUIGlyphSubcomponent glyphSubcomponent;
		glyphSubcomponent.layerIndex = fontData.glyphMapImageId;
		glyphSubcomponent.color = textStyle.color;

		for (const char character : string) {
			if (character < 0) {
				// TODO: Handle UTF-16 characters.

				continue;
			}

			if (character == '\n') {
				positionX = static_cast<float32>(componentOrigin[0]);
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
			if (glyph.invisible && positionX == static_cast<float32>(componentOrigin[0])) {
				continue;
			}

			const float32 advanceWidth = std::ceil(glyph.advanceWidth * scaledHeight);

			if (!glyph.invisible) {
				// For a line height (ascent - descent) of 1210 FUnits,
				// the 'A' glyph's width (not including bearing or advance) is 631 FUnits.
				// The width should be 62.13px.
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

		componentOrigin[layoutDirection] += (pointer_cast_const<const uint32*>(&computedSize))[layoutDirection] + layoutSpacing;
	} */

	/* bool GUILayer::GUIButton(const std::string& string, const ButtonStyle& buttonStyle) {
		const Extent2D textExtent = calculateTextSize(string, *buttonStyle.textStyle);

		Extent2D size;
		size.width = textExtent.width + buttonStyle.padding.width * 2;
		size.height = textExtent.height + buttonStyle.padding.height * 2;

		registerSubcomponent(size, DEFAULT_IMAGE_INDEX, buttonStyle.backgroundColor, buttonStyle.radius);

		const uint32 componentOriginOppositeDirection = componentOrigin[!layoutDirection];

		componentOrigin[0] += buttonStyle.padding.width;
		componentOrigin[1] += buttonStyle.padding.height;

		GUIText(string, *buttonStyle.textStyle);

		componentOrigin[!layoutDirection] = componentOriginOppositeDirection;
		componentOrigin[layoutDirection] += buttonStyle.padding.height;

		// bool hovered = false;

		// if (intersects(
			// {componentOrigin[0], componentOrigin[1]},
			// size,
			// {static_cast<uint32>(Application::getWindow().getMousePosition()[0]), static_cast<uint32>(Application::getWindow().getMousePosition()[1])}
		// )) {
			// hovered = true;
		// }

		// 4. Advance the component origin by the size (towards the current layout direction).
		// componentOrigin[layoutDirection] += (pointer_cast_const<const uint32*>(&size))[layoutDirection] + layoutSpacing;

		// return hovered;
		return false;
	} */

	/* void GUILayer::GUISpacing(uint32 spacing) {
		componentOrigin[layoutDirection] += spacing;
	} */

	void GUILayer::registerSubcomponent(Extent2D origin, const Size2D& size, ImageID imageId, const Color& color, float32 radius) {
		const vec2<float32> decimalOrigin = {
			static_cast<float32>(origin[0]),
			static_cast<float32>(origin[1]),
		};
		const vec2<float32> decimalSize = {
			static_cast<float32>(size.width.value),
			static_cast<float32>(size.height.value),
		};
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