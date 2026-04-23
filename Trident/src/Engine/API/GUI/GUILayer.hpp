// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/Component/Container.hpp"
#include "Engine/API/GUI/GUI.hpp"
#include "Engine/API/GUI/GUIRenderer.hpp"
#include "Engine/API/GUI/ID.hpp"
#include "Engine/API/GUI/Style/ContainerStyle.hpp"
#include "Engine/API/GUI/Style/TextStyle.hpp"
#include "Engine/Filesystem/Stream/UTF8Stream.hpp"
#include "Engine/Layer/Layer.hpp"

namespace Trident {
	class GUILayer : public Layer {
		private:
			struct FontData {
				ref<FontAsset> fontAsset;
				ImageID glyphMapImageId;
			};

			struct FlushWordData {
				UTF8Stream stream;

				const FontAsset* fontAsset;
				vec2<float32> imageSize;
				float32 fontSize;

				// Line height scaled by the font size.
				float32 lineHeight;
			};

		public:
			void build() final override {}

			// Client-side build method.
			virtual void registerAssets() {}

			void postConstruct() final override;

			void preRender() final override;

			void render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const final override;

			void postRender() final override;

			// Client-side render method.
			virtual void GUI() {}

			virtual void onEvent(Event& event) final override;

		protected:
			FontID loadFontAsset(view<char8> assetName);
			ImageID loadImageAsset(view<char8> assetName);

			/// Returns whether the mouse pointer is hovering over the current component.
			bool IsHovered() const;

			/// Returns whether the mouse pointer is clicking on the current component.
			bool IsClicked();

		public:
			void openComponent(const ContainerStyle& style);

			// TODO: Deprecate (use Text instead).
			void GUIText(const sequence<char8>& text, const TextStyle& style);

			void Text(view<char8> text, const TextStyle& style);

			void closeComponent();

		private:
			void openRootComponent();
			void closeRootComponent();

			std::vector<uint32> FindGrowableChildIndices(const GUIContainer& parent, uint8 axis) const;
			std::vector<uint32> FindShrinkableChildIndices(const GUIContainer& parent, uint8 axis) const;

			void FitSizingWidths(uint32 componentIndex);
			void FitSizingHeights(uint32 componentIndex);
			void GrowAndShrinkSizingWidthsRecursive(uint32 parentIndex);
			void GrowAndShrinkSizingHeightsRecursive(uint32 parentIndex);
			void WrapTextChildrenRecursive(uint32 parentIndex);
			void WrapText(GUIContainer& textComponent);
			void FlushWord(FlushWordData& data, std::vector<TextCharacter>& textCharacters, svec2<float32>& characterOffset);
			void PositionsAndAlignment(GUIContainer& component, svec2<float32> origin);
			uint32 GetHoveredComponentIndex() const;
			static bool IsIntersecting(const GUIContainer& component, const vec2<float32>& point);
			void DispatchMouseEvents();

			void registerSubcomponent(const GUIContainer& component);

			bool onMouseDown(MouseDownEvent& event);
			bool onMouseUp(MouseUpEvent& event);
			bool onWindowResize(WindowResizeEvent& event);

			float32 calculateTextPreferredWidth(UTF8Stream& stream, const TextStyle& textStyle) const;
			float32 calculateTextMinimumWidth(UTF8Stream& stream, const TextStyle& textStyle) const;

		public:
			// Shrink towards the layout direction.
			static constexpr float32 FIT = -2.0f;

			// Grow towards the layout direction.
			static constexpr float32 EXPAND = -1.0f;

		private:
			static constexpr uint32 MAX_COMPONENT_COUNT = 64;

			static constexpr ImageID DEFAULT_IMAGE_INDEX = 0;
			static constexpr uint32 MAX_FONT_COUNT = 4;

			static constexpr uint8 WIDTH_AXIS = 0;
			static constexpr uint8 HEIGHT_AXIS = 1;

		private:
			GUIRenderer renderer;

			bool textureArrayFlushed = false;

			std::array<FontData, MAX_FONT_COUNT> fonts;
			uint32 fontCount = 0;

			sequence<GUIContainer> components;
			sequence<uint32> openComponentIndices;
			sequence<uint32> buttonComponentIndices;

			/// Component tree from the previous frame.
			sequence<GUIContainer> previousComponents;

			uint32 componentCount = 0;
			uint32 previousComponentCount = 0;
			uint32 openComponentIndexCount = 0;
			uint32 buttonComponentIndexCount = 0;

			uint32 hoveredComponentIndex = max<uint32>();
			uint32 mouseDownComponentIndex = max<uint32>();

			bool mouseUp = false;
			vec2<float32> lastMouseDownPosition = {-1.0f, -1.0f};
			vec2<float32> lastMouseUpPosition = {-1.0f, -1.0f};
	};

	#define GUIComponent(...) for (bool _opened = (openComponent(__VA_ARGS__), true); _opened; _opened = false, closeComponent())

	// Specific to objects extending GUIComponent.
	#define GUIComponentClass(component, renderChildren) component.render(BIND(openComponent), BIND(closeComponent), renderChildren)
}