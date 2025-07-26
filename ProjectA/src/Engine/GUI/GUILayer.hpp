#pragma once

#include "Engine/GUI/Component/Container.hpp"
#include "Engine/GUI/ComponentStyle/ContainerStyle.hpp"
#include "Engine/GUI/ComponentStyle/TextStyle.hpp"
#include "Engine/GUI/Font/Font.hpp"
#include "Engine/GUI/GUIRenderer.hpp"
#include "Engine/GUI/ID.hpp"
#include "Engine/Layer/Layer.hpp"

namespace ProjectA {
	class GUILayer : public Layer {
		private:
			struct FontData {
				scope<Font> font;
				ImageID glyphMapImageId;
			};

			struct FlushWordData {
				std::string_view word;

				const Font* font;
				vec2<float32> imageSize;
				float32 fontSize;

				// Line height scaled by the font size.
				float32 lineHeight;
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
			void openRootComponent();
			void closeRootComponent();

			std::vector<uint32> FindGrowableChildIndices(const GUIContainer& parent, bool axis) const;
			std::vector<uint32> FindShrinkableChildIndices(const GUIContainer& parent, bool axis) const;

			void FitSizingWidths(uint32 componentIndex);
			void FitSizingHeights(uint32 componentIndex);
			void GrowAndShrinkSizingWidthsRecursive(uint32 parentIndex);
			void GrowAndShrinkSizingHeightsRecursive(uint32 parentIndex);
			void WrapTextChildrenRecursive(uint32 parentIndex);
			void WrapText(GUIContainer& textComponent);
			bool OutsideWord(char8 character, const Font& font) const;
			void FlushWord(const FlushWordData& data, std::vector<TextCharacter>& textCharacters, svec2<float32>& characterOffset);
			void PositionsAndAlignment(GUIContainer& component, svec2<float32> origin);
			uint32 GetHoveredComponentIndex() const;
			static bool IsIntersecting(const GUIContainer& component, const vec2<float32>& point);
			void DispatchMouseEvents();

			void registerSubcomponent(svec2<float32> origin, const Size2D& size, ImageID imageId, const Color& color, float32 outlineSize, const Color& outlineColor, float32 borderRadius);

			bool onMouseDown(MouseDownEvent& event);
			bool onMouseUp(MouseUpEvent& event);
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

			static constexpr bool WIDTH_AXIS = 0;
			static constexpr bool HEIGHT_AXIS = 1;

		private:
			GUIRenderer renderer;

			bool textureArrayFlushed = false;

			std::array<FontData, MAX_FONT_COUNT> fonts;
			uint32 fontCount = 0;

			std::vector<GUIContainer> components;
			std::vector<uint32> openComponentIndices;
			std::vector<uint32> leafComponentIndices;
			std::vector<uint32> buttonComponentIndices;

			uint32 hoveredComponentIndex = max<uint32>();
			uint32 mouseDownComponentIndex = max<uint32>();
	};

	#define GUIComponent(component) for (bool _opened = (openComponent(component), true); _opened; _opened = false, closeComponent())
}