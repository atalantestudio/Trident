#pragma once

#include "Engine/API/Physics/Contact/Contact.hpp"

namespace ProjectA {
	struct Contact2DID;

	// A part of an object's geometry in world-space.
	class Feature2D {
		public:
			// NOTE
			// The vertex/edge union deletes the default constructor.
			// Because of that, structs that use Feature2D cannot be instantiated.
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

		private:
			// The number of vertices used.
			uint8 vertexCount : 1;
	};

	struct ClipVertex2D {
		vec2<float32> position;
		Contact2DID id;
	};

	class ClipFeature2D {
		public:
			// NOTE
			// The vertex/edge union deletes the default constructor,
			// and because of that structs that use ClipFeature2D cannot be constructed.
			ClipFeature2D() :
				edge{}
			{}

		public:
			// The vertices that compose the feature.
			// NOTE: The mesh transform is already applied on the feature's vertices.
			union {
				ClipVertex2D vertex;
				ClipVertex2D edge[2];
			};

			// The number of vertices used.
			uint8 vertexCount;
	};
}