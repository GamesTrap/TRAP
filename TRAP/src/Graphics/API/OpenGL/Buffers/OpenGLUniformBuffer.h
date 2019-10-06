#ifndef _TRAP_OPENGLUNIFORMBUFFER_H_
#define _TRAP_OPENGLUNIFORMBUFFER_H_

#include "Graphics/Buffers/UniformBuffer.h"

namespace TRAP::Graphics
{
	enum class BufferUsage;
}

namespace TRAP::Graphics::API
{
	class OpenGLUniformBuffer final : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(const char* name, const void* data, uint32_t size, BufferUsage usage);
		OpenGLUniformBuffer(const char* name, uint32_t size, BufferUsage usage);
		OpenGLUniformBuffer(const OpenGLUniformBuffer&) = delete;
		OpenGLUniformBuffer& operator=(const OpenGLUniformBuffer&) = delete;
		OpenGLUniformBuffer(OpenGLUniformBuffer&&) = default;
		OpenGLUniformBuffer& operator=(OpenGLUniformBuffer&&) = default;
		~OpenGLUniformBuffer();

		void Bind(uint32_t bindingPoint) const override;
		void Unbind(uint32_t bindingPoint) const override;

		void UpdateData(const void* data) override;
		void UpdateSubData(const void* data, uint32_t size, uint32_t offset) override;

		std::string GetName() const;

	private:
		uint32_t m_handle;
		const char* m_name;
		uint32_t m_size;
		BufferUsage m_usage;
		uint8_t* m_dataPtr;
		uint32_t m_nextHeadOffset;
		uint32_t m_bufferCount;
		uint32_t m_alignedSize;

		static uint32_t s_uniformBufferOffsetAlignment;
		static uint32_t s_maxUniformBufferBindingPoints;
		static std::unordered_map<uint32_t, const OpenGLUniformBuffer*> s_boundUniformBuffers;
	};
}

#endif /*_TRAP_OPENGLUNIFORMBUFFER_H_*/