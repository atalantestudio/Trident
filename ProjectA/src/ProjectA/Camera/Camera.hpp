#pragma once

#include "math/mat/mat4x4.hpp"

namespace ProjectA {
	class Camera {
		protected:
			/// Row-major ordering
			mat4x4<float> view;

			/// Row-major ordering
			mat4x4<float> projection;

			/// Row-major ordering
			mat4x4<float> viewProjection;

			vec3<float> position;

		private:
			/// Degrees
			float fieldOfView;

			/// Width / height
			float aspectRatio;

		public:
			Camera(const mat4x4<float>& projection);

			~Camera() = default;

			const mat4x4<float>& getView() const;

			const mat4x4<float>& getProjection() const;

			const mat4x4<float>& getViewProjection() const;

			const vec3<float>& getPosition() const;

			virtual void setPosition(const vec3<float>& position);

			float getFieldOfView() const;

			float getAspectRatio() const;
	};
}