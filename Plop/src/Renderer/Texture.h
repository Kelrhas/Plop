#pragma once

namespace Plop
{
	class Texture;
	using TexturePtr = std::shared_ptr<Texture>;

	class Texture
	{
	public:
		virtual ~Texture() {}

		virtual void BindSlot(int _iSlot) const = 0;
		virtual void SetData( void* _pData ) = 0;

		virtual bool Compare( const Texture& _other ) = 0;

		static TexturePtr Create2D( const String& _sFile );
		static TexturePtr Create2D( uint32_t _uWidth, uint32_t _uHeight, void* _pData );
	};

	class Texture2D : public Texture
	{
	public:
		uint32_t GetWidth() const { return m_uWidth; }
		uint32_t GetHeight() const { return m_uHeight; }

	protected:
		uint32_t m_uWidth;
		uint32_t m_uHeight;
	};
}
