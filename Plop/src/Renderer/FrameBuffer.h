#pragma once

namespace Plop
{
	class FrameBuffer;
	using FrameBufferPtr = std::shared_ptr<FrameBuffer>;

	class FrameBuffer
	{
	public:
		FrameBuffer( uint32_t _uWidth, uint32_t _uHeight )
			: m_uWidth( _uWidth ), m_uHeight( _uHeight ) {}
		virtual ~FrameBuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize( uint32_t _uWidth, uint32_t _uHeight ) = 0;

		virtual uint32_t GetID() const = 0;
		virtual uint32_t GetColorID() const = 0;

				uint32_t GetWidth() const { return m_uWidth; }
				uint32_t GetHeight() const { return m_uHeight; }

		static FrameBufferPtr Create(uint32_t _uWidth, uint32_t _uHeight);

	protected:
		uint32_t m_uWidth;
		uint32_t m_uHeight;
	};
}
