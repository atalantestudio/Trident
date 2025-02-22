#pragma once

#include "ProjectA.hpp"

struct ParticleProperties {
	lyah::vec2<ProjectA::float32> position;
	lyah::vec2<ProjectA::float32> linearVelocity;
	lyah::vec2<ProjectA::float32> linearVelocityBias;
	lyah::vec4<ProjectA::float32> startColor;
	lyah::vec4<ProjectA::float32> endColor;
	ProjectA::float32 lifetime = 1.0f;
	ProjectA::float32 startSize;
	ProjectA::float32 endSize;
	ProjectA::float32 sizeBias;
};