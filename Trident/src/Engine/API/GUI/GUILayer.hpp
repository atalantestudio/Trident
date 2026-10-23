// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/Component/Container.hpp"
#include "Engine/API/GUI/FillMode.hpp"
#include "Engine/API/GUI/GUIRenderer.hpp"
#include "Engine/API/GUI/ID.hpp"
#include "Engine/API/GUI/Style/ContainerStyle.hpp"
#include "Engine/API/GUI/Style/TextInputStyle.hpp"
#include "Engine/API/GUI/Style/TextStyle.hpp"
#include "Engine/API/GUI/Style/VerticalListStyle.hpp"
#include "Engine/API/Window/Layer/RenderPassLayer.hpp"
#include "Engine/Filesystem/Stream/UTF8Stream.hpp"

namespace Trident {
	class GUILayer : public RenderPassLayer {
		private:
			struct FontData {
				ref<FontAsset> fontAsset;
				ImageID glyphMapImageId;
			};

			struct FlushWordData {
				UTF8Stream stream;
				const FontAsset* fontAsset;
				float32 scale;
			};

		private:
			/// Text cursor width in pixels.
			static constexpr float32 TEXT_CURSOR_WIDTH = 4_px;

			/// Text cursor color, usually opaque.
			static constexpr Color TEXT_CURSOR_COLOR = 0x5cafe7ff;

			/// Text selection box color, usually semi-transparent.
			static constexpr Color TEXT_SELECTION_BOX_COLOR = 0x5cafe750;

		private:
			static bool IsIntersecting(const GUIContainer& component, vec2<float32> point);

			static float32 calculateTextPreferredWidth(UTF8Stream& stream, const FontAsset& fontAsset, float32 scale);
			static float32 calculateTextMinimumWidth(UTF8Stream& stream, const FontAsset& fontAsset, float32 scale);

		public:
			void initialize() final override;

			void encodeOneTimeCommands() final override;

			virtual void registerAssets() {}

			void postConstruct() final override;

			void preRender() final override;

			void render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const final override;

			void postRender() final override;

			virtual void render() {}

			virtual void onEvent(Event& event) final override;

			virtual void onMouseUpClient(MouseUpEvent& event) {}

		protected:
			// TODO(gui/asset-loading): Make available only inside registerAssets.
			FontID loadFontAsset(view<char8> assetName);

			// TODO(gui/asset-loading): Make available only inside registerAssets.
			ImageID registerImage(ref<ImageAsset> imageAsset);

			void openComponent(GUIContainer& component, bool focusable);
			void openContainer(const ContainerStyle& style, bool floating);
			void openVerticalList(const VerticalListStyle& style);
			void openTextInput(const TextInputStyle& style);

			void closeContainer();

			void Text(view<char8> text, const TextStyle& style);

			/// Returns the current component (computed on the previous frame).
			const GUIContainer* CurrentComponent() const;

			/// Returns whether the mouse pointer is hovering over the current component.
			bool IsHovered() const;

			/// Returns whether the mouse pointer is hovering over the next component.
			bool IsNextHovered() const;

			/// Returns whether the mouse button is pressed on the current component.
			bool IsMouseDown() const;

			/// Returns whether the mouse button is pressed on the next component.
			bool IsNextMouseDown() const;

			/// Returns whether the mouse button is released on the current component.
			bool IsMouseUp() const;

			/// Returns whether the mouse pointer is clicking on the current component.
			bool IsClicked();

			/// Returns whether the current component is focused.
			bool IsFocused() const;

			/// Returns whether the next component is focused.
			bool IsNextFocused() const;

			vec2<float32> getMousePosition() const;

		private:
			void setComponentSize(GUIContainer& component, vec2<float32> minimumSize, vec2<float32> maximumSize) const;

			// Processes the current component for rendering and returns whether to process its children too.
			bool registerComponent(const GUIContainer& component, uint32 componentIndex);

			void renderText(const GUIContainer& component);
			void renderTextCursor(const GUIContainer& component);
			void renderTextSelectionBox(const GUIContainer& component);

			std::vector<uint32> FilterChildren(const GUIContainer& parent, uint8 axis, FillMode fillMode) const;

			void AdjustWidth(uint32 componentIndex);
			void AdjustHeight(uint32 componentIndex);

			float32 calculateRemainingSize(const GUIContainer& component, uint8 axis) const;

			// Width is represented by an axis of 0 and height is represented by an axis of 1.
			// This function recursively calls the component's children.
			void expandAndFitComponent(uint32 componentIndex, uint8 axis);

			void WrapTextChildrenRecursive(uint32 parentIndex);
			void WrapText(GUIContainer& component);
			void WrapInputText(GUIContainer& component, uint32 componentIndex);
			void FlushCurrentWord(FlushWordData& flushWordData, std::vector<TextCharacter>& characters, const UTF8Stream& stream, vec2<float32>& characterOffset, range<uint64, uint64>& wordByteRange) const;
			void FlushWord(FlushWordData& data, std::vector<TextCharacter>& textCharacters, vec2<float32>& characterOffset) const;
			void PositionsAndAlignment(GUIContainer& component, uint32 componentIndex, vec2<float32> origin, bool registerSubcomponents);

			bool onKeyDown(KeyDownEvent& event);
			bool onMouseDown(MouseDownEvent& event);
			bool onMouseMove(MouseMoveEvent& event);
			bool onMouseUp(MouseUpEvent& event);
			bool onMouseWheel(MouseWheelEvent& event);
			bool onTextInput(TextInputEvent& event);
			bool onWindowResize(WindowResizeEvent& event);

			void focusComponent(uint32 componentIndex) const;
			void unfocusComponent(uint32 componentIndex) const;

			void updateHoveredComponentIndex();

			void updateTextInput(TextInputEvent& event, const GUIContainer& component) const;
			void updateTextInputAction(KeyDownEvent& event, const GUIContainer& component) const;
			void updateVerticalList(MouseWheelEvent& event, GUIContainer& component) const;

			bool IsHovered(uint32 componentIndex) const;
			bool IsMouseDown(uint32 componentIndex) const;
			bool IsMouseUp(uint32 componentIndex) const;
			bool IsFocused(uint32 componentIndex) const;

		public:
			/// Shrink towards the layout direction.
			static constexpr float32 FIT = -2.0f;

			/// Grow towards the layout direction.
			static constexpr float32 EXPAND = -1.0f;

		private:
			static constexpr uint32 MAX_COMPONENT_COUNT = 128;
			static constexpr uint32 MAX_FONT_COUNT = 4;

		protected:
			/// Color of the root component.
			Color clearColor = 0x00000000;

			// TODO(gui/text-smoothness)
			vec2<float32> hinting = {0.0f, 0.0f};

		private:
			GUIRenderer renderer;

			std::array<FontData, MAX_FONT_COUNT> fonts;
			uint32 fontCount = 0;

			sequence<GUIContainer> components;
			sequence<uint32> openContainerIndices;
			sequence<uint32> focusableComponentIndexToComponentIndex;

			// Component tree from the previous frame.
			sequence<GUIContainer> previousComponents;

			uint32 componentCount = 0;
			uint32 previousComponentCount = 0;
			uint32 previousFocusableComponentCount = 0;
			uint32 openContainerIndexCount = 0;
			uint32 focusableComponentCount = 0;

			uint32 hoveredComponentIndex = max<uint32>();
			uint32 hoveredVerticalListIndex = max<uint32>();

			// Index of the currently focused component.
			uint32 focusedComponentIndex = max<uint32>();

			// Index of a component that was focused and received an Enter signal.
			uint32 enterComponentIndex = max<uint32>();

			bool mouseUp = false;
			vec2<float32> lastMouseDownPosition = {-1.0f, -1.0f};
			vec2<float32> lastMouseUpPosition = {-1.0f, -1.0f};

			// Text cursor and selection box options for the currently focused text input.
			float32 textCursorHorizontalOffset = 0_px;
			float32 textSelectionBoxHorizontalOffset = 0_px;
	};

	/// Opens a GUI container component.
	#define Container(...) for (bool _ = (openContainer(__VA_ARGS__, false), true); _; _ = (closeContainer(), false))

	/// Opens a GUI vertical list.
	#define VerticalList(...) for (bool _ = (openVerticalList(__VA_ARGS__), true); _; _ = (closeContainer(), false))

	/// Opens a floating GUI container component.
	#define Floating(...) for (bool _ = (openContainer(__VA_ARGS__, true), true); _; _ = (closeContainer(), false))

	/// Opens a GUI text input component.
	#define TextInput(...) for (bool _ = (openTextInput(__VA_ARGS__), true); _; _ = (closeContainer(), false))
}