#ifndef _TRAP_VERTEXBUFFERLAYOUT_H_
#define _TRAP_VERTEXBUFFERLAYOUT_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	enum class ShaderDataType
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		Bool
	};

	using UpdateFrequency = RendererAPI::DescriptorUpdateFrequency;

	uint32_t ShaderDataTypeSize(ShaderDataType type);

	struct VertexBufferElement
	{
		std::string Name;
		ShaderDataType Type{ShaderDataType::None};
		uint32_t Size{};
		uint32_t Offset{};
		bool Normalized{};

		VertexBufferElement() = default;
		VertexBufferElement(ShaderDataType type, std::string name, bool normalized = false);

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		VertexBufferElement(const VertexBufferElement&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		VertexBufferElement& operator=(const VertexBufferElement&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		VertexBufferElement(VertexBufferElement&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		VertexBufferElement& operator=(VertexBufferElement&&) = default;

		uint32_t GetComponentCount() const;
	};

	class VertexBufferLayout
	{
	public:
		VertexBufferLayout() = default;
		VertexBufferLayout(const std::initializer_list<VertexBufferElement>& elements);

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		VertexBufferLayout(const VertexBufferLayout&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		VertexBufferLayout& operator=(const VertexBufferLayout&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		VertexBufferLayout(VertexBufferLayout&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		VertexBufferLayout& operator=(VertexBufferLayout&&) = default;

		uint32_t GetStride() const;
		const std::vector<VertexBufferElement>& GetElements() const;

		std::vector<VertexBufferElement>::iterator begin();
		std::vector<VertexBufferElement>::iterator end();
		std::vector<VertexBufferElement>::const_iterator begin() const;
		std::vector<VertexBufferElement>::const_iterator end() const;

	private:
		void CalculateOffsetsAndStride();

		std::vector<VertexBufferElement> m_elements;
		uint32_t m_stride = 0;
	};
}

#endif /*_TRAP_VERTEXBUFFERLAYOUT_H_*/