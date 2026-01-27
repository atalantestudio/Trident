// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

/* #pragma once

#include "Trident.hpp"

#include "ParticleProperties.hpp"

class ParticleSystem {
	public:
		ParticleSystem(Trident::uint32 maxParticles);

		void update(Trident::float32 deltaTime);
		void render(const Trident::OrthographicCamera& camera);

		void emitParticle(const ParticleProperties& properties);

	private:
		struct Particle {
			Trident::vec2<Trident::float32> position;
			Trident::vec2<Trident::float32> linearVelocity;
			Trident::vec4<Trident::float32> startColor;
			Trident::vec4<Trident::float32> endColor;
			Trident::float32 rotation = 0.0f;
			Trident::float32 startSize;
			Trident::float32 endSize;
			Trident::float32 lifetime = 1.0f;
			Trident::float32 remainingLife = 0.0f;
			bool active = false;
		};

		std::vector<Particle> particlePool;

		Trident::uint32 poolIndex;
}; */