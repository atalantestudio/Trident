#include "pch.hpp"

#include "Engine/API/Event/EventDispatcher.hpp"
#include "Engine/Application.hpp"
#include "Engine/Filesystem/File/PNGFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/GUI/GUILayer.hpp"

namespace ProjectA {
	GUILayer::GUILayer() {
		renderer.build();

		TextureArrayLayer defaultImageLayer;
		defaultImageLayer.width = 1;
		defaultImageLayer.height = 1;
		defaultImageLayer.data = {255, 255, 255, 255};

		renderer.registerTextureArrayLayer(defaultImageLayer);
	}

	void GUILayer::postConstruct() {
		renderer.flushTextureArrayStagingBuffer();

		textureArrayFlushed = true;
	}

	GUILayer::FontID GUILayer::loadFont(std::string_view path) {
		ENGINE_ASSERT(!textureArrayFlushed, "Images can only be added when constructing the layer.");

		// "../ProjectA/src/Asset/Font/test.ttf"
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

	GUILayer::ImageID GUILayer::loadImage(std::string_view path) {
		ENGINE_ASSERT(!textureArrayFlushed, "Images can only be added when constructing the layer.");

		const PNGFile file = Filesystem::openTemporary<PNGFile>(path);

		TextureArrayLayer imageLayer;
		imageLayer.width = file.width;
		imageLayer.height = file.height;
		imageLayer.data = file.data;

		return renderer.registerTextureArrayLayer(imageLayer);
	}

	void GUILayer::preRender() {
		const ref<Swapchain>& swapchain = Renderer::getSwapchain();

		componentOrigin[HORIZONTAL] = 0;
		componentOrigin[VERTICAL] = 0;
		layoutDirection = VERTICAL;
		layoutSpacing = 0;
		currentContainerSize = {
			static_cast<sint16>(swapchain->getExtent().width),
			static_cast<sint16>(swapchain->getExtent().height),
		};
		currentContainerInnerSize = currentContainerSize;

		GUI();
	}

	void GUILayer::render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const {
		renderer.render(commandEncoder);
	}

	void GUILayer::postRender() {
		renderer.resetCounters();
	}

	void GUILayer::GUI() {}

	void GUILayer::onEvent(Event& event) {
		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));
	}

	bool GUILayer::onWindowResize(WindowResizeEvent& event) {
		renderer.resize(event);

		return true;
	}

	GUIContainer GUILayer::pushContainer(GUISize size, GUISize padding, LayoutDirection layoutDirection, uint16 layoutSpacing) {
		// TODO: Ensure that the container isn't expanding towards the current layout direction.

		GUIContainer container;
		container.origin = {componentOrigin[0], componentOrigin[1]};
		container.padding = padding;
		container.size = computeFixedSize(size);
		container.innerSize = container.size;
		container.innerSize.width -= padding.width * 2;
		container.innerSize.height -= padding.height * 2;
		container.parentSize = currentContainerSize;
		container.parentInnerSize = currentContainerInnerSize;
		container.root = rootComponent;
		container.parentLayoutDirection = this->layoutDirection;
		container.parentLayoutSpacing = this->layoutSpacing;

		// Add container padding to the origin.
		componentOrigin[HORIZONTAL] += padding.width;
		componentOrigin[VERTICAL] += padding.height;

		currentContainerSize = container.size;
		currentContainerInnerSize = container.innerSize;

		// Set layout direction.
		this->layoutDirection = layoutDirection;

		// Set layout spacing (dependent of the current layout direction).
		this->layoutSpacing = layoutSpacing;

		rootComponent = false;

		return container;
	}

	GUIContainer GUILayer::pushContainer(GUISize size, GUISize padding, LayoutDirection layoutDirection, uint16 layoutSpacing, const vec4<float32>& color) {
		const GUIContainer container = pushContainer(size, padding, layoutDirection, layoutSpacing);
		const GUISize fixedSize = computeFixedSize(container.size);

		registerComponent(fixedSize, DEFAULT_IMAGE_ID, color);

		return container;
	}

	void GUILayer::popContainer(GUIContainer& container) {
		container.iteration = true;

		layoutDirection = container.parentLayoutDirection;
		layoutSpacing = container.parentLayoutSpacing;

		if (container.root) {
			rootComponent = true;

			componentOrigin[HORIZONTAL] = container.padding.width;
			componentOrigin[VERTICAL] -= container.padding.height;
		}

		// Set the component origin to the end of the popped container.
		componentOrigin[layoutDirection] = container.origin[layoutDirection] + container.size[layoutDirection] + layoutSpacing;
		componentOrigin[!layoutDirection] = container.origin[!layoutDirection];

		// Set the current container sizes to the parent container sizes.
		currentContainerSize = container.parentSize;
		currentContainerInnerSize = container.parentInnerSize;
	}

	void GUILayer::registerComponent(GUISize size, ImageID imageId, const vec4<float32>& color) {
		const vec2<float32> sizeFloat = {
			static_cast<float32>(size[0]),
			static_cast<float32>(size[1]),
		};
		const vec2<float32> originFloat = {
			static_cast<float32>(componentOrigin[0]),
			static_cast<float32>(componentOrigin[1]),
		};
		const mat3x3<float32> transform = mat3x3<float32>::scaling(sizeFloat) * mat3x3<float32>::translation(originFloat);

		const Extent2D imageExtent = renderer.getTextureArrayLayerExtent(imageId);

		// TODO: Fix wrong first image column due to image transform precision error.
		const mat3x3<float32> imageTransform = mat3x3<float32>::scaling({
			imageExtent.width / static_cast<float32>(renderer.TEXTURE_ARRAY_MAX_WIDTH),
			imageExtent.height / static_cast<float32>(renderer.TEXTURE_ARRAY_MAX_HEIGHT),
		});

		renderer.registerSubcomponent(transform, imageTransform, imageId, color);
	}

	void GUILayer::GUIComponent(GUISize size, const vec4<float32>& color) {
		return GUIComponent(size, DEFAULT_IMAGE_ID, color);
	}

	void GUILayer::GUIComponent(GUISize size, ImageID imageId, const vec4<float32>& color) {
		const GUISize fixedSize = computeFixedSize(size);

		registerComponent(fixedSize, imageId, color);

		componentOrigin[layoutDirection] += fixedSize[layoutDirection] + layoutSpacing;
	}

	void GUILayer::GUIText(const std::string& string, const TextStyle& textStyle) {
		constexpr FontID fontId = 0;
		const TrueTypeFont& font = *fonts[fontId].font;
		const uint32 glyphMapImageId = fonts[fontId].glyphMapImageId;
		const Extent2D imageExtent = renderer.getTextureArrayLayerExtent(glyphMapImageId);
		const float32 lineHeight = static_cast<float32>(font.getLineHeight());

		float32 positionX = componentOrigin[0];
		float32 positionY = componentOrigin[1];

		float32 width = 0;
		const float32 height = textStyle.fontSize;

		for (const char glyph : string) {
			const uint16 codePoint = static_cast<uint16>(glyph);
			const TrueTypeGlyph& glyph = font[codePoint];
			const float32 advanceWidth = std::ceil(glyph.advanceWidth / 1210.0f * textStyle.fontSize);

			if (!glyph.invisible) {
				// For a line height (ascent - descent) of 1210 FUnits,
				// the 'A' glyph's width (not including bearing or advance) is 631 FUnits.
				// The width should be 62.13px.
				// TODO: float32 lyah::ceil(float32)
				const float32 width = glyph.width / lineHeight * textStyle.fontSize;
				const float32 leftSideBearing = glyph.leftSideBearing / lineHeight * textStyle.fontSize;

				const vec2<float32> position = {positionX + leftSideBearing, positionY};
				const vec2<float32> size = {width, textStyle.fontSize};
				const mat3x3<float32> transform = mat3x3<float32>::scaling(size) * mat3x3<float32>::translation(position);

				const vec2<float32> uvStart = {glyph.minCoordinateX, glyph.minCoordinateY};
				const vec2<float32> uvEnd = {glyph.maxCoordinateX, glyph.maxCoordinateY};
				const vec2<float32> uvDiff = uvEnd - uvStart;

				const mat3x3<float32> imageTransform = mat3x3<float32>::scaling({
					uvDiff[0] * imageExtent.width / renderer.TEXTURE_ARRAY_MAX_WIDTH,
					uvDiff[1] * imageExtent.height / renderer.TEXTURE_ARRAY_MAX_HEIGHT,
				}) * mat3x3<float32>::translation(uvStart);

				renderer.registerGlyphSubcomponent(transform, imageTransform, glyphMapImageId, textStyle);
			}

			positionX += advanceWidth;
			width += advanceWidth;
		}

		const GUISize fixedSize = {static_cast<sint16>(width), static_cast<sint16>(height)};

		componentOrigin[layoutDirection] += fixedSize[layoutDirection] + layoutSpacing;
	}

	bool GUILayer::GUIButton(const std::string& string, const ButtonStyle& buttonStyle) {
		constexpr FontID fontId = 0;
		const TrueTypeFont& font = *fonts[fontId].font;
		const uint32 glyphMapImageId = fonts[fontId].glyphMapImageId;
		const Extent2D imageExtent = renderer.getTextureArrayLayerExtent(glyphMapImageId);

		// 1. Calculate the minimum size that fully encloses the text.
		GUISize size;

		size.width = 0;
		size.height = static_cast<sint16>(buttonStyle.textStyle->fontSize);

		for (const char glyph : string) {
			const uint16 codePoint = static_cast<uint16>(glyph);
			const TrueTypeGlyph& glyph = font[codePoint];
			const float32 advanceWidth = std::ceil(glyph.advanceWidth / 1210.0f * buttonStyle.textStyle->fontSize);

			size.width += static_cast<sint16>(advanceWidth);
		}

		// 2. Register a quad having the same size.
		registerComponent(size, DEFAULT_IMAGE_ID, {0.5f, 0.5f, 0.5f, 1.0f});

		// 3. Register the text on top of it.
		{
			const float32 lineHeight = static_cast<float32>(font.getLineHeight());

			float32 positionX = componentOrigin[0];
			float32 positionY = componentOrigin[1];

			for (const char glyph : string) {
				const uint16 codePoint = static_cast<uint16>(glyph);
				const TrueTypeGlyph& glyph = font[codePoint];
				const float32 advanceWidth = std::ceil(glyph.advanceWidth / 1210.0f * buttonStyle.textStyle->fontSize);

				if (!glyph.invisible) {
					// For a line height (ascent - descent) of 1210 FUnits,
					// the 'A' glyph's width (not including bearing or advance) is 631 FUnits.
					// The width should be 62.13px.
					// TODO: float32 lyah::ceil(float32)
					const float32 width = glyph.width / lineHeight * buttonStyle.textStyle->fontSize;
					const float32 leftSideBearing = glyph.leftSideBearing / lineHeight * buttonStyle.textStyle->fontSize;

					const vec2<float32> position = {positionX + leftSideBearing, positionY};
					const vec2<float32> size = {width, buttonStyle.textStyle->fontSize};
					const mat3x3<float32> transform = mat3x3<float32>::scaling(size) * mat3x3<float32>::translation(position);

					const vec2<float32> uvStart = {glyph.minCoordinateX, glyph.minCoordinateY};
					const vec2<float32> uvEnd = {glyph.maxCoordinateX, glyph.maxCoordinateY};
					const vec2<float32> uvDiff = uvEnd - uvStart;

					const mat3x3<float32> imageTransform = mat3x3<float32>::scaling({
						uvDiff[0] * imageExtent.width / renderer.TEXTURE_ARRAY_MAX_WIDTH,
						uvDiff[1] * imageExtent.height / renderer.TEXTURE_ARRAY_MAX_HEIGHT,
					}) * mat3x3<float32>::translation(uvStart);

					renderer.registerGlyphSubcomponent(transform, imageTransform, glyphMapImageId, *buttonStyle.textStyle);
				}

				positionX += advanceWidth;
			}
		}

		bool hovered = false;

		if (intersects(
			{componentOrigin[0], componentOrigin[1]},
			size,
			{static_cast<sint16>(Application::getWindow().getMousePosition()[0]), static_cast<sint16>(Application::getWindow().getMousePosition()[1])}
		)) {
			hovered = true;
		}

		// 4. Advance the component origin by the size (towards the current layout direction).
		componentOrigin[layoutDirection] += size[layoutDirection] + layoutSpacing;

		return hovered;
	}

	bool GUILayer::intersects(GUISize origin, GUISize size, GUISize point) {
		return (
			point.width >= origin.width &&
			point.width < origin.width + size.width &&
			point.height >= origin.height &&
			point.height < origin.height + size.height
		);
	}

	GUISize GUILayer::computeFixedSize(GUISize size) const {
		GUISize fixedSize{};

		if (size.width == EXPAND) {
			fixedSize.width = currentContainerInnerSize.width;
		}
		else {
			fixedSize.width = size.width;
		}

		ENGINE_ASSERT(fixedSize.width > 0, "The fixed width must be positive.");

		if (size.height == EXPAND) {
			fixedSize.height = currentContainerInnerSize.height;
		}
		else {
			fixedSize.height = size.height;
		}

		ENGINE_ASSERT(fixedSize.height > 0, "The fixed height must be positive.");

		return fixedSize;
	}
}