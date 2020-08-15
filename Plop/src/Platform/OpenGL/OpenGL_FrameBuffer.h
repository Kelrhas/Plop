#pragma once
#include <Renderer/FrameBuffer.h>

namespace Plop
{
	class OpenGL_FrameBuffer : public FrameBuffer
	{
	public:
		OpenGL_FrameBuffer( uint32_t _uWidth, uint32_t _uHeight );
		virtual ~OpenGL_FrameBuffer();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual uint32_t GetID() const override { return m_uID; }
		virtual uint32_t GetColorID() const override { return m_uColorBuffer; }

				void Recreate();

	private:
		uint32_t m_uID;
		uint32_t m_uColorBuffer;
		uint32_t m_uDepthBuffer;
	};
}
