#include "Config.h"

#include "Spritesheet.h"

#include "Debug/Debug.h"
#include "Renderer.h"
#include "Renderer/Sprite.h"

namespace Plop
{
	bool Spritesheet::GetSpriteUV(const glm::uvec2 &_vIndices, glm::vec2 &_vUVMin, glm::vec2 &_vUVMax, const glm::uvec2 _vSize /*= VEC2_1*/) const
	{
		ASSERTM(_vIndices.x < m_uNbColumn && _vIndices.y < m_uNbRow,
				"Spritesheet::GetSpriteUV: Wrong coordinates %u %u, spritesheet only defines %u columns and %u rows",
				_vIndices.x,
				_vIndices.y,
				m_uNbColumn,
				m_uNbRow);

		const float fWidth	= 1.f / m_uNbColumn;
		const float fHeight = 1.f / m_uNbRow;

		_vUVMin.x = _vIndices.x * fWidth;
		_vUVMax.x = _vUVMin.x + fWidth * _vSize.x;
		if constexpr (Renderer::USE_INVERTED_Y_UV)
		{
			_vUVMax.y = (m_uNbRow - _vIndices.y - 1) * fHeight;
			_vUVMin.y = _vUVMax.y + fHeight * _vSize.y;
		}
		else
		{
			_vUVMin.y = (m_uNbRow - _vIndices.y - 1) * fHeight;
			_vUVMax.y = _vUVMin.y + fHeight * _vSize.y;
		}

		return true;
	}

	bool Spritesheet::GetSpriteUV(const String &_alias, glm::vec2 &_vUVMin, glm::vec2 &_vUVMax, glm::ivec2 *_pIndices /*= nullptr*/) const
	{
		String sAliasLower = _alias;
		std::transform(sAliasLower.begin(), sAliasLower.end(), sAliasLower.begin(), [](unsigned char c) { return std::tolower(c); });

		auto itpair = m_mapAliases.find(sAliasLower);
		ASSERTM(itpair != m_mapAliases.end(), "Spritesheet::GetSpriteUV: Unknown alias %s", _alias);
		if (itpair != m_mapAliases.end())
		{
			if (_pIndices)
				*_pIndices = glm::ivec2(itpair->second.uColumn, itpair->second.uRow);

			return GetSpriteUV(glm::uvec2(itpair->second.uColumn, itpair->second.uRow), _vUVMin, _vUVMax, itpair->second.vSize);
		}

		return false;
	}

	TextureHandle Spritesheet::GetTextureHandle() const { return m_hTexture; }

	uint64_t Spritesheet::GetNativeHandle() const
	{
		if (m_hTexture)
			return m_hTexture->GetNativeHandle();

		return 0;
	}

	void from_json(const nlohmann::json &j, Spritesheet::SpriteAliasData &_data)
	{
		_data.uColumn = j.at("Column");
		_data.uRow	  = j.at("Row");
		if (j.contains("Source"))
			_data.sSource = String(j.at("Source"));
	}

	void to_json(nlohmann::json &j, const Spritesheet::SpriteAliasData &_data)
	{
		j["Colmun"] = _data.uColumn;
		j["Row"]	= _data.uRow;
		if (!_data.sSource.empty())
			j["Source"] = _data.sSource.string();
	}
} // namespace Plop