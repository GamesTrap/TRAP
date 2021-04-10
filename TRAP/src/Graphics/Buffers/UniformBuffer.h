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

		static Scope<UniformBuffer> Create(const std::string& name, uint64_t size, BufferUsage usage, Window* window = nullptr);
		static Scope<UniformBuffer> Create(const std::string& name, void* data, uint64_t size, BufferUsage usage, Window* window = nullptr);

	private:
		std::vector<TRAP::Ref<TRAP::Graphics::Buffer>> m_uniformBuffers;

		std::vector<API::SyncToken> m_tokens;

		BufferUsage m_bufferUsage;

		std::string m_name;

		static std::unordered_map<Window*, TRAP::Graphics::DescriptorSet*> s_descriptorSetMap; //TODO This may likely need to be moved to somewhere else :C
	};
}

#endif /*_TRAP_UNIFORMBUFFER_H_*/