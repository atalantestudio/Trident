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
				// TODO: Check why the "projection * view" multiplication order doesn't return the right matrix.
				projectionView = view * projection;
			}

		protected:
			mat4x4<float32> projection = mat4x4<float32>::identity();
			mat4x4<float32> view = mat4x4<float32>::identity();
			mat4x4<float32> projectionView = mat4x4<float32>::identity();
	};
}