#ifndef _TRAP_STORAGEBUFFER_H_
#define _TRAP_STORAGEBUFFER_H_

#include "Graphics/API/ResourceLoader.h"
#include "Graphics/API/Objects/Buffer.h"
#include "Application.h"

namespace TRAP
{
	class Window;
}

namespace TRAP::Graphics
{
	using UpdateFrequency = RendererAPI::DescriptorUpdateFrequency;

	class StorageBuffer
	{
	protected:
		explicit StorageBuffer(RendererAPI::DescriptorUpdateFrequency updateFrequency);
		StorageBuffer(const StorageBuffer&) = default;
		StorageBuffer& operator=(const StorageBuffer&) = default;
		StorageBuffer(StorageBuffer&&) = default;
		StorageBuffer& operator=(StorageBuffer&&) = default;

	public:
		~StorageBuffer();

		uint64_t GetSize() const;
		UpdateFrequency GetUpdateFrequency() const;
		const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& GetSSBOs() const;

		void SetData(const void* data, uint64_t size, uint64_t offset = 0);
		template<typename T>
		void GetData(const T* data, uint64_t size, const uint64_t offset = 0,
		             Window* window = nullptr)
		{
			TRAP_ASSERT(size + offset <= m_storageBuffers[0]->GetSize());

			if(!window)
				window = TRAP::Application::GetWindow().get();

			RendererAPI::BufferUpdateDesc desc{};
			const uint32_t imageIndex = GetUpdateFrequency() ==
			                            RendererAPI::DescriptorUpdateFrequency::Static ?
				                            0 : RendererAPI::GetCurrentImageIndex(window);
			desc.Buffer = m_storageBuffers[imageIndex];
			desc.DstOffset = offset;
			RendererAPI::GetResourceLoader()->BeginUpdateResource(desc);
			std::memcpy(data, desc.MappedData, size);
			RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_tokens[imageIndex]);
		}

		bool IsLoaded() const;
		void AwaitLoading() const;

		static uint64_t CalculateAlignedSize(const uint64_t byteSize);

		static Scope<StorageBuffer> Create(uint64_t size, UpdateFrequency updateFrequency);
		static Scope<StorageBuffer> Create(void* data, uint64_t size, UpdateFrequency updateFrequency);

	private:
		static Scope<StorageBuffer> Init(void* data, uint64_t size, UpdateFrequency updateFrequency);

		std::vector<TRAP::Ref<TRAP::Graphics::Buffer>> m_storageBuffers;

		std::vector<API::SyncToken> m_tokens;
	};
}

#endif /*_TRAP_STORAGEBUFFER_H_*/