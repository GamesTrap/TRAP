#ifndef _TRAP_BUFFERLAYOUT_H_
#define _TRAP_BUFFERLAYOUT_H_

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

	static uint32_t ShaderDataTypeSize(const ShaderDataType type)
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
			TP_CORE_ASSERT(false, "[BufferLayout] Unknown ShaderDataType!");
			return 0;
		}
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type{ShaderDataType::None};
		uint32_t Size{};
		uint32_t Offset{};
		bool Normalized{};

		BufferElement() = default;
		BufferElement(const ShaderDataType type, std::string name, const bool normalized = false)
			: Name(std::move(name)), Type(type), Size(ShaderDataTypeSize(type)), Normalized(normalized)
		{
		}

		uint32_t GetComponentCount() const;
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		uint32_t GetStride() const;
		const std::vector<BufferElement>& GetElements() const;

		std::vector<BufferElement>::iterator begin();
		std::vector<BufferElement>::iterator end();
		std::vector<BufferElement>::const_iterator begin() const;
		std::vector<BufferElement>::const_iterator end() const;

	private:
		void CalculateOffsetsAndStride();

		std::vector<BufferElement> m_elements;
		uint32_t m_stride = 0;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Graphics::BufferLayout::GetStride() const
{
	return m_stride;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::vector<TRAP::Graphics::BufferElement>& TRAP::Graphics::BufferLayout::GetElements() const
{
	return m_elements;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::vector<TRAP::Graphics::BufferElement>::iterator TRAP::Graphics::BufferLayout::begin()
{
	return m_elements.begin();
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::vector<TRAP::Graphics::BufferElement>::iterator TRAP::Graphics::BufferLayout::end()
{
	return m_elements.end();
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::vector<TRAP::Graphics::BufferElement>::const_iterator TRAP::Graphics::BufferLayout::begin() const
{
	return m_elements.begin();
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::vector<TRAP::Graphics::BufferElement>::const_iterator TRAP::Graphics::BufferLayout::end() const
{
	return m_elements.end();
}

#endif /*_TRAP_BUFFERLAYOUT_H_*/