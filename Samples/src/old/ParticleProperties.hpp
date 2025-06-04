#pragma once

#include "ProjectA.hpp"

struct ParticleProperties {
	ProjectA::vec2<ProjectA::float32> position;
	ProjectA::vec2<ProjectA::float32> linearVelocity;
	ProjectA::vec2<ProjectA::float32> linearVelocityBias;
	ProjectA::vec4<ProjectA::float32> startColor;
	ProjectA::vec4<ProjectA::float32> endColor;
	ProjectA::float32 lifetime = 1.0f;
	ProjectA::float32 startSize;
	ProjectA::float32 endSize;
	ProjectA::float32 sizeBias;
};