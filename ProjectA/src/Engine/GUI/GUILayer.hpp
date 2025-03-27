#pragma once

#include "Engine/GUI/GUIRenderer.hpp"
#include "Engine/GUI/Object/GUIObject.hpp"
#include "Engine/Layer/Layer.hpp"

namespace ProjectA {
	class GUILayer : public Layer {
		public:
			GUILayer();

			~GUILayer();

			void append(const GUIObject& object);

			void registerColors(const chain<vec4<float32>>& colors) const;

			void registerEntityPool();

			void update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;

			void updateScriptComponents() const;

		private:
			static constexpr uint8 MAX_GUI_COMPONENTS = 100;

		private:
			ECS ecs;
			GUIRenderer renderer;
	};
}