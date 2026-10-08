// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class Camera {
		public:
			const mat4x4<float32>& getProjection() const {
				return projection;
			}

			const mat4x4<float32>& getView() const {
				return view;
			}

			const mat4x4<float32>& getProjectionView() const {
				return projectionView;
			}

			void updateProjectionView() {
				projectionView = view * projection;
			}

		protected:
			mat4x4<float32> projection = identity<mat4x4<float32>>();
			mat4x4<float32> view = identity<mat4x4<float32>>();
			mat4x4<float32> projectionView = identity<mat4x4<float32>>();
	};
}