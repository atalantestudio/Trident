#pragma once

#include "Engine/GUI/Component/Container.hpp"
#include "Engine/GUI/ComponentStyle/ContainerStyle.hpp"
#include "Engine/GUI/Font/TrueType/TrueTypeFont.hpp"
#include "Engine/GUI/GUIRenderer.hpp"
#include "Engine/GUI/ID.hpp"
#include "Engine/GUI/px.hpp"
#include "Engine/Layer/Layer.hpp"

namespace ProjectA {
	class GUILayer : public Layer {
		private:
			struct FontData {
				ref<TrueTypeFont> font;
				ImageID glyphMapImageId;
			};

		public:
			void build() final override;

			// Client-side build method.
			virtual void registerAssets() {}

			void postConstruct() final override;

			void preRender() final override;

			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const final override;

			void postRender() final override;

			// Client-side render method.
			virtual void GUI() {}

			virtual void onEvent(Event& event) final override;

		protected:
			FontID loadFont(std::string_view path);

			ImageID loadImage(std::string_view path);

			void openContainer(const ContainerStyle& style);

			void closeContainer();

			// Draw a rectangle with a custom color.
			// void GUIComponent(Extent2D size, const vec4<float32>& color, float32 radius = 0.0f);

			// void GUIImage(Extent2D size, ImageID imageId, float32 radius = 0.0f);

			// void GUIText(const std::string& string, const TextStyle& textStyle);

			// Returns true if the button is hovered.
			// bool GUIButton(const std::string& string, const ButtonStyle& buttonStyle);

			// Advances the component origin by `spacing` towards the current layout direction.
			// void GUISpacing(uint32 spacing);

		private:
			// Renders a colored quad without moving the origin.
			// `origin` is the top-left corner of the quad.
			void registerSubcomponent(Extent2D origin, const Size2D& size, ImageID imageId, const Color& color, float32 radius);

			static bool intersects(Extent2D origin, Extent2D size, Extent2D point);

			bool onWindowResize(WindowResizeEvent& event);

			Extent2D calculateTextSize(const std::string& string, const TextStyle& textStyle) const;

		protected:
			static constexpr ImageID DEFAULT_IMAGE_INDEX = 0;

			// Grow towards the layout direction.
			static constexpr uint32 EXPAND = -1;

			// Shrink towards the layout direction.
			static constexpr uint32 FIT = -2;

		private:
			static constexpr uint32 MAX_FONT_COUNT = 4;

		private:
			GUIRenderer renderer;

			bool textureArrayFlushed = false;

			std::array<FontData, MAX_FONT_COUNT> fonts;
			uint32 fontCount = 0;

			std::vector<uint32> openedComponents;
			std::vector<GUIContainer> components;

			// uint32 componentOrigin[2];
			// LayoutDirection layoutDirection = VERTICAL;
			// uint32 layoutSpacing = 0;
			// Extent2D currentContainerSize;
			// bool rootComponent = true;
	};

	#define GUIContainer(...) for (bool _opened = (openContainer(__VA_ARGS__), true); _opened; _opened = false, closeContainer())
}