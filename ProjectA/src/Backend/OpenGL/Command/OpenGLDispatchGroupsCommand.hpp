#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLDispatchGroupsCommand : public OpenGLCommand {
		public:
			OpenGLDispatchGroupsCommand(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z) :
				num_groups_x(num_groups_x),
				num_groups_y(num_groups_y),
				num_groups_z(num_groups_z)
			{}

			void execute() const override {
				glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
			}

		private:
			const GLuint num_groups_x;
			const GLuint num_groups_y;
			const GLuint num_groups_z;
	};
}