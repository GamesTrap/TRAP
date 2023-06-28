#ifndef TRAP_VERTEXBUFFERLAYOUT_H
#define TRAP_VERTEXBUFFERLAYOUT_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/RenderCommand.h"

namespace TRAP::Graphics
{
	/// <summary>
	/// Different data types used by shaders.
	/// </summary>
	enum class ShaderDataType
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		UInt, UInt2, UInt3, UInt4,
		Bool
	};

	/// <summary>
	/// Retrieve the byte size of a given shader data type.
	/// </summary>
	/// <param name="type">Shader data type.</param>
	/// <returns>Byte size of the shader data type.</returns>
	[[nodiscard]] constexpr uint32_t ShaderDataTypeSize(ShaderDataType type);

	/// <summary>
	/// Struct used to describe a single vertex attribute.
	/// </summary>
	struct VertexBufferElement
	{
		std::string Name;
		ShaderDataType Type{ShaderDataType::None};
		uint32_t Size{};
		uint32_t Offset{};
		bool Normalized{};

		/// <summary>
		/// Constructor.
		/// </summary>
		VertexBufferElement() noexcept = default;
		/// <summary>
		/// Constructor.
		/// Initialize the vertex buffer element with the given data.
		/// </summary>
		/// <param name="type">Shader data type.</param>
		/// <param name="name">Name of the vertex attribute.</param>
		/// <param name="normalized">Whether data is normalized.</param>
		constexpr VertexBufferElement(ShaderDataType type, std::string name, bool normalized = false) noexcept;

		/// <summary>
		/// Retrieve the component count of this vertex attribute.
		/// </summary>
		/// <returns>Component count of this vertex attribute.</returns>
		[[nodiscard]] uint32_t GetComponentCount() const;
	};

	/// <summary>
	/// Describes a vertex buffer layout.
	/// </summary>
	class VertexBufferLayout
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		VertexBufferLayout() noexcept = default;
		/// <summary>
		/// Constructor.
		/// Initialize the vertex buffer layout with the given elements.
		/// </summary>
		/// <param name="elements">Vertex buffer elements.</param>
		VertexBufferLayout(const std::initializer_list<VertexBufferElement>& elements) noexcept;

		/// <summary>
		/// Retrieve the total byte size of all vertex buffer elements used in this layout.
		/// </summary>
		[[nodiscard]] uint32_t GetStride() const noexcept;
		/// <summary>
		/// Retrieve the vertex buffer elements described by this layout.
		/// </summary>
		/// <returns>Vertex buffer elements.</returns>
		[[nodiscard]] constexpr const std::vector<VertexBufferElement>& GetElements() const noexcept;

		[[nodiscard]] std::vector<VertexBufferElement>::iterator begin() noexcept;
		[[nodiscard]] std::vector<VertexBufferElement>::iterator end() noexcept;
		[[nodiscard]] std::vector<VertexBufferElement>::const_iterator begin() const noexcept;
		[[nodiscard]] std::vector<VertexBufferElement>::const_iterator end() const noexcept;

	private:
		/// <summary>
		/// Update the offset and stride values of all vertex buffer elements.
		/// </summary>
		void CalculateOffsetsAndStride() noexcept;

		std::vector<VertexBufferElement> m_elements;
		uint32_t m_stride = 0;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::VertexBufferElement::VertexBufferElement(const ShaderDataType type, std::string name,
                                                                   const bool normalized) noexcept
	: Name(std::move(name)), Type(type), Size(ShaderDataTypeSize(type)), Normalized(normalized)
{
}

//-------------------------------------------------------------------------------------------------------------------//


[[nodiscard]] constexpr uint32_t TRAP::Graphics::ShaderDataTypeSize(const ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:  return 4;
	case ShaderDataType::Float2: return 4 * 2;
	case ShaderDataType::Float3: return 4 * 3;
	case ShaderDataType::Float4: return 4 * 4;

	case ShaderDataType::Mat3:   return 4 * 3 * 3;
	case ShaderDataType::Mat4:   return 4 * 4 * 4;

	case ShaderDataType::Int:    return 4;
	case ShaderDataType::Int2:   return 4 * 2;
	case ShaderDataType::Int3:   return 4 * 3;
	case ShaderDataType::Int4:   return 4 * 4;

	case ShaderDataType::Bool:   return 1;

	default:
		TRAP_ASSERT(false, "ShaderDataTypeSize(): Unknown shader data type!");
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Graphics::VertexBufferElement>& TRAP::Graphics::VertexBufferLayout::GetElements() const noexcept
{
	return m_elements;
}

#endif /*TRAP_VERTEXBUFFERLAYOUT_H*/