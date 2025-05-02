#pragma once

#include "Engine/GUI/GUIRenderer.hpp"
#include "Engine/GUI/Object/GUIObject.hpp"
#include "Engine/Layer/Layer.hpp"
#include "Engine/Renderer/Font/Font.hpp"

namespace ProjectA {
	class GUILayer : public Layer {
		public:
			GUILayer();

			~GUILayer();

			//void loadDefaultFont();

			//void loadTexture(const std::string& path);

			/*template<typename T, typename... U>
			void appendRoot(U&&... arguments) {
				const ref<T> object = createRef<T>(std::forward<U>(arguments)...);

				rootObjects.emplace_back(object);
			}*/

			//void compute();

			//void registerEntityPool();

			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) final override;

			virtual void renderGUI();

			//void updateScriptComponents() const;

			// Renders a quad.
			void GUIQuad(const mat3x3<float32>& world);

		public:
			//chain<ref<Font>> fonts;

		private:
			//static constexpr uint8 MAX_GUI_COMPONENTS = 100;

		private:
			GUIRenderer renderer;
			//chain<ref<Image>> images;
			//chain<ref<GUIObject>> rootObjects;
	};
}