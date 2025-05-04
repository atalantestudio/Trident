#pragma once

#include "Engine/GUI/GUIComponentLayout.hpp"
#include "Engine/GUI/GUIRenderer.hpp"
#include "Engine/GUI/GUISize.hpp"
#include "Engine/GUI/LayoutDirection.hpp"
#include "Engine/GUI/Object/GUIObject.hpp"
#include "Engine/Layer/Layer.hpp"
#include "Engine/Renderer/Font/Font.hpp"

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

	struct GUIContainer {
		// Top-left corner of the container, excluding padding.
		GUISize origin;

		GUISize padding;

		// Container size, including padding.
		GUISize size;

		// Container content size, excluding padding.
		GUISize innerSize;

		GUISize parentPadding;
		GUISize parentSize;
		GUISize parentInnerSize;

		bool root;

		bool _iteration = false;
	};

	class GUILayer : public Layer {
		public:
			GUILayer();

			~GUILayer();

			//void loadDefaultFont();

			//void loadImage(const std::string& path);

			//void registerEntityPool();

			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) final override;

			virtual void renderGUI();

			//void updateScriptComponents() const;

			// Renders a container cascading from top to bottom and left to right.
			// layoutDirection is the direction the container's component must be laid towards.
			// layoutSpacing is the spacing between the container's component, in pixels.
			GUIContainer pushContainer(GUISize size, GUISize padding, LayoutDirection layoutDirection, uint16 layoutSpacing);
			GUIContainer pushContainer(GUISize size, GUISize padding, LayoutDirection layoutDirection, uint16 layoutSpacing, const vec4<float32>& color);

			// Renders a colored quad.
			void GUIQuad(GUISize size, const vec4<float32>& color);

			void popContainer(GUIContainer& container);

		protected:
			LayoutDirection getLayoutDirection() const;

		private:
			// Renders a colored quad without moving the origin.
			// origin is the top-left corner of the quad.
			void GUIFloatingQuad(GUISize origin, GUISize size, const vec4<float32>& color);

			GUISize computeFixedSize(GUISize size) const;

		public:
			//chain<ref<Font>> fonts;

		private:
			//static constexpr uint8 MAX_GUI_COMPONENTS = 100;

		private:
			GUIRenderer renderer;
			chain<ref<Image>> images;

		private:
			sint16 componentOrigin[2];
			LayoutDirection layoutDirection = VERTICAL;
			uint16 layoutSpacing = 0;
			GUISize currentContainerPadding;
			GUISize currentContainerSize;
			GUISize currentContainerInnerSize;
			bool rootComponent = true;
	};

	// Allows a GUI component to grow towards the desired direction.
	constexpr sint16 EXPAND = -1;
}

#define GUIContainer(...) for (GUIContainer container = pushContainer(__VA_ARGS__); container._iteration != true; popContainer(container))