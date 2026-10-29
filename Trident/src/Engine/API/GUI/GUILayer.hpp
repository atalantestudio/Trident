// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/AABB.hpp"
#include "Engine/API/GUI/Component/Container.hpp"
#include "Engine/API/GUI/FillMode.hpp"
#include "Engine/API/GUI/GUIRenderer.hpp"
#include "Engine/API/GUI/ID.hpp"
#include "Engine/API/GUI/Style/ContainerStyle.hpp"
#include "Engine/API/GUI/Style/TextInputStyle.hpp"
#include "Engine/API/GUI/Style/TextStyle.hpp"
#include "Engine/API/GUI/Style/VerticalListStyle.hpp"
#include "Engine/API/Window/Layer/RenderPassLayer.hpp"
#include "Engine/Application/Application.hpp"
#include "Engine/Filesystem/Asset/FontAsset.hpp"
#include "Engine/Filesystem/Stream/UTF8Stream.hpp"

namespace Trident {
	class GUILayer : public RenderPassLayer {
		private:
			struct FontData {
				ref<FontAsset> fontAsset;
				ImageID glyphMapImageId;
			};

		private:
			/// Text cursor width in pixels.
			static constexpr float32 TEXT_CURSOR_WIDTH = 1_px;

			/// Text cursor color, usually opaque.
			static constexpr Color TEXT_CURSOR_COLOR = 0x5cafe7ff;

			/// Text selection box color, usually semi-transparent.
			static constexpr Color TEXT_SELECTION_BOX_COLOR = 0x5cafe750;

		private:
			static bool IsIntersecting(const GUIComponent& component, vec2<float32> point);

			static float32 calculateTextPreferredWidth(UTF8Stream& stream, const FontAsset& fontAsset, float32 scale);
			static float32 calculateTextMinimumWidth(UTF8Stream& stream, const FontAsset& fontAsset, float32 scale);

			static TextCharacter createTextCharacter(const Glyph& glyph, vec2<float32> offset, float32 fontScale);

			/// Clips an AABB against another AABB and returns a new AABB where each vector represents the axis percents for coordinate interpolation.
			static AABB2D clipAabb(AABB2D a, AABB2D b);

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

			virtual bool onKeyDownClient(KeyDownEvent& event) {
				return false;
			}

			virtual void onMouseUpClient(MouseUpEvent& event) {}

		protected:
			// TODO(gui/asset-loading): Make available only inside registerAssets.
			FontID loadFontAsset(view<char8> assetName);

			// TODO(gui/asset-loading): Make available only inside registerAssets.
			ImageID registerImage(ref<ImageAsset> imageAsset);

			void openComponent(GUIComponent& component, bool focusable);
			void openContainer(const ContainerStyle& style, bool floating);
			void openVerticalList(const VerticalListStyle& style);

			void closeContainer();

			/// Registers a text component.
			void Text(view<char8> text, const TextStyle& style);

			/// Registers a text input component.
			void TextInput(const TextInputStyle& style);

			/// Returns the current component (computed on the previous frame).
			const GUIComponent* CurrentComponent() const;

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
			void setComponentSize(GUIComponent& component, vec2<float32> minimumSize, vec2<float32> maximumSize) const;

			// Processes the current component for rendering and returns whether to process its children too.
			bool registerComponent(const GUIComponent& component, uint32 componentIndex, AABB2D parentAabb);

			void renderText(const GUIComponent& component, AABB2D parentAabb);
			void renderTextCursor(const GUIComponent& component);
			void renderTextSelectionBox(const GUIComponent& component);

			std::vector<uint32> FilterChildren(const GUIComponent& parent, uint8 axis, FillMode fillMode) const;

			void AdjustWidth(uint32 componentIndex);
			void AdjustHeight(uint32 componentIndex);

			float32 calculateRemainingSize(const GUIComponent& component, uint8 axis) const;
			float32 calculateFontScale(ref<FontAsset> fontAsset, float32 fontSize) const;

			// Width is represented by an axis of 0 and height is represented by an axis of 1.
			// This function recursively calls the component's children.
			void expandAndFitComponent(uint32 componentIndex, uint8 axis);

			void WrapTextChildrenRecursive(uint32 parentIndex);
			void WrapText(GUIComponent& component);
			void WrapInputText(GUIComponent& component, uint32 componentIndex);
			void FlushCurrentWord(const UTF8Stream& stream, const FontAsset& fontAsset, float32 fontScale, vec2<float32>& characterOffset, std::vector<TextCharacter>& characters, range<uint64, uint64>& wordByteRange) const;
			void FlushWord(UTF8Stream& stream, const FontAsset& fontAsset, float32 fontScale, vec2<float32>& characterOffset, std::vector<TextCharacter>& characters) const;
			void PositionsAndAlignment(GUIComponent& component, uint32 componentIndex, vec2<float32> origin, AABB2D parentAabb, bool registerSubcomponents);

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

			void updateTextInput(TextInputEvent& event, const GUIComponent& component) const;
			void updateTextInputAction(KeyDownEvent& event, const GUIComponent& component) const;
			void updateVerticalList(MouseWheelEvent& event, GUIComponent& component) const;

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
			static constexpr uint32 MAX_COMPONENT_COUNT = 1024;
			static constexpr uint32 MAX_FONT_COUNT = 4;

		protected:
			/// Color of the root component.
			Color clearColor = 0x00000000;

		private:
			GUIRenderer renderer;

			std::array<FontData, MAX_FONT_COUNT> fonts;
			uint32 fontCount = 0;

			sequence<GUIComponent> components;
			sequence<uint32> openContainerIndices;
			sequence<uint32> focusableComponentIndexToComponentIndex;

			// Component tree from the previous frame.
			sequence<GUIComponent> previousComponents;

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
}