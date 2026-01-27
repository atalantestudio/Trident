/* #include "pch.hpp"

#include "ParticleSystem.hpp"
#include "Random.hpp"

ParticleSystem::ParticleSystem(Trident::uint32 maxParticles) : poolIndex(maxParticles - 1) {
	particlePool.resize(maxParticles);
}

void ParticleSystem::update(Trident::float32 deltaTime) {
	for (Particle& particle : particlePool) {
		if (!particle.active) {
			continue;
		}

		if (particle.remainingLife <= 0.0f) {
			particle.active = false;

			continue;
		}

		particle.remainingLife -= deltaTime;
		particle.position += particle.linearVelocity * deltaTime;
		particle.rotation += 0.01f * deltaTime;
	}
}

void ParticleSystem::render(const Trident::OrthographicCamera& camera) {
	for (const Particle& particle : particlePool) {
		if (!particle.active) {
			continue;
		}

		const Trident::float32 remainingLife = particle.remainingLife / particle.lifetime;
		const Trident::vec4<Trident::float32> color = Trident::lerp(particle.endColor, particle.startColor, remainingLife);

		const Trident::float32 size = Trident::lerp(particle.startSize, particle.endSize, 1.0f - remainingLife);

		const Trident::vec3<Trident::float32> position = {particle.position[0], particle.position[1], -0.2f};
	}
}

void ParticleSystem::emitParticle(const ParticleProperties& properties) {
	Particle& particle = particlePool[poolIndex];

	particle.active = true;
	particle.lifetime = properties.lifetime;
	particle.remainingLife = properties.lifetime;

	particle.position = properties.position;
	particle.rotation = Random::get<Trident::float32>() * 2.0f * Trident::pi<Trident::float32>();

	Trident::float32 linearVelocityX = properties.linearVelocityBias[0] * (Random::get<Trident::float32>() - 0.5f);
	Trident::float32 linearVelocityY = properties.linearVelocityBias[1] * (Random::get<Trident::float32>() - 0.5f);
	particle.linearVelocity = properties.linearVelocity;
	particle.linearVelocity += {linearVelocityX, linearVelocityY};

	particle.startColor = properties.startColor;
	particle.endColor = properties.endColor;

	particle.startSize = properties.startSize + properties.sizeBias * (Random::get<Trident::float32>() - 0.5f);
	particle.endSize = properties.endSize;

	poolIndex -= 1;
	poolIndex %= particlePool.size();
} */