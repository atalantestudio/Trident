// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/ECS/ScriptEntity.hpp"

namespace Trident {
	struct ScriptComponent {
		ScriptEntity* entity = nullptr;

		ScriptEntity* (*constructScript)();
		void (*destroyScript)(ScriptComponent*);

		template<typename T>
		void bind() {
			constructScript = []() {
				return pointer_cast<ScriptEntity*>(new T());
			};

			destroyScript = [](ScriptComponent* scriptComponent) {
				delete scriptComponent->entity;

				scriptComponent->entity = nullptr;
			};
		}
	};
}