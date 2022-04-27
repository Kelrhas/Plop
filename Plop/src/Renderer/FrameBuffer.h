#pragma once

namespace Plop
{
	class FrameBuffer;
	using FrameBufferPtr = std::shared_ptr<FrameBuffer>;

	class FrameBuffer
	{
	public:
		enum class TextureFormat
		{
			NONE = 0,
			RGBA8,
			UINT32,
			DEPTH24S8
		};

		struct Specification
		{
			struct RenderTargetSpecs
			{
				TextureFormat eFormat;

				RenderTargetSpecs() = default;
				RenderTargetSpecs(TextureFormat _eFormat) : eFormat(_eFormat) {}
			};

			uint32_t uWidth = 0;
			uint32_t uHeight = 0;
			std::vector<RenderTargetSpecs> vecRenderTargetSpecs;
		};

		virtual ~FrameBuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize( uint32_t _uWidth, uint32_t _uHeight ) = 0;
		virtual uint32_t ReadPixelUInt32(uint32_t _renderTargetIndex, int _x, int _y) = 0;

		virtual void ClearRT() = 0;

		virtual uint32_t GetID() const = 0;
		virtual uint32_t GetColorID(int _index = 0) const = 0;

				uint32_t GetWidth() const { return m_specs.uWidth; }
				uint32_t GetHeight() const { return m_specs.uHeight; }

		static FrameBufferPtr Create(const Specification &_specs);

	protected:
		FrameBuffer(const Specification &_specs) : m_specs(_specs) {}

		Specification m_specs;
	};
}
