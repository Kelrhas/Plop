#pragma once

namespace Plop
{
	class BufferLayout
	{
	public:
		enum class ElementType : uint8_t
		{
			NONE,
			FLOAT,
			FLOAT3,
			FLOAT4,
			MAT4,
		};

		struct Element
		{
			String		sName;
			ElementType eType;
			uint32_t	uSize;
			size_t		uOffset;
			bool		bNormalized;

			Element(String _sName, ElementType _eType, bool _bNormalized = false);
		};


		BufferLayout(const std::initializer_list<Element>& _elements);

		const std::vector<Element>& GetElements() const { return m_vecElements; }
		uint32_t GetStride() const { return m_uStride; }

		std::vector<Element>::iterator begin() { return m_vecElements.begin(); }
		std::vector<Element>::iterator end() { return m_vecElements.end(); }
		std::vector<Element>::const_iterator begin() const { return m_vecElements.begin(); }
		std::vector<Element>::const_iterator end() const { return m_vecElements.end(); }

		static uint32_t GetTypeSize(ElementType _eType);
		static uint32_t GetElementCount(ElementType _eType);

	private:
		std::vector<Element> m_vecElements;
		uint32_t m_uStride;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(size_t _uSize) {}
		VertexBuffer(uint32_t _uCount, float* _pVerts) {}
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetLayout(const BufferLayout& _layout) = 0;

		static VertexBuffer* Create(size_t _uSize);
		static VertexBuffer* Create(uint32_t _uCount, float* _pVerts);
	};

	class IndexBuffer
	{
	public:
		IndexBuffer(uint32_t _uCount, uint32_t* _pIndices) : m_uCount(_uCount) {}
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

				uint32_t GetCount() const { return m_uCount; }

		static IndexBuffer* Create(uint32_t _uCount, uint32_t* _pIndices);

	protected:
		uint32_t m_uCount = 0;
	};
}

