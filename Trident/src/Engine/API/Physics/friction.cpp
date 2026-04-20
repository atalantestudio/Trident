	// Friction
	vec3 t = relativeVel - (relativeNorm * Dot(relativeVel, relativeNorm));

	if (CMP(MagnitudeSq(t), 0.0f)) {
		return;
	}

	Normalize(t);

	numerator = -Dot(relativeVel, t);
	d2 = cross(cross(r1, t) * i1, r1);
	d3 = cross(cross(r2, t) * i2, r2);
	denominator = invMassSum + Dot(d2 + d3, t);

	float jt = (denominator == 0.0f) ? 0.0f : numerator / denominator;
	if (M.contacts.size() > 0.0f && jt != 0.0f) {
		jt /= (float)M.contacts.size();
	}

	if (CMP(jt, 0.0f)) {
		return;
	}

	vec3 tangentImpuse;

#ifdef DYNAMIC_FRICTION
	float sf = sqrtf(A.staticFriction * B.staticFriction);
	float df = sqrtf(A.dynamicFriction * B.dynamicFriction);
	if (fabsf(jt) < j * sf) {
		tangentImpuse = t * jt;
	}
	else {
		tangentImpuse = t * -j * df;
	}
#else
	float friction = sqrtf(A.friction * B.friction);
	if (jt > j * friction) {
		jt = j * friction;
	}
	else if (jt < -j * friction) {
		jt = -j * friction;
	}
	tangentImpuse = t * jt;
#endif

	A.velocity -= tangentImpuse * invMass1;
	A.angVel -= cross(r1, tangentImpuse) * i1;

	B.velocity += tangentImpuse * invMass2;
	B.angVel += cross(r2, tangentImpuse) * i2;