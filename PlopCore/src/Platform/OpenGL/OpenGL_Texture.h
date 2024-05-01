#pragma once

#include "Renderer/Texture.h"

#include <Gl/glew.h>

namespace Plop
{
	class OpenGL_Texture2D : public Texture2D
	{
	public:
		OpenGL_Texture2D(const StringPath &_sFile);
		OpenGL_Texture2D( uint32_t _uWidth, uint32_t _uHeight, FlagsType _eFlags, void* _pData, const String& _sName = "" );
		virtual ~OpenGL_Texture2D();

		// from Texture
		virtual void BindSlot( int _iSlot ) const override;
		virtual void SetData( void* _pData ) override;

		virtual uint64_t GetNativeHandle() const override { return m_uID; }

		virtual bool Compare( const Texture& _other ) const override;

	private:
		GLuint m_uID;
		GLenum m_eDataFormat;
	};
}
