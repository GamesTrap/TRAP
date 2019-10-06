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

	enum class BufferUsage
	{
		Static,  //Buffer can't be updated
		Dynamic, //Buffer Data can be updated(also partially) when needed
		Stream   //Whole Buffer Data must be updated every Frame
	};

	uint32_t ShaderDataTypeSize(ShaderDataType type);

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type{ShaderDataType::None};
		intptr_t Size{};
		uint32_t Offset{};
		bool Normalized{};

		BufferElement() = default;
		BufferElement(ShaderDataType type, std::string name, bool normalized = false);

		uint32_t GetComponentCount() const;
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements);

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

#endif /*_TRAP_BUFFERLAYOUT_H_*/