#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace ProjectA {
	class OpenGLDispatchCommand final : public OpenGLCommand {
		public:
			explicit OpenGLDispatchCommand(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z) :
				num_groups_x(num_groups_x),
				num_groups_y(num_groups_y),
				num_groups_z(num_groups_z)
			{}

			void execute() const final override {
				#if OPENGL_VERSION >= OPENGL_VERSION_4_3
					glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif
			}

		private:
			const GLuint num_groups_x;
			const GLuint num_groups_y;
			const GLuint num_groups_z;
	};
}