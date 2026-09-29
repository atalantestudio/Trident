// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	bool pointTriangleIntersection(vec3<float32> a, vec3<float32> b, vec3<float32> c, vec3<float32> p) {
		const vec3<float32> ab = b - a;
		const vec3<float32> ac = c - a;
		const vec3<float32> ap = p - a;

		const float32 ab_ab = dot(ab, ab);
		const float32 ab_ac = dot(ab, ac);
		const float32 ac_ac = dot(ac, ac);
		const float32 ap_ab = dot(ap, ab);
		const float32 ap_ac = dot(ap, ac);

		const float32 inverseD = 1.0f / (ab_ab * ac_ac - ab_ac * ab_ac);

		const float32 v = (ac_ac * ap_ab - ab_ac * ap_ac) * inverseD;
		const float32 w = (ab_ab * ap_ac - ab_ac * ap_ab) * inverseD;
		const float32 u = 1.0f - w - v;

		return u > 0.0f && v > 0.0f && w > 0.0f;
	}
}