#include "Plop_pch.h"
#include "Sprite.h"

namespace Plop
{
	void Sprite::SetTexture( const TexturePtr& _xTexture )
	{
		m_xTexture = _xTexture;
	}

	void Sprite::SetUV( const glm::vec2& _vUVMin, const glm::vec2& _vUVMax )
	{
		m_vUVMin.x = glm::clamp( std::min( _vUVMin.x, _vUVMax.x ), 0.f, 1.f );
		m_vUVMin.y = glm::clamp( std::min( _vUVMin.y, _vUVMax.y ), 0.f, 1.f );
		m_vUVMax.x = glm::clamp( std::max( _vUVMax.x, _vUVMax.x ), 0.f, 1.f );
		m_vUVMax.y = glm::clamp( std::max( _vUVMax.y, _vUVMax.y ), 0.f, 1.f );
	}

	void Sprite::SetSpriteIndex( uint32_t _uXIndex, uint32_t _uYIndex, uint32_t _uMaxHorizontal, uint32_t _uMaxVertical )
	{
		const float fWidth = 1.f / _uMaxHorizontal;
		const float fHeight = 1.f / _uMaxVertical;

		m_vUVMin.x = _uXIndex * fWidth;
		m_vUVMin.y = _uYIndex * fHeight;
		m_vUVMax.x = (_uXIndex + 1) * fWidth;
		m_vUVMax.y = (_uYIndex + 1) * fHeight;
	}

	void Sprite::SetTint( const glm::vec4& _vTint )
	{
		m_vTint = _vTint;
	}

}
