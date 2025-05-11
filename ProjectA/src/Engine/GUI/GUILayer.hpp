#pragma once

#include "Engine/GUI/GUIRenderer.hpp"
#include "Engine/GUI/GUISize.hpp"
#include "Engine/GUI/LayoutDirection.hpp"
#include "Engine/Layer/Layer.hpp"
#include "Engine/Renderer/Font/TrueType/TrueTypeFont.hpp"

namespace ProjectA {
	#pragma warning(disable: 4455)

	// User-defined literals not prefixed with an underscore are forbidden,
	// but for now I'll leave this here to keep consistency
	// with a future transpiled scripting language.
	constexpr sint16 operator"" px(uint64 a) {
		const sint16 b = static_cast<sint16>(a);
		const uint64 c = static_cast<uint64>(b);

		// TODO: static_assert instead?
		ENGINE_ASSERT(a == c, "UDL overflow.");

		return b;
	}

	// Allows a GUI component to grow towards the desired direction.
	constexpr sint16 EXPAND = -1;

	typedef uint32 ImageID;

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

		bool _iteration = false;
	};

	class GUILayer : public Layer {
		public:
			GUILayer();

			//~GUILayer();

			void preRender() final override;

			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const final override;

			void postRender() final override;

			virtual void updateGUI();

		protected:
			ImageID loadImage(std::string_view path);

			//void registerEntityPool();

			//void updateScriptComponents() const;

			// Renders a container cascading from top to bottom and left to right.
			// layoutDirection is the direction the container's component must be laid towards.
			// layoutSpacing is the spacing between the container's component, in pixels.
			GUIContainer pushContainer(GUISize size, GUISize padding, LayoutDirection layoutDirection, uint16 layoutSpacing);
			GUIContainer pushContainer(GUISize size, GUISize padding, LayoutDirection layoutDirection, uint16 layoutSpacing, const vec4<float32>& color);

			// Renders a colored quad.
			void GUIQuad(GUISize size, const vec4<float32>& color);
			void GUIQuad(GUISize size, ImageID imageId, const vec4<float32>& color = {1.0f, 1.0f, 1.0f, 1.0f});

			template<typename... T>
			void GUIText(std::string_view pattern, T&&... arguments) {
				constexpr float32 fontSize = 32;

				const std::string text = Logger::engineLogger.format(pattern, std::forward<T>(arguments)...);
				const float32 lineHeight = static_cast<float32>(defaultFont->getLineHeight());

				float32 positionX = componentOrigin[0];
				float32 positionY = componentOrigin[1];

				for (const char glyph : text) {
					const uint16 codePoint = static_cast<uint16>(glyph);
					const TrueTypeGlyph& glyph = (*defaultFont)[codePoint];
					const float32 advanceWidth = std::ceil(glyph.advanceWidth / 1210.0f * static_cast<float32>(fontSize));

					if (!glyph.invisible) {
						// For a line height (ascent - descent) of 1210 FUnits,
						// the 'A' glyph's width (not including bearing or advance) is 631 FUnits.
						// The width should be 62.13px.
						// TODO: float32 lyah::ceil(float32)
						const float32 width = glyph.width / lineHeight * fontSize;
						const float32 leftSideBearing = glyph.leftSideBearing / lineHeight * fontSize;

						const vec2<float32> position = {positionX + leftSideBearing, positionY};
						const vec2<float32> size = {width, fontSize};
						const mat3x3<float32> transform = mat3x3<float32>::scaling(size) * mat3x3<float32>::translation(position);

						const vec2<float32> uvStart = {glyph.minCoordinateX, glyph.minCoordinateY};
						const vec2<float32> uvEnd = {glyph.maxCoordinateX, glyph.maxCoordinateY};
						const mat3x3<float32> imageTransform = mat3x3<float32>::scaling(uvEnd - uvStart) * mat3x3<float32>::translation(uvStart);

						renderer.registerSubcomponent(transform, imageTransform, defaultFontImageIndex, {1.0f, 1.0f, 1.0f, 1.0f});
					}

					positionX += advanceWidth;
				}
			}

			void popContainer(GUIContainer& container);

		private:
			void loadDefaultFont();

			// Renders a colored quad without moving the origin.
			// origin is the top-left corner of the quad.
			void GUIFloatingQuad(GUISize origin, GUISize size, ImageID imageId, const vec4<float32>& color);

			GUISize computeFixedSize(GUISize size) const;

		private:
			//static constexpr uint8 MAX_GUI_COMPONENTS = 100;

		private:
			GUIRenderer renderer;
			sint16 componentOrigin[2];
			LayoutDirection layoutDirection = VERTICAL;
			uint16 layoutSpacing = 0;
			GUISize currentContainerSize;
			GUISize currentContainerInnerSize;
			bool rootComponent = true;
			scope<TrueTypeFont> defaultFont;

			ImageID defaultImageIndex;
			ImageID defaultFontImageIndex;
	};
}

#define GUIContainer(...) for (GUIContainer _container = pushContainer(__VA_ARGS__); !_container._iteration; popContainer(_container))