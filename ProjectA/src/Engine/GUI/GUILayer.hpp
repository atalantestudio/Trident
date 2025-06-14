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
			GUILayer();

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
			// layoutDirection is the direction the container's component must be laid towards.
			// layoutSpacing is the spacing between the container's component, in pixels.
			GUIContainer pushContainer(GUISize size, GUISize padding, LayoutDirection layoutDirection, uint16 layoutSpacing);
			GUIContainer pushContainer(GUISize size, GUISize padding, LayoutDirection layoutDirection, uint16 layoutSpacing, const vec4<float32>& color);

			void popContainer(GUIContainer& container);

			// Renders a colored/textured quad.
			void GUIComponent(GUISize size, const vec4<float32>& color);
			void GUIComponent(GUISize size, ImageID imageId, const vec4<float32>& color = {1.0f, 1.0f, 1.0f, 1.0f});

			void GUIText(const std::string& string, const TextStyle& textStyle);

			// Returns true if the button is hovered.
			bool GUIButton(const std::string& string, const ButtonStyle& buttonStyle);

		private:
			// Renders a colored quad without moving the origin.
			// origin is the top-left corner of the quad.
			void registerComponent(GUISize size, ImageID imageId, const vec4<float32>& color);

			GUISize computeFixedSize(GUISize size) const;

			static bool intersects(GUISize origin, GUISize size, GUISize point);

			bool onWindowResize(WindowResizeEvent& event);

		protected:
			static constexpr ImageID DEFAULT_IMAGE_ID = 0;

			// Grow towards the layout direction.
			static constexpr sint16 EXPAND = -1;

			// Retreat towards the layout direction.
			// static constexpr sint16 SHRINK = -2;

		private:
			static constexpr uint32 MAX_FONT_COUNT = 4;

		private:
			GUIRenderer renderer;

			bool textureArrayFlushed = false;

			std::array<FontData, MAX_FONT_COUNT> fonts;
			uint32 fontCount = 0;

			sint16 componentOrigin[2];
			LayoutDirection layoutDirection = VERTICAL;
			uint16 layoutSpacing = 0;
			GUISize currentContainerSize;
			GUISize currentContainerInnerSize;
			bool rootComponent = true;
	};

	#define GUIContainer(...) for (GUIContainer _container = pushContainer(__VA_ARGS__); !_container.iteration; popContainer(_container))
}