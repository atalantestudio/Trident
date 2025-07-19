#pragma once

namespace ProjectA {
	// A part of an object's geometry in world-space.
	class Feature2D {
		public:
			// NOTE: The vertex/edge union deletes the default constructor,
			//       and because of that structs that use Feature2D cannot be constructed.
			Feature2D() {}

			uint32 getVertexCount() const {
				return vertexCount + 1;
			}

			void setVertexCount(uint32 vertexCount) {
				this->vertexCount = vertexCount - 1;
			}

		public:
			// The vertices that compose the feature.
			// NOTE: The mesh transform is already applied on the feature's vertices.
			union {
				vec2<float32> vertex;
				vec2<float32> edge[2];
			};

			// TODO: `vertexIndex`/`edgeIndices` union is deprecated; use `vertex`/`edge` union instead.
			//       This is to account for shapes which are not represented by vertex geometry, e.g. a circle.
			union {
				uint32 vertexIndex;
				uint32 edgeIndices[2];
			};

		private:
			// The number of vertices used.
			flag vertexCount : 1;
	};
}