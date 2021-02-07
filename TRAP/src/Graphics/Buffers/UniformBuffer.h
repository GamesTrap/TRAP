#ifndef _TRAP_UNIFORMBUFFER_H_
#define _TRAP_UNIFORMBUFFER_H_

#include "Graphics/API/ResourceLoader.h"

namespace TRAP
{
	class Window;
}

namespace TRAP::Graphics
{
	enum class BufferUsage;

	class UniformBuffer
	{
	protected:
		UniformBuffer();
		UniformBuffer(const UniformBuffer&) = default;
		UniformBuffer& operator=(const UniformBuffer&) = default;
		UniformBuffer(UniformBuffer&&) = default;
		UniformBuffer& operator=(UniformBuffer&&) = default;
		
	public:
		~UniformBuffer();

		uint64_t GetSize() const;
		BufferUsage GetBufferUsage() const;
		
		void Use(Window* window = nullptr);
		
		void SetData(const void* data, uint64_t size, uint64_t offset = 0);

		bool IsLoaded() const;
		void AwaitLoading() const;

		const std::string& GetName() const;
		
		static Scope<UniformBuffer> Create(const char* name, uint64_t size, BufferUsage usage);
		static Scope<UniformBuffer> Create(const char* name, void* data, uint64_t size, BufferUsage usage);

	private:
		TRAP::Ref<TRAP::Graphics::Buffer> m_uniformBuffer;
		
		API::SyncToken m_token;

		BufferUsage m_bufferUsage;
		
		std::string m_name;
	};
}

#endif /*_TRAP_UNIFORMBUFFER_H_*/