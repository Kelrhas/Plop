#include "Config.h"

#include "Buffers.h"

#include "Debug/Debug.h"
#include "Platform/OpenGL/OpenGL_Buffers.h"
#include "Renderer/Renderer.h"

namespace Plop
{
	//////////////////////////////////////////////////////////////////////////
	// BufferLayout
	BufferLayout::Element::Element(String _sName, ElementType _eType, bool _bNormalized /*= false*/)
		: sName(_sName), eType(_eType), bNormalized(_bNormalized)
	{
		uSize = GetTypeSize(_eType);
	}

	BufferLayout::BufferLayout(const std::initializer_list<BufferLayout::Element>& _elements)
	{
		m_vecElements = _elements;
		uint32_t uOffset = 0;
		for (BufferLayout::Element& elmt : m_vecElements)
		{
			elmt.uOffset = uOffset;
			uOffset += elmt.uSize;
		}

		m_uStride = uOffset;
	}

	uint32_t BufferLayout::GetTypeSize(ElementType _eType)
	{
		switch (_eType)
		{
			case ElementType::INT:		return 4;
			case ElementType::FLOAT:		return 4;
			case ElementType::FLOAT2:		return 4 * 2;
			case ElementType::FLOAT3:		return 4 * 3;
			case ElementType::FLOAT4:		return 4 * 4;
			case ElementType::MAT4:			return 4 * 4 * 4;
		}

		ASSERTM(false, "BufferLayout type not supported");
		return 0;
	}

	uint32_t BufferLayout::GetElementCount(ElementType _eType)
	{
		switch (_eType)
		{
			case ElementType::INT:			return 1;
			case ElementType::FLOAT:		return 1;
			case ElementType::FLOAT2:		return 2;
			case ElementType::FLOAT3:		return 3;
			case ElementType::FLOAT4:		return 4;
			case ElementType::MAT4:			return 4;
		}

		ASSERTM(false, "BufferLayout type not supported");
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// VertexBuffer
	void VertexBuffer::SetLayout(const BufferLayout& _layout)
	{
		m_layout = _layout;
	}

	VertexBufferPtr VertexBuffer::Create(size_t _uSize)
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::OPENGL: return std::make_shared<OpenGL_VertexBuffer>(_uSize);
		}

		ASSERTM(false, "Render API not supported");
		return nullptr;
	}

	VertexBufferPtr VertexBuffer::Create(size_t _uSize, void* _pData)
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::OPENGL: return std::make_shared<OpenGL_VertexBuffer>( _uSize, _pData );
		}

		ASSERTM(false, "Render API not supported");
		return nullptr;
	}


	//////////////////////////////////////////////////////////////////////////
	// IndexBuffer
	IndexBufferPtr IndexBuffer::Create(uint32_t _uCount)
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::OPENGL: return std::make_shared<OpenGL_IndexBuffer>(_uCount);
		}

		ASSERTM(false, "Render API not supported");
		return nullptr;
	}

	IndexBufferPtr IndexBuffer::Create(uint32_t _uCount, uint32_t* _pIndices)
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::OPENGL: return std::make_shared<OpenGL_IndexBuffer>(_uCount, _pIndices);
		}

		ASSERTM(false, "Render API not supported");
		return nullptr;
	}
}
