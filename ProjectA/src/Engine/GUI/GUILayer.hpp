#pragma once

#include "Engine/GUI/GUIRenderer.hpp"
#include "Engine/GUI/GUISize.hpp"
#include "Engine/GUI/LayoutDirection.hpp"
#include "Engine/Layer/Layer.hpp"
#include "Engine/Renderer/Font/TrueType/TrueTypeFont.hpp"

namespace ProjectA {
	#pragma warning(disable: 4455)

	// User-defined literals not prefixed with an underscore are forbidden,
	// but for now I'll leave this here to keep consistency with a future transpiled scripting language.
	constexpr sint16 operator"" px(uint64 a) {
		const sint16 b = static_cast<sint16>(a);
		const uint64 c = static_cast<uint64>(b);

		DEBUG_ASSERT(a == c);

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

			virtual void GUI();

			virtual void onEvent(Event& event) final override;

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

			void GUIText(const std::string& string, const TextStyle& textStyle);

			bool GUIButton(const std::string& string, const ButtonStyle& buttonStyle);

			void popContainer(GUIContainer& container);

		private:
			void loadDefaultFont();

			// Renders a colored quad without moving the origin.
			// origin is the top-left corner of the quad.
			void registerQuadComponent(GUISize origin, GUISize size, ImageID imageId, const vec4<float32>& color);

			GUISize computeFixedSize(GUISize size) const;

			static bool intersects(GUISize origin, GUISize size, GUISize point);

			bool onWindowResize(WindowResizeEvent& event);

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