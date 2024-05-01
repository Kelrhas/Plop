#pragma once

#include "Types.h"

#include <Renderer/FrameBuffer.h>

namespace Plop
{
	class OpenGL_FrameBuffer : public FrameBuffer
	{
	public:
		OpenGL_FrameBuffer(const Specification &_specs);
		virtual ~OpenGL_FrameBuffer();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize( uint32_t _uWidth, uint32_t _uHeight ) override;
		virtual uint32_t ReadPixelUInt32(uint32_t _attachmentIndex, int _x, int _y) override;

		virtual void ClearRT() override;

		virtual uint32_t GetID() const override { return m_uID; }
		virtual uint32_t GetColorID(int _index = 0) const override { return m_vecColorAttachments[_index]; }

				void Recreate();

	private:
		uint32_t m_uID;

		std::vector<Specification::RenderTargetSpecs> m_vecColorSpecs;
		std::vector<uint32_t> m_vecColorAttachments;
		Specification::RenderTargetSpecs m_depthSpec = TextureFormat::NONE;
		uint32_t m_uDepthAttachment = 0;
	};
}
