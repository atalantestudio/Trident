#define RUNTIME_APPLICATION_PHYSICS_2D
//#define RUNTIME_APPLICATION_SAMPLE

// Select the application to run.
#define RUNTIME_APPLICATION RUNTIME_APPLICATION_PHYSICS_2D

#ifdef RUNTIME_APPLICATION_PHYSICS_2D
	#include "Physics2D/Physics2DApplication.hpp"
#endif

#ifdef RUNTIME_APPLICATION_SAMPLE
	#include "Sample/SampleApplication.hpp"
#endif