#pragma once

#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class Renderer3D : public Renderer {
		public:
			virtual void setScene(const Scene3D* scene) {
				this->scene = scene;
			}

			void setCamera(const Camera3D* camera) {
				this->camera = camera;
			}

		protected:
			const Scene3D* scene = nullptr;
			const Camera3D* camera = nullptr;
	};
}