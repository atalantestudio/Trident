#pragma once

#include "ProjectA.hpp"

#include "ParticleProperties.hpp"

class ParticleSystem {
	public:
		ParticleSystem(ProjectA::uint32 maxParticles);

		void update(ProjectA::float32 deltaTime);
		void render(const ProjectA::OrthographicCamera3D& camera);

		void emitParticle(const ParticleProperties& properties);

	private:
		struct Particle {
			ProjectA::vec2<ProjectA::float32> position;
			ProjectA::vec2<ProjectA::float32> linearVelocity;
			ProjectA::vec4<ProjectA::float32> startColor;
			ProjectA::vec4<ProjectA::float32> endColor;
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