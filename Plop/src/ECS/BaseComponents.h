#pragma once

#include <Renderer/Sprite.h>

namespace Plop
{
	struct NameComponent
	{
		NameComponent() = delete;
		NameComponent( const String& _sName ) : sName( _sName ) {}

		String sName;
	};

	struct TransfomComponent
	{
		TransfomComponent() = default;
		TransfomComponent( const glm::mat4& _mTransform ) : mTransform( _mTransform ) {}
		// TRS constructor
		TransfomComponent( const glm::vec3& _vTranslation, const glm::quat& _qRotation, const glm::vec3& _vScale )
		{
			Debug::TODO( "test this" );
			mTransform = glm::translate( glm::identity<glm::mat4>(), _vTranslation );
			mTransform = mTransform * (glm::mat4)_qRotation;
			mTransform = glm::scale( mTransform, _vScale );
		}

		operator glm::mat4() const { return mTransform; }

		glm::mat4 mTransform = glm::identity<glm::mat4>();
	};

	struct SpriteRenderer
	{
		SpritePtr xSprite = nullptr;
	};

}
