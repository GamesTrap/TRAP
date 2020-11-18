#include "TRAPPCH.h"

#include "VulkanCommandBuffer.h"

#include "VulkanPipeline.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Internals/VulkanInitializers.h"

TRAP::Graphics::API::Vulkan::CommandBuffer::CommandBuffer(VkCommandBuffer& commandBuffer)
	: m_commandBuffer(commandBuffer), m_recording(false)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::CommandBuffer::BindPipeline(Pipeline& pipeline, const VkPipelineBindPoint bindPoint)
{
	if (!m_recording)
		StartRecording(); 

	if (pipeline.GetPipeline())
		vkCmdBindPipeline(m_commandBuffer, bindPoint, pipeline.GetPipeline());
	else
		TP_CRITICAL(Log::RendererVulkanPrefix, "Graphics Pipeline is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::CommandBuffer::StartRecording()
{
	if(!m_recording)
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo = Initializers::CommandBufferBeginInfo();
		VkCall(vkBeginCommandBuffer(m_commandBuffer, &commandBufferBeginInfo));
		
		m_recording = true;
	}	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::CommandBuffer::EndRecording()
{
	if(m_recording)
	{
		VkCall(vkEndCommandBuffer(m_commandBuffer));
		
		m_recording = false;
	}	
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::Vulkan::CommandBuffer::IsRecording() const
{
	return m_recording;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::CommandBuffer::SetViewport(VkViewport& viewport) const
{
	if(m_recording)
		vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::CommandBuffer::SetScissor(VkRect2D& scissor) const
{
	if(m_recording)
		vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);
}

//-------------------------------------------------------------------------------------------------------------------//

VkCommandBuffer& TRAP::Graphics::API::Vulkan::CommandBuffer::GetCommandBuffer()
{
	return m_commandBuffer;
}