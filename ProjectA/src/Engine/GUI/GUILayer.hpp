#pragma once

#include "Engine/GUI/Component/Container.hpp"
#include "Engine/GUI/GUIRenderer.hpp"
#include "Engine/GUI/ID.hpp"
#include "Engine/GUI/px.hpp"
#include "Engine/Layer/Layer.hpp"
#include "Engine/Renderer/Font/TrueType/TrueTypeFont.hpp"

namespace ProjectA {
	enum LayoutDirection : uint8 {
		HORIZONTAL = 0,
		VERTICAL = 1,
	};

	class GUILayer : public Layer {
		private:
			struct FontData {
				ref<TrueTypeFont> font;
				ImageID glyphMapImageId;
			};

		public:
			void build() final override;

			void postConstruct() final override;

			void preRender() final override;

			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const final override;

			void postRender() final override;

			virtual void GUI();

			virtual void onEvent(Event& event) final override;

		protected:
			FontID loadFont(std::string_view path);

			ImageID loadImage(std::string_view path);

			// Renders a container cascading from top to bottom and left to right.
			// `layoutDirection` is the direction the container's component must be laid towards.
			// `layoutSpacing` is the spacing between the container's component, in pixels.
			GUIContainer pushContainer(Extent2D size, Extent2D padding, LayoutDirection layoutDirection, uint32 layoutSpacing);
			GUIContainer pushContainer(Extent2D size, Extent2D padding, LayoutDirection layoutDirection, uint32 layoutSpacing, const vec4<float32>& color);

			void popContainer(GUIContainer& container);

			// Draw a rectangle with a custom color.
			void GUIComponent(Extent2D size, const vec4<float32>& color, float32 radius = 0.0f);

			void GUIImage(Extent2D size, ImageID imageId, float32 radius = 0.0f);

			void GUIText(const std::string& string, const TextStyle& textStyle);

			// Returns true if the button is hovered.
			bool GUIButton(const std::string& string, const ButtonStyle& buttonStyle);

			// Advances the component origin by `spacing` towards the current layout direction.
			void GUISpacing(uint32 spacing);

		private:
			// Renders a colored quad without moving the origin.
			// origin is the top-left corner of the quad.
			void registerSubcomponent(Extent2D size, ImageID imageId, const vec4<float32>& color, float32 radius);

			Extent2D computeFixedSize(Extent2D size) const;

			static bool intersects(Extent2D origin, Extent2D size, Extent2D point);

			bool onWindowResize(WindowResizeEvent& event);

			Extent2D calculateTextSize(const std::string& string, const TextStyle& textStyle);

		protected:
			static constexpr ImageID DEFAULT_IMAGE_INDEX = 0;

			// Grow towards the layout direction.
			static constexpr uint32 EXPAND = -1;

			// Retreat towards the layout direction.
			// static constexpr uint32 SHRINK = -2;

		private:
			static constexpr uint32 MAX_FONT_COUNT = 4;

		private:
			GUIRenderer renderer;

			bool textureArrayFlushed = false;

			std::array<FontData, MAX_FONT_COUNT> fonts;
			uint32 fontCount = 0;

			uint32 componentOrigin[2];
			LayoutDirection layoutDirection = VERTICAL;
			uint32 layoutSpacing = 0;
			Extent2D currentContainerSize;
			Extent2D currentContainerInnerSize;
			bool rootComponent = true;
	};

	#define GUIContainer(...) for (GUIContainer _container = pushContainer(__VA_ARGS__); !_container.iteration; popContainer(_container))
}