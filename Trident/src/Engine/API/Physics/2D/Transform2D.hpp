// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/Base.hpp"

namespace Trident {
	struct Rotation2D {
		float32 getAngle() const {
			return atan2(s, c);
		}

		void setAngle(float32 angle) {
			s = sin(angle);
			c = cos(angle);
		}

		float32 s;
		float32 c;
	};

	struct Transform2D {
		Transform2D() :
			position(0.0f, 0.0f),
			rotation{0.0f, 1.0f}
		{}

		vec2<float32> position;
		Rotation2D rotation;
	};

	// CCW
	inline Rotation2D operator*(Rotation2D q, Rotation2D r) {
		Rotation2D qr;
		qr.s = q.c * r.s - q.s * r.c;
		qr.c = q.c * r.c + q.s * r.s;

		return qr;
	}

	// TODO(physics/2d/transform): Refactor.
	inline Rotation2D normalized(Rotation2D q) {
		const float32 length = sqrtf(q.s * q.s + q.c * q.c);

		float32 inverseLength = 0.0f;

		if (length > 0.0f) {
			inverseLength = 1.0f / length;
		}

		return {q.c * inverseLength, q.s * inverseLength};
	}

	// TODO(physics/2d/transform): Refactor.
	inline Rotation2D integrateRotation(Rotation2D q1, float32 deltaAngle) {
		const Rotation2D q2 = {
			q1.c - deltaAngle * q1.s,
			q1.s + deltaAngle * q1.c,
		};

		const float32 mag = sqrt( q2.s * q2.s + q2.c * q2.c );
		const float32 invMag = mag > 0.0f ? 1.0f / mag : 0.0f;

		return {q2.c * invMag, q2.s * invMag};
	}

	inline Rotation2D inverse(Rotation2D q) {
		q.s = -q.s;

		return q;
	}

	// CCW
	inline vec2<float32> operator*(vec2<float32> a, Rotation2D q) {
		return {
			a.x * q.c - a.y * q.s,
			a.x * q.s + a.y * q.c,
		};
	}

	inline vec2<float32> operator*=(vec2<float32>& a, Rotation2D q) {
		return a = a * q;
	}

	// CCW
	inline vec2<float32> operator*(vec2<float32> v, Transform2D T) {
		const float32 x = (T.rotation.c * v.x - T.rotation.s * v.y) + T.position.x;
		const float32 y = (T.rotation.s * v.x + T.rotation.c * v.y) + T.position.y;

		return {x, y};
	}

	// TODO(physics/2d/transform): Refactor.
	/* inline vec2<float32> mulT(vec2<float32> v, Transform2D T) {
		const float32 px = v.x - T.p.x;
		const float32 py = v.y - T.p.y;
		const float32 x = (T.q.c * px + T.q.s * py);
		const float32 y = (-T.q.s * px + T.q.c * py);

		return {x, y};
	} */

	inline vec2<float32> operator*=(vec2<float32>& a, Transform2D t) {
		return a = a * t;
	}

	inline Transform2D operator*(Transform2D tA, Transform2D tB) {
		Transform2D t;
		t.rotation = tA.rotation * tB.rotation;
		t.position = tA.position * tB.rotation + tB.position;

		return t;
	}

	inline Transform2D inverse(Transform2D t) {
		t.rotation = inverse(t.rotation);
		t.position = -t.position * t.rotation;

		return t;
	}
}