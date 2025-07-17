#pragma once

namespace ProjectA {
	struct Body {};

	struct Body2D : public Body {
		// Applies a linear force to the body.
		virtual void applyForce(const vec2<float32>& force) = 0;

		// Applies a linear and angular impulse to the body.
		// - `j` is the impulse vector.
		// - `r` is the vector from the entity's position to the contact (referred to as "radius").
		virtual void applyImpulse(const vec2<float32>& j, const vec2<float32>& r) = 0;
	};
}