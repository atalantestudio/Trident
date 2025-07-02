#pragma once

#include "Engine/GUI/Component/Container.hpp"
#include "Engine/GUI/ComponentStyle/ContainerStyle.hpp"
#include "Engine/GUI/Font/TrueType/TrueTypeFont.hpp"
#include "Engine/GUI/GUIRenderer.hpp"
#include "Engine/GUI/ID.hpp"
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

			void openComponent(const ContainerStyle& style);

			void GUIText(const std::string& string, const TextStyle& textStyle);

			void closeComponent();

		private:
			// Renders a colored quad without moving the origin.
			// `origin` is the top-left corner of the quad.
			void registerSubcomponent(svec2<float32> origin, const Size2D& size, ImageID imageId, const Color& color, float32 radius);

			static bool intersects(Extent2D origin, Extent2D size, Extent2D point);

			bool onWindowResize(WindowResizeEvent& event);

			uint32 calculateTextPreferredWidth(const std::string& string, const TextStyle& textStyle) const;

			// Returns the size 
			uint32 calculateTextMinimumWidth(const std::string& string, const TextStyle& textStyle) const;

		protected:
			// Shrink towards the layout direction.
			static constexpr float32 FIT = -2.0f;

			// Grow towards the layout direction.
			static constexpr float32 EXPAND = -1.0f;

		private:
			static constexpr ImageID DEFAULT_IMAGE_INDEX = 0;
			static constexpr uint32 MAX_FONT_COUNT = 4;

		private:
			GUIRenderer renderer;

			bool textureArrayFlushed = false;

			std::array<FontData, MAX_FONT_COUNT> fonts;
			uint32 fontCount = 0;

			std::vector<uint32> openedComponents;
			std::vector<GUIContainer> components;
	};

	#define GUIComponent(component) for (bool _opened = (openComponent(component), true); _opened; _opened = false, closeComponent())
}