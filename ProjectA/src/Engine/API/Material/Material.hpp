#pragma once

namespace ProjectA {
	/*
		TODO: Include default material presets?

		https://code.tutsplus.com/how-to-create-a-custom-2d-physics-engine-the-core-engine--gamedev-7493t

		| Type       | Density | COR  |
		| ---------- | ------- | -----|
		| Rock       | 0.6     | 0.1  |
		| Wood       | 0.3     | 0.2  |
		| Metal      | 1.2     | 0.05 |
		| BouncyBall | 0.3     | 0.8  |
		| SuperBall  | 0.3     | 0.95 |
		| Pillow     | 0.1     | 0.2  |
		| Static     | 0.0     | 0.4  |
	*/
	struct Material {
		// TODO: Validate data.
		//       ENGINE_ASSERT(density > 0.0f, "The density must be positive.");
		//       ENGINE_ASSERT(coefficientOfRestitution >= 0.0f && coefficientOfRestitution <= 1.0f, "The coefficient of restitution must be between 0 and 1.");

		// Mass is assumed to be uniformly distributed.
		float32 density;

		float32 coefficientOfRestitution;
	};
}