#include "pch.hpp"

#define RUNTIME_APPLICATION_PHYSICS_2D 0
#define RUNTIME_APPLICATION_SAMPLE 1

// Select the application to run.
#define RUNTIME_APPLICATION RUNTIME_APPLICATION_PHYSICS_2D

#if RUNTIME_APPLICATION == RUNTIME_APPLICATION_PHYSICS_2D
	#include "Physics2D/Physics2DApplication.hpp"
#endif

#if RUNTIME_APPLICATION == RUNTIME_APPLICATION_SAMPLE
	#include "Sample/SampleApplication.hpp"
#endif