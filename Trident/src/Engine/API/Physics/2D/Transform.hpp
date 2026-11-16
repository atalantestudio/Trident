// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// Clockwise angle.
	struct Rotation2D {
		Rotation2D() {}

		Rotation2D(float32 s, float32 c) :
			s(s),
			c(c)
		{}

		Rotation2D(float32 angle) :
			s(sin(angle)),
			c(cos(angle))
		{}

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

	inline Rotation2D normalized(Rotation2D q) {
		const float32 length = sqrt(q.s * q.s + q.c * q.c);

		float32 inverseLength = 0.0f;

		if (length > 0.0f) {
			inverseLength = 1.0f / length;
		}

		q.s *= inverseLength;
		q.c *= inverseLength;

		return q;
	}

	inline Rotation2D integrate(Rotation2D q, float32 deltaAngle) {
		return normalized({
			q.s + deltaAngle * q.c,
			q.c - deltaAngle * q.s,
		});
	}

	inline Rotation2D inverse(Rotation2D q) {
		q.s = -q.s;

		return q;
	}

	// CW
	inline vec2<float32> operator*(vec2<float32> a, Rotation2D q) {
		return {
			 a.x * q.c + a.y * q.s,
			-a.x * q.s + a.y * q.c,
		};
	}

	inline vec2<float32> operator*=(vec2<float32>& a, Rotation2D q) {
		return a = a * q;
	}

	// CW
	inline vec2<float32> operator*(vec2<float32> a, Transform2D t) {
		return a * t.rotation + t.position;
	}

	inline vec2<float32> operator*=(vec2<float32>& a, Transform2D t) {
		return a = a * t;
	}

	inline Transform2D operator*(Transform2D tA, Transform2D tB) {
		Transform2D t;
		t.rotation = tA.rotation * tB.rotation;
		t.position = tA.position * tB.rotation + tB.position;
		// tB.rotation = tA.rotation * tB.rotation;
		// tB.position = tB.position * tA.rotation + tA.position;

		return t;
	}

	inline Transform2D inverse(Transform2D t) {
		t.rotation = inverse(t.rotation);
		t.position = -t.position * t.rotation;

		return t;
	}
}