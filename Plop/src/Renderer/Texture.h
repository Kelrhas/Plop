#pragma once

#include <entt/resource/handle.hpp>

namespace Plop
{
	class Texture;
	using TexturePtr = std::shared_ptr<Texture>;
	using TextureHandle = entt::resource_handle<Texture>;

	class Texture
	{
	public:
		using FlagsType = uint8_t;
		enum class Flags : FlagsType
		{
			NONE				= 0,
			UV_REPEAT			= 1 << 1,
		};

		virtual ~Texture() {}

		virtual void BindSlot(int _iSlot) const = 0;
		virtual void SetData( void* _pData ) = 0;

		virtual bool Compare( const Texture& _other ) const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual const char* GetName() const { return m_sName.c_str(); }
		virtual uint64_t GetNativeHandle() const = 0;
		
		static TexturePtr Create2D( uint32_t _uWidth, uint32_t _uHeight, FlagsType _eFlags, void* _pData, const String& _sName = "" );
		
	protected:
		String m_sName;
	};

	class Texture2D : public Texture
	{
	public:
		uint32_t GetWidth() const override { return m_uWidth; }
		uint32_t GetHeight() const override { return m_uHeight; }

	protected:
		uint32_t m_uWidth;
		uint32_t m_uHeight;
	};
}
