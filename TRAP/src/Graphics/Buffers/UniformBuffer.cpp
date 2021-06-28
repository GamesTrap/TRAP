#include "TRAPPCH.h"
#include "UniformBuffer.h"

#include "Application.h"
#include "BufferLayout.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Buffer.h"
#include "Graphics/API/Objects/DescriptorSet.h"
#include "Graphics/Shaders/Shader.h"

//TODO Replace BufferUsage with (Descriptor)UpdateFrequency ?! This would resolve issue in Use function
TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const std::string& name, const uint64_t size, const BufferUsage usage)
{
	TP_PROFILE_FUNCTION();

	TRAP::Scope<UniformBuffer> buffer = TRAP::Scope<UniformBuffer>(new UniformBuffer());
	buffer->m_name = name;
	buffer->m_bufferUsage = usage;
	buffer->m_tokens.resize(usage == BufferUsage::Static ? 1 : RendererAPI::ImageCount);
	buffer->m_uniformBuffers.resize(usage == BufferUsage::Static ? 1 : RendererAPI::ImageCount);

	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (usage == BufferUsage::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly : RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Flags = (usage == BufferUsage::Static) ? RendererAPI::BufferCreationFlags::None : RendererAPI::BufferCreationFlags::PersistentMap;
	desc.Desc.Descriptors = RendererAPI::DescriptorType::UniformBuffer;
	desc.Desc.Size = size;

	for (uint32_t i = 0; i < buffer->m_uniformBuffers.size(); ++i)
	{
		RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_tokens[i]);
		buffer->m_uniformBuffers[i] = desc.Buffer;
	}

	return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const std::string& name, void* data, const uint64_t size, const BufferUsage usage)
{
	TP_PROFILE_FUNCTION();

	TRAP::Scope<UniformBuffer> buffer = TRAP::Scope<UniformBuffer>(new UniformBuffer());
	buffer->m_name = name;
	buffer->m_bufferUsage = usage;
	buffer->m_tokens.resize(usage == BufferUsage::Static ? 1 : RendererAPI::ImageCount);
	buffer->m_uniformBuffers.resize(usage == BufferUsage::Static ? 1 : RendererAPI::ImageCount);

	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (usage == BufferUsage::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly : RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Flags = (usage == BufferUsage::Static) ? RendererAPI::BufferCreationFlags::None : RendererAPI::BufferCreationFlags::PersistentMap;
	desc.Desc.Descriptors = RendererAPI::DescriptorType::UniformBuffer;
	desc.Desc.Size = size;
	desc.Data = data;

	for(uint32_t i = 0; i < buffer->m_uniformBuffers.size(); ++i)
	{
		RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_tokens[i]);
		buffer->m_uniformBuffers[i] = desc.Buffer;
	}

	return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::UniformBuffer::UniformBuffer()
	: m_uniformBuffers(), m_tokens(), m_bufferUsage(BufferUsage::Static)
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

const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& TRAP::Graphics::UniformBuffer::GetUBOs() const
{
	return m_uniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::UniformBuffer::SetData(const void* data, const uint64_t size, const uint64_t offset)
{
	TRAP_ASSERT(size + offset <= m_uniformBuffers[0]->GetSize());

	for(uint32_t i = 0; i < m_uniformBuffers.size(); ++i)
	{
		RendererAPI::BufferUpdateDesc desc{};
		desc.Buffer = m_uniformBuffers[i];
		desc.DstOffset = offset;
		RendererAPI::GetResourceLoader()->BeginUpdateResource(desc);
		std::memcpy(desc.MappedData, data, size);
		RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_tokens[i]);
	}
	AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::UniformBuffer::IsLoaded() const
{
	for(uint32_t i = 0; i < m_uniformBuffers.size(); ++i)
	{
	   if(!RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_tokens[i]))
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::UniformBuffer::AwaitLoading() const
{
	for(uint32_t i = 0; i < m_uniformBuffers.size(); ++i)
		RendererAPI::GetResourceLoader()->WaitForToken(&m_tokens[i]);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::UniformBuffer::GetName() const
{
	return m_name;
}