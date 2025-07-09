#pragma once

namespace ProjectA {
	struct Body {};

	struct Body2D : public Body {
		virtual void applyLinearForce(const vec2<float32>& force) = 0;
	};
}