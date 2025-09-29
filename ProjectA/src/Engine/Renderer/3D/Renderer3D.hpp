#pragma once

#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Camera/3D/Camera3D.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	template<typename SceneType>
	class Renderer3D : public Renderer {
		public:
			virtual void setScene(const SceneType* scene) {
				this->scene = scene;
			}

			void setCamera(const Camera3D* camera) {
				this->camera = camera;
			}

		protected:
			const SceneType* scene = nullptr;
			const Camera3D* camera = nullptr;
	};
}