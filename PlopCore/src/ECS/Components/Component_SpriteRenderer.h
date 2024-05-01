#pragma once

#include <ECS/ComponentManager.h>
#include <Renderer/Sprite.h>

namespace Plop
{
	struct Component_SpriteRenderer
	{
		SpritePtr xSprite = nullptr;

		Component_SpriteRenderer();
		Component_SpriteRenderer(const Component_SpriteRenderer &);
		Component_SpriteRenderer(Component_SpriteRenderer &&);
		Component_SpriteRenderer &operator=(const Component_SpriteRenderer &_other);
		Component_SpriteRenderer &operator=(Component_SpriteRenderer &&_other);

		void EditorUI();
		json ToJson() const;
		void FromJson(const json& _j);

	};
}
