#include "TRAPPCH.h"
#include "UniformBuffer.h"

#include "Application.h"
#include "VertexBufferLayout.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Buffer.h"
#include "Graphics/API/Objects/DescriptorSet.h"
#include "Graphics/Shaders/Shader.h"

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const std::string& name,
                                                                                 const uint64_t size,
																				 const UpdateFrequency updateFrequency)
{
	TP_PROFILE_FUNCTION();

	return Init(name, nullptr, size, updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const std::string& name,
                                                                                 void* data,
																				 const uint64_t size,
																				 const UpdateFrequency updateFrequency)
{
	TP_PROFILE_FUNCTION();

	return Init(name, data, size, updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::UniformBuffer::UniformBuffer()
	: m_uniformBuffers(), m_tokens()
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

TRAP::Graphics::UpdateFrequency TRAP::Graphics::UniformBuffer::GetUpdateFrequency() const
{
	//TODO What about PerBatch & PerDraw
	return (m_uniformBuffers[0]->GetMemoryUsage() == RendererAPI::ResourceMemoryUsage::GPUOnly) ? UpdateFrequency::None :
	                                                                                              UpdateFrequency::PerFrame;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& TRAP::Graphics::UniformBuffer::GetUBOs() const
{
	return m_uniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::UniformBuffer::SetData(const void* data, const uint64_t size, const uint64_t offset)
{
	TRAP_ASSERT(data);
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

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Init(const std::string& name, void* data,
                                                                               const uint64_t size,
																			   const UpdateFrequency updateFrequency)
{
	//TODO What about PerBatch & PerDraw
	TRAP::Scope<UniformBuffer> buffer = TRAP::Scope<UniformBuffer>(new UniformBuffer());
	buffer->m_name = name;
	buffer->m_tokens.resize(updateFrequency == UpdateFrequency::None ? 1 : RendererAPI::ImageCount);
	buffer->m_uniformBuffers.resize(updateFrequency == UpdateFrequency::None ? 1 : RendererAPI::ImageCount);

	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (updateFrequency == UpdateFrequency::None) ? RendererAPI::ResourceMemoryUsage::GPUOnly :
	                                                                     RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Flags = (updateFrequency != UpdateFrequency::None) ? RendererAPI::BufferCreationFlags::PersistentMap :
																   RendererAPI::BufferCreationFlags::None;
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