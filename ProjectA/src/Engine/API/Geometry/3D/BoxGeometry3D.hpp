#pragma once

#include "Engine/API/Geometry/3D/Geometry3D.hpp"

namespace ProjectA {
	class BoxGeometry3D : public Geometry3D {
		private:
			static inline const sequence<float32> VERTEX_POSITION_COMPONENTS = {
				 0.5f, -0.5f,  0.5f,
				 0.5f,  0.5f,  0.5f,
				-0.5f,  0.5f,  0.5f,
				-0.5f, -0.5f,  0.5f,
				-0.5f, -0.5f, -0.5f,
				-0.5f,  0.5f, -0.5f,
				 0.5f,  0.5f, -0.5f,
				 0.5f, -0.5f, -0.5f,
			};

			static inline const sequence<uint32> TRIANGLE_VERTEX_POSITION_INDICES = {
				// Front (+Z)
				0, 1, 2,
				0, 2, 3,
				// Back (-Z)
				4, 5, 6,
				4, 6, 7,
				// Left (-X)
				3, 2, 5,
				3, 5, 4,
				// Right (+X)
				7, 6, 1,
				7, 1, 0,
				// Top (+Y)
				5, 2, 1,
				5, 1, 6,
				// Bottom (-Y)
				3, 4, 7,
				3, 7, 0,
			};

			static inline const sequence<float32> VERTEX_NORMAL_COMPONENTS = {
				 0.0f,  0.0f,  1.0f,
				 0.0f,  0.0f, -1.0f,
				-1.0f,  0.0f,  0.0f,
				 1.0f,  0.0f,  0.0f,
				 0.0f,  1.0f,  0.0f,
				 0.0f, -1.0f,  0.0f,
			};

			static inline const sequence<uint32> VERTEX_NORMAL_INDICES = {
				// Front (+Z)
				0, 0, 0,
				0, 0, 0,
				// Back (-Z)
				1, 1, 1,
				1, 1, 1,
				// Left (-X)
				2, 2, 2,
				2, 2, 2,
				// Right (+X)
				3, 3, 3,
				3, 3, 3,
				// Top (+Y)
				4, 4, 4,
				4, 4, 4,
				// Bottom (-Y)
				5, 5, 5,
				5, 5, 5,
			};

			static inline const sequence<float32> VERTEX_TEXTURE_COORDINATE_COMPONENTS = {
				0, 0,
				1, 0,
				0, 1,
				1, 1,
			};

			static inline const sequence<uint32> VERTEX_TEXTURE_COORDINATE_INDICES = {
				// Front (+Z)
				0, 2, 3,
				0, 3, 1,
				// Back (-Z)
				0, 2, 3,
				0, 3, 1,
				// Left (-X)
				0, 2, 3,
				0, 3, 1,
				// Right (+X)
				0, 2, 3,
				0, 3, 1,
				// Top (+Y)
				0, 2, 3,
				0, 3, 1,
				// Bottom (-Y)
				0, 2, 3,
				0, 3, 1,
			};

		public:
			explicit BoxGeometry3D(const vec3<float32>& size) {
				const vec3<float32> halfSize = size * 0.5f;

				vertexPositionComponents = VERTEX_POSITION_COMPONENTS;

				for (uint32 vertexIndex = 0; vertexIndex < VERTEX_POSITION_COMPONENTS.count(); vertexIndex += 3) {
					vertexPositionComponents[vertexIndex + 0] *= halfSize[0];
					vertexPositionComponents[vertexIndex + 1] *= halfSize[1];
					vertexPositionComponents[vertexIndex + 2] *= halfSize[2];
				}

				vertexPositionIndices = TRIANGLE_VERTEX_POSITION_INDICES;
				vertexNormalComponents = VERTEX_NORMAL_COMPONENTS;
				vertexNormalIndices = VERTEX_NORMAL_INDICES;
				vertexTextureCoordinateComponents = VERTEX_TEXTURE_COORDINATE_COMPONENTS;
				vertexTextureCoordinateIndices = VERTEX_TEXTURE_COORDINATE_INDICES;
			}
	};
}