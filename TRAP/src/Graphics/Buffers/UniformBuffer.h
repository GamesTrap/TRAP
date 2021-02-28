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
		
		void Use();
		
		void SetData(const void* data, uint64_t size, uint64_t offset = 0);

		bool IsLoaded() const;
		void AwaitLoading() const;

		const std::string& GetName() const;

		//TODO Remove?!
		const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& GetUniformBuffers() const;
		
		static Scope<UniformBuffer> Create(const char* name, uint32_t binding, uint64_t size, BufferUsage usage, Window* window = nullptr);
		static Scope<UniformBuffer> Create(const char* name, uint32_t binding, void* data, uint64_t size, BufferUsage usage, Window* window = nullptr);

	private:
		Window* m_window;
		uint32_t m_binding;
		
		std::vector<TRAP::Ref<TRAP::Graphics::Buffer>> m_uniformBuffers;

		TRAP::Graphics::DescriptorSet* m_descriptorSet;
		
		std::vector<API::SyncToken> m_tokens;

		BufferUsage m_bufferUsage;
		
		std::string m_name;

		static std::unordered_map<Window*, DescriptorSet*> s_boundDescriptorSets;
	};
}

#endif /*_TRAP_UNIFORMBUFFER_H_*/