#pragma once

#include "Camera.hpp"
#include "math/vec/vec3.hpp"

namespace ProjectA {
	class OrthographicCamera : public Camera {
		private:
			/// Rotation along the Z axis
			float rotation;

			void computeView();

		public:
			OrthographicCamera(float, float, float, float, float, float);

			void setPosition(const vec3<float>& position) override;

			float getRotation() const;

			void setRotation(float rotation);
	};
}