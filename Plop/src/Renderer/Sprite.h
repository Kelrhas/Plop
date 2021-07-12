#pragma once

#include <Renderer/Spritesheet.h>

#include "SpriteFwd.h"

namespace Plop
{
	class Sprite
	{
	public:
		Sprite() = default;
		~Sprite() = default;

		operator bool() const;

				void				SetSpritesheet( SpritesheetHandle _hSpritesheet, const String& _sAlias );
				void				SetSpritesheet( SpritesheetHandle _hSpritesheet, const glm::uvec2& _vIndices );
				SpritesheetHandle	GetSpritesheetHandle() const;
				TextureHandle		GetTextureHandle() const;

				//void				SetUV( const glm::vec2& _vUVMin, const glm::vec2& _vUVMax );
				//void				SetSpriteIndex( const glm::uvec2& _vSpriteIndex, const glm::uvec2& _vSpriteSheetCount, const glm::uvec2& _vSpriteSize = { 1, 1 } );
				
				const String&		GetAlias() const { return m_sSpriteAlias; }
				const glm::ivec2&	GetCoord() const { return m_vSpriteCoord; }

				const glm::vec2&	GetUVMin() const { return m_vUVMin; }
				const glm::vec2&	GetUVMax() const { return m_vUVMax; }

				void				SetTint( const glm::vec4& _vTint );
				const glm::vec4&	GetTint() const { return m_vTint; }
				glm::vec4&			GetTint() { return m_vTint; }

	private:
		SpritesheetHandle	m_hSpritesheet;
		String				m_sSpriteAlias;
		glm::ivec2			m_vSpriteCoord = -VEC2_1;

		glm::vec2			m_vUVMin = glm::vec2( 0.f );
		glm::vec2			m_vUVMax = glm::vec2( 1.f );
		glm::vec4			m_vTint = glm::vec4( 1.f );
	};
}
