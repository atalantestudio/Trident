#pragma once

#define ENGINE_ERROR_UNIMPLEMENTED() \
	::ProjectA::Logger::engineLogger.error("This feature is not implemented."); \
	ABORT()

#define ENGINE_ERROR_UNSUPPORTED() \
	::ProjectA::Logger::engineLogger.error("This feature is not supported."); \
	ABORT()