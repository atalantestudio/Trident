#include "ParticleSystem.hpp"
#include "Random.hpp"

ParticleSystem::ParticleSystem(const ProjectA::uint32 maxParticles) : poolIndex(maxParticles - 1) {
	CLIENT_ASSERT(maxParticles > 0, "Max particles should be > 1");

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
	ProjectA::Renderer2D::beginScene(camera);

	for (const Particle& particle : particlePool) {
		if (!particle.active) {
			continue;
		}

		const ProjectA::float32 remainingLife = particle.remainingLife / particle.lifetime;
		const lyah::vec4<ProjectA::float32> color = lyah::lerp(particle.endColor, particle.startColor, remainingLife);

		// TODO [LYAH]: Scalar linear interpolation
		const ProjectA::float32 size = (1 - remainingLife) * particle.endSize + remainingLife * particle.startSize;

		const lyah::vec3<ProjectA::float32> position = {particle.position[0], particle.position[1], -0.2f};

		ProjectA::Renderer2D::drawRotatedQuad(position, {size, size}, particle.rotation, color);
	}

	ProjectA::Renderer2D::endScene();
}

void ParticleSystem::emitParticle(const ParticleProperties& properties) {
	Particle& particle = particlePool[poolIndex];

	particle.active = true;
	particle.lifetime = properties.lifetime;
	particle.remainingLife = properties.lifetime;

	particle.position = properties.position;
	particle.rotation = Random::get<ProjectA::float32>() * 2.0f * lyah::pi<ProjectA::float32>();

	ProjectA::float32 linearVelocityX = properties.linearVelocityBias[0] * (Random::get<ProjectA::float32>() - 0.5f);
	ProjectA::float32 linearVelocityY = properties.linearVelocityBias[1] * (Random::get<ProjectA::float32>() - 0.5f);
	particle.linearVelocity = properties.linearVelocity;
	particle.linearVelocity += {linearVelocityX, linearVelocityY};

	particle.startColor = properties.startColor;
	particle.endColor = properties.endColor;

	particle.startSize = properties.startSize + properties.sizeBias * (Random::get<ProjectA::float32>() - 0.5f);
	particle.endSize = properties.endSize;

	poolIndex--;
	poolIndex %= particlePool.size();
}