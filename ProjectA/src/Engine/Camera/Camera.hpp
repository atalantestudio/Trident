#pragma once

#include "pch.hpp"

// TODO: Check order of view and projection members
namespace ProjectA {
	class Camera {
		protected:
			// Row-major ordering
			lyah::mat4x4<float32> view;

			// Row-major ordering
			lyah::mat4x4<float32> projection;

			// Row-major ordering
			lyah::mat4x4<float32> viewProjection;

		public:
			// TODO: Accept view matrix as 2nd argument?
			Camera(const lyah::mat4x4<float32>& projection);

			~Camera() = default;

			inline const lyah::mat4x4<float32>& getView() const {
				return view;
			}

			inline const lyah::mat4x4<float32>& getProjection() const {
				return projection;
			}

			inline const lyah::mat4x4<float32>& getViewProjection() const {
				return viewProjection;
			}
	};
}