#include "TRAPPCH.h"
#include "UniformBuffer.h"

#include "Application.h"
#include "VertexBufferLayout.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Buffer.h"
#include "Graphics/API/Objects/DescriptorSet.h"
#include "Graphics/Shaders/Shader.h"

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const uint64_t size,
																				 const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	return Init(nullptr, size, updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const void* const data,
																				 const uint64_t size,
																				 const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	return Init(data, size, updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::UniformBuffer::UniformBuffer(const RendererAPI::DescriptorUpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	m_tokens.resize(updateFrequency == UpdateFrequency::Static ? 1 : RendererAPI::ImageCount);
	m_uniformBuffers.resize(updateFrequency == UpdateFrequency::Static ? 1 : RendererAPI::ImageCount);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::UniformBuffer::~UniformBuffer()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	m_uniformBuffers = {};
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::Graphics::UniformBuffer::GetSize() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_uniformBuffers[0]->GetSize();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::UpdateFrequency TRAP::Graphics::UniformBuffer::GetUpdateFrequency() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_uniformBuffers.size() == 1 ? UpdateFrequency::Static : UpdateFrequency::Dynamic;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& TRAP::Graphics::UniformBuffer::GetUBOs() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_uniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::UniformBuffer::SetData(const void* const data, const uint64_t size, const uint64_t offset)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(data, "UniformBuffer::SetData(): Data is nullptr!");
	TRAP_ASSERT(size + offset <= m_uniformBuffers[0]->GetSize(), "UniformBuffer::SetData(): Out of bounds!");

	for(std::size_t i = 0; i < m_uniformBuffers.size(); ++i)
	{
		RendererAPI::BufferUpdateDesc desc{};
		desc.Buffer = m_uniformBuffers[i];
		desc.DstOffset = offset;
		RendererAPI::GetResourceLoader()->BeginUpdateResource(desc);
		std::copy_n(static_cast<const uint8_t*>(data), size, static_cast<uint8_t*>(desc.MappedData));
		RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_tokens[i]);
	}
	AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::UniformBuffer::IsLoaded() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	for(std::size_t i = 0; i < m_uniformBuffers.size(); ++i)
	{
	   if(!RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_tokens[i]))
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::UniformBuffer::AwaitLoading() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	for(std::size_t i = 0; i < m_uniformBuffers.size(); ++i)
		RendererAPI::GetResourceLoader()->WaitForToken(&m_tokens[i]);
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::Graphics::UniformBuffer::CalculateAlignedSize(const uint64_t byteSize)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const uint64_t minUBOAlignment = RendererAPI::GPUSettings.UniformBufferAlignment;
	uint64_t alignedSize = byteSize;

	if(minUBOAlignment > 0)
		alignedSize = (alignedSize + minUBOAlignment - 1) & ~(minUBOAlignment - 1);

	return alignedSize;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Init(const void* const data, const uint64_t size,
																			   const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP::Scope<UniformBuffer> buffer = TRAP::Scope<UniformBuffer>(new UniformBuffer(updateFrequency));

	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (updateFrequency == UpdateFrequency::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly :
	                                                                     RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Flags = (updateFrequency == UpdateFrequency::Static) ? RendererAPI::BufferCreationFlags::None :
																   RendererAPI::BufferCreationFlags::PersistentMap;
	desc.Desc.Descriptors = RendererAPI::DescriptorType::UniformBuffer;
	desc.Desc.Size = size;
	desc.Data = data;

	for(std::size_t i = 0; i < buffer->m_uniformBuffers.size(); ++i)
	{
		RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_tokens[i]);
		buffer->m_uniformBuffers[i] = desc.Buffer;
	}

	return buffer;
}