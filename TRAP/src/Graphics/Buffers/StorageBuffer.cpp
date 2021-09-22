#include "TRAPPCH.h"
#include "StorageBuffer.h"

#include "Application.h"
#include "VertexBufferLayout.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Buffer.h"
#include "Graphics/API/Objects/DescriptorSet.h"
#include "Graphics/Shaders/Shader.h"

TRAP::Scope<TRAP::Graphics::StorageBuffer> TRAP::Graphics::StorageBuffer::Create(const bool writable,
																				 const uint64_t size,
																				 const UpdateFrequency updateFrequency)
{
	TP_PROFILE_FUNCTION();

	return Init(writable, nullptr, size, updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::StorageBuffer> TRAP::Graphics::StorageBuffer::Create(const bool writable, void* data,
																				 const uint64_t size,
																				 const UpdateFrequency updateFrequency)
{
	TP_PROFILE_FUNCTION();

	return Init(writable, data, size, updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::StorageBuffer::StorageBuffer(const RendererAPI::DescriptorUpdateFrequency updateFrequency)
	: m_storageBuffers(), m_tokens()
{
	m_tokens.resize(updateFrequency == UpdateFrequency::Static ? 1 : RendererAPI::ImageCount);
	m_storageBuffers.resize(updateFrequency == UpdateFrequency::Static ? 1 : RendererAPI::ImageCount);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::StorageBuffer::~StorageBuffer()
{
	m_storageBuffers = {};
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::Graphics::StorageBuffer::GetSize() const
{
	return m_storageBuffers[0]->GetSize();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::UpdateFrequency TRAP::Graphics::StorageBuffer::GetUpdateFrequency() const
{
	return m_storageBuffers.size() == 1 ? UpdateFrequency::Static : UpdateFrequency::Dynamic;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& TRAP::Graphics::StorageBuffer::GetSSBOs() const
{
	return m_storageBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::StorageBuffer::SetData(const void* data, const uint64_t size, const uint64_t offset)
{
	TRAP_ASSERT(data);
	TRAP_ASSERT(size + offset <= m_storageBuffers[0]->GetSize());

	for(uint32_t i = 0; i < m_storageBuffers.size(); ++i)
	{
		RendererAPI::BufferUpdateDesc desc{};
		desc.Buffer = m_storageBuffers[i];
		desc.DstOffset = offset;
		RendererAPI::GetResourceLoader()->BeginUpdateResource(desc);
		std::memcpy(desc.MappedData, data, size);
		RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_tokens[i]);
	}
	AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::StorageBuffer::IsWritable() const
{
	return m_storageBuffers[0]->GetDescriptors() == RendererAPI::DescriptorType::RWBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::StorageBuffer::IsLoaded() const
{
	for(uint32_t i = 0; i < m_storageBuffers.size(); ++i)
	{
	   if(!RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_tokens[i]))
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::StorageBuffer::AwaitLoading() const
{
	for(uint32_t i = 0; i < m_storageBuffers.size(); ++i)
		RendererAPI::GetResourceLoader()->WaitForToken(&m_tokens[i]);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::StorageBuffer> TRAP::Graphics::StorageBuffer::Init(const bool writable, void* data,
																			   const uint64_t size,
																			   const UpdateFrequency updateFrequency)
{
	TRAP::Scope<StorageBuffer> buffer = TRAP::Scope<StorageBuffer>(new StorageBuffer(updateFrequency));

	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (updateFrequency == UpdateFrequency::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly :
	                                                                     RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Flags = writable ? RendererAPI::BufferCreationFlags::PersistentMap :
								 RendererAPI::BufferCreationFlags::None;
	desc.Desc.Descriptors = writable ? RendererAPI::DescriptorType::RWBuffer : RendererAPI::DescriptorType::Buffer;
	desc.Desc.Size = size;
	desc.Data = data;

	for(uint32_t i = 0; i < buffer->m_storageBuffers.size(); ++i)
	{
		RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_tokens[i]);
		buffer->m_storageBuffers[i] = desc.Buffer;
	}

	return buffer;
}