#ifndef TRAP_VERTEXBUFFERLAYOUT_H
#define TRAP_VERTEXBUFFERLAYOUT_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	/// @brief Different data types used by shaders.
	enum class ShaderDataType
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		UInt, UInt2, UInt3, UInt4,
		Bool
	};

	/// @brief Retrieve the byte size of a given shader data type.
	/// @param type Shader data type.
	/// @return Byte size of the shader data type.
	[[nodiscard]] u32 ShaderDataTypeSize(ShaderDataType type);

	/// @brief Struct used to describe a single vertex attribute.
	struct VertexBufferElement
	{
		std::string Name;
		ShaderDataType Type{ShaderDataType::None};
		u32 Size{};
		u32 Offset{};
		bool Normalized{};

		/// @brief Constructor.
		constexpr VertexBufferElement() noexcept = default;
		/// @brief Constructor.
		/// Initialize the vertex buffer element with the given data.
		/// @param type Shader data type.
		/// @param name Name of the vertex attribute.
		/// @param normalized Whether data is normalized.
		VertexBufferElement(ShaderDataType type, std::string name, bool normalized = false) noexcept;

		/// @brief Copy constructor.
		constexpr VertexBufferElement(const VertexBufferElement&) = default;
		/// @brief Copy assignment operator.
		constexpr VertexBufferElement& operator=(const VertexBufferElement&) = default;
		/// @brief Move constructor
		constexpr VertexBufferElement(VertexBufferElement&&) noexcept= default;
		/// @brief Move assignment operator.
		constexpr VertexBufferElement& operator=(VertexBufferElement&&) noexcept = default;

		/// @brief Destructor.
		constexpr ~VertexBufferElement() = default;

		/// @brief Retrieve the component count of this vertex attribute.
		/// @return Component count of this vertex attribute.
		[[nodiscard]] u32 GetComponentCount() const;
	};

	/// @brief Describes a vertex buffer layout.
	class VertexBufferLayout
	{
	public:
		/// @brief Constructor.
		constexpr VertexBufferLayout() noexcept = default;
		/// @brief Constructor.
		/// Initialize the vertex buffer layout with the given elements.
		/// @param elements Vertex buffer elements.
		constexpr VertexBufferLayout(const std::initializer_list<VertexBufferElement>& elements) noexcept;

		/// @brief Copy constructor.
		constexpr VertexBufferLayout(const VertexBufferLayout&) = default;
		/// @brief Copy assignment operator.
		constexpr VertexBufferLayout& operator=(const VertexBufferLayout&) = default;
		/// @brief Move constructor.
		constexpr VertexBufferLayout(VertexBufferLayout&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr VertexBufferLayout& operator=(VertexBufferLayout&&) noexcept = default;

		/// @brief Destructor.
		constexpr ~VertexBufferLayout() = default;

		/// @brief Retrieve the total byte size of all vertex buffer elements used in this layout.
		[[nodiscard]] constexpr u32 GetStride() const noexcept;
		/// @brief Retrieve the vertex buffer elements described by this layout.
		/// @return Vertex buffer elements.
		[[nodiscard]] constexpr const std::vector<VertexBufferElement>& GetElements() const noexcept;

		/// @brief Returns an iterator to the first element of the vertexbuffer layout.
		/// @return Iterator to the first element.
		[[nodiscard]] constexpr std::vector<VertexBufferElement>::iterator begin() noexcept;
		/// @brief Returns an iterator to the first element of the vertexbuffer layout.
		/// @return Iterator to the first element.
		[[nodiscard]] constexpr std::vector<VertexBufferElement>::const_iterator begin() const noexcept;
		/// @brief Returns an iterator to the first element of the vertexbuffer layout.
		/// @return Iterator to the first element.
		[[nodiscard]] constexpr std::vector<VertexBufferElement>::const_iterator cbegin() const noexcept;
		/// @brief Returns an iterator to the element following the last element of the vertex buffer layout.
		/// @return Iterator to the element following the last element.
		/// @note This element acts as a placeholder; attempting to access it results in undefined behaviour.
		[[nodiscard]] constexpr std::vector<VertexBufferElement>::iterator end() noexcept;
		/// @brief Returns an iterator to the element following the last element of the vertex buffer layout.
		/// @return Iterator to the element following the last element.
		/// @note This element acts as a placeholder; attempting to access it results in undefined behaviour.
		[[nodiscard]] constexpr std::vector<VertexBufferElement>::const_iterator end() const noexcept;
		/// @brief Returns an iterator to the element following the last element of the vertex buffer layout.
		/// @return Iterator to the element following the last element.
		/// @note This element acts as a placeholder; attempting to access it results in undefined behaviour.
		[[nodiscard]] constexpr std::vector<VertexBufferElement>::const_iterator cend() const noexcept;

	private:
		/// @brief Update the offset and stride values of all vertex buffer elements.
		constexpr void CalculateOffsetsAndStride() noexcept;

		std::vector<VertexBufferElement> m_elements;
		u32 m_stride = 0;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::VertexBufferLayout::VertexBufferLayout(const std::initializer_list<VertexBufferElement>& elements) noexcept
	: m_elements(elements)
{
	CalculateOffsetsAndStride();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::VertexBufferLayout::GetStride() const noexcept
{
	return m_stride;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Graphics::VertexBufferElement>& TRAP::Graphics::VertexBufferLayout::GetElements() const noexcept
{
	return m_elements;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<TRAP::Graphics::VertexBufferElement>::iterator TRAP::Graphics::VertexBufferLayout::begin() noexcept
{
	return m_elements.begin();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<TRAP::Graphics::VertexBufferElement>::const_iterator TRAP::Graphics::VertexBufferLayout::begin() const noexcept
{
	return m_elements.begin();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<TRAP::Graphics::VertexBufferElement>::const_iterator TRAP::Graphics::VertexBufferLayout::cbegin() const noexcept
{
	return m_elements.cbegin();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<TRAP::Graphics::VertexBufferElement>::iterator TRAP::Graphics::VertexBufferLayout::end() noexcept
{
	return m_elements.end();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<TRAP::Graphics::VertexBufferElement>::const_iterator TRAP::Graphics::VertexBufferLayout::end() const noexcept
{
	return m_elements.end();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<TRAP::Graphics::VertexBufferElement>::const_iterator TRAP::Graphics::VertexBufferLayout::cend() const noexcept
{
	return m_elements.cend();
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::VertexBufferLayout::CalculateOffsetsAndStride() noexcept
{
	u32 offset = 0;
	m_stride = 0;

	for (auto& element : m_elements)
	{
		element.Offset = offset;
		offset += element.Size;
		m_stride += element.Size;
	}
}

#endif /*TRAP_VERTEXBUFFERLAYOUT_H*/