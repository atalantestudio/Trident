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

			void loadDefaultFont();

			void loadTexture(const std::string& path);

			template<typename T, typename... U>
			void appendRoot(U&&... arguments) {
				const ref<T> object = createRef<T>(std::forward<U>(arguments)...);

				rootObjects.emplace_back(object);
			}

			void compute();

			void registerEntityPool();

			void update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;

			void updateScriptComponents() const;

		public:
			chain<ref<Font>> fonts;

		private:
			void loadDefaultTexture();

		private:
			static constexpr uint8 MAX_GUI_COMPONENTS = 100;

		private:
			ECS ecs;
			GUIRenderer renderer;
			chain<ref<Texture>> textures;
			chain<ref<GUIObject>> rootObjects;
	};
}