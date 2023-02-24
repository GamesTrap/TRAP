#include "TRAPPCH.h"
#include "StorageBuffer.h"

#include "Application.h"
#include "VertexBufferLayout.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Buffer.h"
#include "Graphics/API/Objects/DescriptorSet.h"
#include "Graphics/Shaders/Shader.h"

[[nodiscard]] TRAP::Scope<TRAP::Graphics::StorageBuffer> TRAP::Graphics::StorageBuffer::Create(const uint64_t size,
																				               const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	return Init(nullptr, size, updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Graphics::StorageBuffer> TRAP::Graphics::StorageBuffer::Create(const void* const data, const uint64_t size,
																				               const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	return Init(data, size, updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::StorageBuffer::StorageBuffer(const RendererAPI::DescriptorUpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	m_tokens.resize(updateFrequency == UpdateFrequency::Static ? 1 : RendererAPI::ImageCount);
	m_storageBuffers.resize(updateFrequency == UpdateFrequency::Static ? 1 : RendererAPI::ImageCount);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::StorageBuffer::~StorageBuffer()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	m_storageBuffers = {};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint64_t TRAP::Graphics::StorageBuffer::GetSize() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_storageBuffers[0]->GetSize();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::UpdateFrequency TRAP::Graphics::StorageBuffer::GetUpdateFrequency() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_storageBuffers.size() == 1 ? UpdateFrequency::Static : UpdateFrequency::Dynamic;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& TRAP::Graphics::StorageBuffer::GetSSBOs() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_storageBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::StorageBuffer::SetData(const void* const data, const uint64_t size, const uint64_t offset)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(data, "StorageBuffer::SetData(): Data is nullptr!");
	TRAP_ASSERT(size + offset <= m_storageBuffers[0]->GetSize(), "StorageBuffer::SetData(): Out of bounds!");

	for(std::size_t i = 0; i < m_storageBuffers.size(); ++i)
	{
		RendererAPI::BufferUpdateDesc desc{};
		desc.Buffer = m_storageBuffers[i];
		desc.DstOffset = offset;
		RendererAPI::GetResourceLoader()->BeginUpdateResource(desc);
		std::copy_n(static_cast<const uint8_t*>(data), size, static_cast<uint8_t*>(desc.MappedData));
		RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_tokens[i]);
	}
	AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::StorageBuffer::IsLoaded() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	for(std::size_t i = 0; i < m_storageBuffers.size(); ++i)
	{
	   if(!RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_tokens[i]))
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::StorageBuffer::AwaitLoading() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	for(std::size_t i = 0; i < m_storageBuffers.size(); ++i)
		RendererAPI::GetResourceLoader()->WaitForToken(&m_tokens[i]);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint64_t TRAP::Graphics::StorageBuffer::CalculateAlignedSize(const uint64_t byteSize) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const uint64_t minSSBOAlignment = RendererAPI::GPUSettings.StorageBufferAlignment;
	uint64_t alignedSize = byteSize;

	if(minSSBOAlignment > 0)
		alignedSize = (alignedSize + minSSBOAlignment - 1) & ~(minSSBOAlignment - 1);

	return alignedSize;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Graphics::StorageBuffer> TRAP::Graphics::StorageBuffer::Init(const void* const data, const uint64_t size,
																			                 const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP::Scope<StorageBuffer> buffer = TRAP::Scope<StorageBuffer>(new StorageBuffer(updateFrequency));

	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (updateFrequency == UpdateFrequency::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly :
	                                                                     RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Flags = RendererAPI::BufferCreationFlags::PersistentMap;
	desc.Desc.Descriptors = RendererAPI::DescriptorType::RWBuffer;
	desc.Desc.Size = size;
	desc.Data = data;

	for(std::size_t i = 0; i < buffer->m_storageBuffers.size(); ++i)
	{
		RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_tokens[i]);
		buffer->m_storageBuffers[i] = desc.Buffer;
	}

	return buffer;
}