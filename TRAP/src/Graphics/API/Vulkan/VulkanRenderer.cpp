#include "TRAPPCH.h"
#include "VulkanRenderer.h"

#include "Graphics/RenderCommand.h"
#include "Window/Window.h"
#include "Window/WindowingAPI.h"

TRAP::Graphics::API::VulkanRenderer* TRAP::Graphics::API::VulkanRenderer::s_renderer = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::VulkanRenderer()
{
	s_renderer = this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::~VulkanRenderer()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::InitInternal()
{
	TP_INFO(Log::RendererVulkanPrefix, "----------------------------------");
	TP_INFO(Log::RendererVulkanPrefix, "Vulkan:");
	TP_INFO(Log::RendererVulkanPrefix, "Version:  0.0.0");
	TP_INFO(Log::RendererVulkanPrefix, "Renderer: XXX");
	TP_INFO(Log::RendererVulkanPrefix, "Driver:   0.0.0");
	TP_INFO(Log::RendererVulkanPrefix, "----------------------------------");

	m_rendererTitle = "[Vulkan 0.0.0]";
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Clear(RendererBufferType buffer)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Present(const Scope<Window>& window)
{
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetClearColor(const Math::Vec4& color)
{
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthTesting(bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthMasking(bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthFunction(RendererFunction function)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilTesting(bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilMasking(uint32_t mask)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilMaskingSeparate(RendererFaceMode face, uint32_t mask)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilFunction(RendererFunction function, int32_t reference, uint32_t mask)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilFunctionSeparate(RendererFaceMode face, RendererFunction function, int32_t reference, uint32_t mask)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilOperation(RendererOperation stencilFail, RendererOperation depthFail, RendererOperation pass)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilOperationSeparate(RendererFaceMode face, RendererOperation stencilFail, RendererOperation depthFail, RendererOperation pass)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlend(const bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetCull(const bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetFrontFace(const RendererFrontFace frontFace)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetWireFrame(const bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendFunction(const RendererBlendFunction source, const RendererBlendFunction destination)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendFunctionSeparate(RendererBlendFunction sourceRGB,
	                                                               RendererBlendFunction sourceAlpha,
	                                                               RendererBlendFunction destinationRGB,
	                                                               RendererBlendFunction destinationAlpha)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendEquation(RendererBlendEquation blendEquation)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendEquationSeparate(RendererBlendEquation blendEquationRGB, RendererBlendEquation blendEquationAlpha)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetCullMode(const RendererFaceMode cullMode)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::DrawIndexed(const Scope<VertexArray>& vertexArray, uint32_t indexCount, const RendererPrimitive primitive)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Draw(const Scope<VertexArray>& vertexArray, const RendererPrimitive primitive)
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::API::VulkanRenderer::GetTitle() const
{
	return m_rendererTitle;
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<uint8_t, 16> TRAP::Graphics::API::VulkanRenderer::GetCurrentGPUUUID()
{
	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::VulkanRenderer::GetCurrentGPUName()
{
	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::pair<std::string, std::array<uint8_t, 16>>> TRAP::Graphics::API::VulkanRenderer::GetAllGPUs()
{
	return {};
}