#pragma once

#include "ProjectA.hpp"

#include "ParticleProperties.hpp"

class ParticleSystem {
	public:
		ParticleSystem(ProjectA::uint32 maxParticles);

		void update(ProjectA::float32 deltaTime);
		void render(const ProjectA::OrthographicCamera& camera);

		void emitParticle(const ParticleProperties& properties);

	private:
		struct Particle {
			lyah::vec2<ProjectA::float32> position;
			lyah::vec2<ProjectA::float32> linearVelocity;
			lyah::vec4<ProjectA::float32> startColor;
			lyah::vec4<ProjectA::float32> endColor;
			ProjectA::float32 rotation = 0.0f;
			ProjectA::float32 startSize;
			ProjectA::float32 endSize;
			ProjectA::float32 lifetime = 1.0f;
			ProjectA::float32 remainingLife = 0.0f;
			bool active = false;
		};

		std::vector<Particle> particlePool;

		ProjectA::uint32 poolIndex;
};