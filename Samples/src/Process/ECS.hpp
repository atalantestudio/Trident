// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/ECS/ECS.hpp"
#include "Engine/ECS/Tag.hpp"

namespace Trident {
	class ProcessECSProcess : public Process {
		private:
			struct CharacterTraits {
				uint32 age;
				float32 weight;
			};

			DEFINE_TAG_COMPONENT(IsMainCharacter);

		public:
			sint32 main(view<void*> arguments) override {
				ECS ecs;
				ecs.registerComponentPool<CharacterTraits>();

				const Entity bob = ecs.createEntity();
				const Entity patrick = ecs.createEntity();

				ecs.setComponent<CharacterTraits>(bob, {19, 45.3_kg});
				ecs.setComponent<CharacterTraits>(patrick, {21, 74.8_kg});

				ENGINE_DEBUG("Bob\n- Age: []\n- Weight: []kg", ecs.getComponent<CharacterTraits>(bob).age, ecs.getComponent<CharacterTraits>(bob).weight);
				ENGINE_DEBUG("Patrick\n- Age: []\n- Weight: []kg", ecs.getComponent<CharacterTraits>(patrick).age, ecs.getComponent<CharacterTraits>(patrick).weight);

				ecs.registerTagPool<IsMainCharacter>();
				ecs.setTag<IsMainCharacter>(bob);

				ENGINE_DEBUG("Is Bob tagged with IsMainCharacter: []", ecs.hasTag<IsMainCharacter>(bob));
				ENGINE_DEBUG("Is Patrick tagged with IsMainCharacter: []", ecs.hasTag<IsMainCharacter>(patrick));

				ecs.resetTag<IsMainCharacter>(bob);

				ENGINE_INFO("Removed Bob IsMainCharacter tag.");

				ENGINE_DEBUG("Is Bob tagged with IsMainCharacter: []", ecs.hasTag<IsMainCharacter>(bob));

				ecs.resetComponent<CharacterTraits>(patrick);

				ENGINE_INFO("Patrick died.");

				ENGINE_DEBUG("Does Patrick have CharacterTraits: []", ecs.hasComponent<CharacterTraits>(patrick));

				return 0;
			}
	};
}