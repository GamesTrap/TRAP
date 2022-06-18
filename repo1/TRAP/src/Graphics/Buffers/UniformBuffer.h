#ifndef _TRAP_UNIFORMBUFFER_H_
#define _TRAP_UNIFORMBUFFER_H_

namespace TRAP::Graphics
{
	enum class BufferUsage;

	class UniformBuffer
	{
	public:
		UniformBuffer() = default;
		virtual ~UniformBuffer() = default;
		UniformBuffer(const UniformBuffer&) = default;
		UniformBuffer& operator=(const UniformBuffer&) = default;
		UniformBuffer(UniformBuffer&&) = default;
		UniformBuffer& operator=(UniformBuffer&&) = default;

		static Scope<UniformBuffer> Create(const char* name, uint32_t size, BufferUsage usage);
		static Scope<UniformBuffer> Create(const char* name, void* data, uint32_t size, BufferUsage usage);

		virtual void Bind(uint32_t bindingPoint = 1) const = 0;
		virtual void Unbind(uint32_t bindingPoint = 1) const = 0;

		virtual void UpdateData(const void* data) = 0;
		virtual void UpdateSubData(const void* data, uint32_t size, uint32_t offset) = 0;

		virtual const std::string& GetName() const = 0;
		virtual uint32_t GetSize() const = 0;
		virtual BufferUsage GetUsage() const = 0;
	};
}

#endif /*_TRAP_UNIFORMBUFFER_H_*/