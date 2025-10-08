/* #include "pch.hpp"

#include "ParticleSystem.hpp"
#include "Random.hpp"

ParticleSystem::ParticleSystem(ProjectA::uint32 maxParticles) : poolIndex(maxParticles - 1) {
	particlePool.resize(maxParticles);
}

void ParticleSystem::update(ProjectA::float32 deltaTime) {
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

void ParticleSystem::render(const ProjectA::OrthographicCamera& camera) {
	for (const Particle& particle : particlePool) {
		if (!particle.active) {
			continue;
		}

		const ProjectA::float32 remainingLife = particle.remainingLife / particle.lifetime;
		const ProjectA::vec4<ProjectA::float32> color = ProjectA::lerp(particle.endColor, particle.startColor, remainingLife);

		const ProjectA::float32 size = ProjectA::lerp(particle.startSize, particle.endSize, 1.0f - remainingLife);

		const ProjectA::vec3<ProjectA::float32> position = {particle.position[0], particle.position[1], -0.2f};
	}
}

void ParticleSystem::emitParticle(const ParticleProperties& properties) {
	Particle& particle = particlePool[poolIndex];

	particle.active = true;
	particle.lifetime = properties.lifetime;
	particle.remainingLife = properties.lifetime;

	particle.position = properties.position;
	particle.rotation = Random::get<ProjectA::float32>() * 2.0f * ProjectA::pi<ProjectA::float32>();

	ProjectA::float32 linearVelocityX = properties.linearVelocityBias[0] * (Random::get<ProjectA::float32>() - 0.5f);
	ProjectA::float32 linearVelocityY = properties.linearVelocityBias[1] * (Random::get<ProjectA::float32>() - 0.5f);
	particle.linearVelocity = properties.linearVelocity;
	particle.linearVelocity += {linearVelocityX, linearVelocityY};

	particle.startColor = properties.startColor;
	particle.endColor = properties.endColor;

	particle.startSize = properties.startSize + properties.sizeBias * (Random::get<ProjectA::float32>() - 0.5f);
	particle.endSize = properties.endSize;

	poolIndex -= 1;
	poolIndex %= particlePool.size();
} */