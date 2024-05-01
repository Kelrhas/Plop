#include "Config.h"

#include "FrameBuffer.h"

#include "Debug/Debug.h"
#include "Platform/OpenGL/OpenGL_FrameBuffer.h"
#include "Renderer/Renderer.h"

namespace Plop
{
	FrameBufferPtr FrameBuffer::Create(const Specification &_specs)
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::OPENGL: return std::make_shared<OpenGL_FrameBuffer>(_specs);
		}

		ASSERTM( false, "Render API not supported" );
		return nullptr;
	}
}
