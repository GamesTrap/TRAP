#ifndef _TRAP_OPENGLUNIFORMBUFFER_H_
#define _TRAP_OPENGLUNIFORMBUFFER_H_

#include "Graphics/Buffers/UniformBuffer.h"

namespace TRAP::Graphics::API
{
	class OpenGLUniformBuffer final : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer();
		OpenGLUniformBuffer(const char* name, const void* data, uint32_t size, BufferUsage usage);
		OpenGLUniformBuffer(const char* name, uint32_t size, BufferUsage usage);
		OpenGLUniformBuffer(const OpenGLUniformBuffer&) = default;
		OpenGLUniformBuffer& operator=(const OpenGLUniformBuffer&) = default;
		OpenGLUniformBuffer(OpenGLUniformBuffer&&) = default;
		OpenGLUniformBuffer& operator=(OpenGLUniformBuffer&&) = default;
		~OpenGLUniformBuffer();

		void Bind() const override;
		void Unbind() const override;

		void UpdateData(const void* data, uint32_t size, BufferUsage usage) override;
		void UpdateSubData(const void* data, uint32_t size, uint32_t offset) override;

		std::string GetName() const;

	private:
		uint32_t m_handle;
		const char* m_name;
		uint32_t m_size;
	};
}

inline std::string TRAP::Graphics::API::OpenGLUniformBuffer::GetName() const
{
	return std::string(m_name);
}

#endif /*_TRAP_OPENGLUNIFORMBUFFER_H_*/