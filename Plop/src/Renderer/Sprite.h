#pragma once

#include <Renderer/Texture.h>

namespace Plop
{
	class Sprite
	{
	public:
		Sprite() = default;
		~Sprite() = default;

				void				SetTexture( const TexturePtr& _xTexture );
				const TexturePtr&	GetTexture() const { return m_xTexture; }
				void				SetUV( const glm::vec2& _vUVMin, const glm::vec2& _vUVMax );
				const glm::vec2&	GetUVMin() const { return m_vUVMin; }
				const glm::vec2&	GetUVMax() const { return m_vUVMax; }
				void				SetTint( const glm::vec4& _vTint );
				const glm::vec4&	GetTint() const { return m_vTint; }

	private:
		TexturePtr		m_xTexture = nullptr;
		glm::vec2		m_vUVMin = glm::vec2( 0.f );
		glm::vec2		m_vUVMax = glm::vec2( 1.f );
		glm::vec4		m_vTint = glm::vec4( 1.f );
	};
}
