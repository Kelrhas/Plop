#pragma once

#include <entt/resource/handle.hpp>
#include <json.hpp>

#include "Assets/Asset.h"
#include "Renderer/Texture.h"

#include "SpriteFwd.h"

namespace Plop
{
	class Spritesheet;
	using SpritesheetPtr = std::shared_ptr<Spritesheet>;
	using SpritesheetHandle = entt::resource_handle<Spritesheet>;

	namespace AssetLoader
	{
		struct SpritesheetLoader;
	}

	class Spritesheet : public Asset
	{
		friend struct AssetLoader::SpritesheetLoader;

	public:

		struct SpriteAliasData
		{
			uint8_t uColumn = 0;
			uint8_t uRow = 0;
			glm::uvec2 vSize = VEC2_1;
			StringPath sSource;
		};

		Spritesheet() = default;
		virtual ~Spritesheet() = default;


		bool			GetSpriteUV( const glm::uvec2& _vIndices, glm::vec2& _vUVMin, glm::vec2& _vUVMax, const glm::uvec2 _vSize = VEC2_1 ) const;
		bool			GetSpriteUV( const String& _alias, glm::vec2& _vUVMin, glm::vec2& _vUVMax, glm::ivec2* _pIndices = nullptr ) const;

		TextureHandle	GetTextureHandle() const;
		uint64_t		GetNativeHandle() const;

		uint8_t			GetNbColumn() const { return m_uNbColumn; }
		uint8_t			GetNbRow() const { return m_uNbRow; }
		const auto&		GetAliasData() const { return m_mapAliases; }

	private:
		TextureHandle	m_hTexture;
		uint8_t			m_uNbColumn = 1;
		uint8_t			m_uNbRow = 1;

		std::map<String, SpriteAliasData> m_mapAliases;
	};


	void to_json( nlohmann::json& j, const Spritesheet::SpriteAliasData& _data );
	void from_json( const nlohmann::json& j, Spritesheet::SpriteAliasData& _data );
}
