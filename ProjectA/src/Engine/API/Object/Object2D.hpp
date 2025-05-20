#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Material/Material.hpp"

namespace ProjectA {
	class Scene2D;

	struct Object2DDescriptor {
		uint32 geometryIndex;
		uint32 materialIndex;
	};

	class Object2D {
		public:
			explicit Object2D(const Object2DDescriptor& descriptor, const Scene2D* scene);

			const ref<Geometry2D>& getGeometry() const;

			const ref<Material>& getMaterial() const;

			void updateTransform();

			void applyLinearForce(const vec2<float32>& linearForce);

			void applyAngularForce(float32 angularForce);

		public:
			uint32 geometryIndex;
			uint32 materialIndex;

			// Position of the center of mass in world space.
			vec2<float32> position = {0.0f, 0.0f};

			// TODO
			// Position of the object in the previous frame,
			// used for Verlet integration.
			//vec2<float32> previousPosition = {0.0f, 0.0f};

			vec2<float32> linearVelocity = {0.0f, 0.0f};
			vec2<float32> linearAcceleration = {0.0f, 0.0f};

			float32 rotation = 0.0f;

			// TODO
			// Rotation of the object in the previous frame,
			// used for Verlet integration.
			//float32 previousRotation = 0.0f;

			float32 angularVelocity = 0.0f;
			float32 angularAcceleration = 0.0f;

			vec2<float32> scale = {1.0f, 1.0f};

			mat3x3<float32> transform = mat3x3<float32>::identity();

		private:
			const Scene2D* scene;
	};
}