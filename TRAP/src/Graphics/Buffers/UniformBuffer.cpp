#include "TRAPPCH.h"
#include "UniformBuffer.h"

#include "BufferLayout.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Buffer.h"

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const char* name, const uint64_t size, const BufferUsage usage)
{
	TP_PROFILE_FUNCTION();

	TRAP::Scope<UniformBuffer> buffer = TRAP::Scope<UniformBuffer>(new UniformBuffer());
	buffer->m_name = name;
	buffer->m_bufferUsage = usage;
	
	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (usage == BufferUsage::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly : RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Flags = (usage == BufferUsage::Stream) ? RendererAPI::BufferCreationFlags::PersistentMap : RendererAPI::BufferCreationFlags::None;
	desc.Desc.Descriptors = RendererAPI::DescriptorType::UniformBuffer;
	desc.Desc.Size = size;

	for (uint32_t i = 0; i < 3u; ++i)
	{
		RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_tokens[i]);
		buffer->m_uniformBuffers[i] = desc.Buffer;
	}
	
	return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const char* name, void* data, const uint64_t size, const BufferUsage usage)
{
	TP_PROFILE_FUNCTION();

	TRAP::Scope<UniformBuffer> buffer = TRAP::Scope<UniformBuffer>(new UniformBuffer());
	buffer->m_name = name;
	buffer->m_bufferUsage = usage;

	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (usage == BufferUsage::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly : RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Flags = (usage == BufferUsage::Stream) ? RendererAPI::BufferCreationFlags::PersistentMap : RendererAPI::BufferCreationFlags::None;
	desc.Desc.Descriptors = RendererAPI::DescriptorType::UniformBuffer;
	desc.Desc.Size = size;
	desc.Data = data;

	for(uint32_t i = 0; i < 3u; ++i)
	{
		RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_tokens[i]);
		buffer->m_uniformBuffers[i] = desc.Buffer;
	}

	return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::UniformBuffer::UniformBuffer()
	: m_uniformBuffers(3), m_tokens(3), m_bufferUsage(BufferUsage::Static)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::UniformBuffer::~UniformBuffer()
{
	m_uniformBuffers = {};
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::Graphics::UniformBuffer::GetSize() const
{
	return m_uniformBuffers[0]->GetSize();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::BufferUsage TRAP::Graphics::UniformBuffer::GetBufferUsage() const
{
	return m_bufferUsage;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::UniformBuffer::Use(Window* window)
{
	//TODO
	//PerWindowData Add DescriptorInfo
	//Implicitly Set DescriptorSet
	//CommandBuffer->BindDescriptorSet();

	//BufferUsage Static = UpdateFreqNone, Dynamic = UpdateFreqPerBatch, Stream = UpdateFreqPerFrame
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::UniformBuffer::SetData(const void* data, const uint64_t size, const uint64_t offset)
{
	TRAP_ASSERT(size + offset <= m_uniformBuffers[0]->GetSize());

	for(uint32_t i = 0; i < 3u; ++i)
	{
		RendererAPI::BufferUpdateDesc desc{ m_uniformBuffers[i] };
		desc.DstOffset = offset;
		RendererAPI::GetResourceLoader()->BeginUpdateResource(desc);
		std::memcpy(desc.MappedData, data, size);
		RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_tokens[i]);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::UniformBuffer::IsLoaded() const
{
	return RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_tokens[0]) &&
		   RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_tokens[1]) &&
		   RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_tokens[2]);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::UniformBuffer::AwaitLoading() const
{
	for(uint32_t i = 0; i < 3u; ++i)
		RendererAPI::GetResourceLoader()->WaitForToken(&m_tokens[i]);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::UniformBuffer::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& TRAP::Graphics::UniformBuffer::GetUniformBuffers() const
{
	return m_uniformBuffers;
}