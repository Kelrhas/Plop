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

	void Sprite::SetTint( const glm::vec4& _vTint )
	{
		m_vTint = _vTint;
	}

}
