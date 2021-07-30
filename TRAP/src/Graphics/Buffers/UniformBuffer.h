#ifndef _TRAP_UNIFORMBUFFER_H_
#define _TRAP_UNIFORMBUFFER_H_

#include "Graphics/API/ResourceLoader.h"

namespace TRAP
{
	class Window;
}

namespace TRAP::Graphics
{
	using UpdateFrequency = RendererAPI::DescriptorUpdateFrequency;

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
		UpdateFrequency GetUpdateFrequency() const;
		const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& GetUBOs() const;

		void SetData(const void* data, uint64_t size, uint64_t offset = 0);

		bool IsLoaded() const;
		void AwaitLoading() const;

		const std::string& GetName() const;

		static Scope<UniformBuffer> Create(const std::string& name, uint64_t size, UpdateFrequency updateFrequency);
		static Scope<UniformBuffer> Create(const std::string& name, void* data, uint64_t size, UpdateFrequency updateFrequency);

	private:
		static Scope<UniformBuffer> Init(const std::string& name, void* data, uint64_t size, UpdateFrequency updateFrequency);

		std::vector<TRAP::Ref<TRAP::Graphics::Buffer>> m_uniformBuffers;

		std::vector<API::SyncToken> m_tokens;

		std::string m_name;

		bool m_first = true;
	};
}

#endif /*_TRAP_UNIFORMBUFFER_H_*/