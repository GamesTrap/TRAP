#include "VulkanTests.h"

#include <Graphics/API/Objects/DescriptorPool.h>
#include <Graphics/API/Objects/DescriptorSet.h>

VulkanTests::VulkanTests()
	: Layer("VulkanTests"),
	  m_window(nullptr),
	  m_wireFrame(false),
	  m_quad(false),
	  m_indexed(false),
	  m_vsync(TRAP::Application::GetConfig().Get<bool>("VSync")),
	  m_pushConstantOrUBO(0),
	  m_descriptorSet(nullptr),
	  m_colorData(),
	  m_sizeMultiplicatorData(),
	  m_colorTimer(),
	  m_vertexTimer()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnAttach()
{
	TRAP::VFS::SetHotShaderReloading(true);
	TRAP::VFS::MountShaders("Assets/Shaders");

	TRAP::Application::GetWindow()->SetTitle("Vulkan Multi-Window Test");

	if (s_window)
	{
		TRAP::WindowProps windowProps
		{
			"Vulkan Test",
			200,
			200,
			60,
			false,
			TRAP::Window::DisplayMode::Windowed,
			TRAP::WindowProps::AdvancedProps
			{
				true,
				false,
				true,
				true,
				true,
				true,
				false,
				TRAP::Window::CursorMode::Normal
			},
			0
		};
		m_window = TRAP::MakeScope<TRAP::Window>(windowProps);
		m_window->SetEventCallback([this](TRAP::Events::Event& e) { OnEvent(e); });
		TRAP::Graphics::RendererAPI::GetRenderer()->SetClearColor({ 1.0f, 0.0f, 1.0f, 1.0f }, m_window.get());
	}

	m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(m_triangleVertices.data(), static_cast<uint32_t>(m_quadVertices.size()) * sizeof(float), TRAP::Graphics::BufferUsage::Dynamic);
	const TRAP::Graphics::BufferLayout layout =
	{
		{TRAP::Graphics::ShaderDataType::Float3, "Pos"},
		{TRAP::Graphics::ShaderDataType::Float3, "Color"}
	};
	m_vertexBuffer->SetLayout(layout);
	m_vertexBuffer->AwaitLoading();
	m_vertexBuffer->Use();

	m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(m_triangleIndices.data(), static_cast<uint32_t>(m_quadIndices.size()) * sizeof(uint16_t), TRAP::Graphics::BufferUsage::Dynamic);
	m_indexBuffer->AwaitLoading();
	m_vertexBuffer->Use();

	TRAP::Graphics::ShaderManager::LoadFile("VKTest", "/shaders/test.shader");
	TRAP::Graphics::ShaderManager::LoadFile("VKTestPushConstant", "/shaders/testpushconstant.shader");
	TRAP::Graphics::ShaderManager::LoadFile("VKTestUBO", "/shaders/testubo.shader");

	TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

	//////////////////////////////////////////////
	//INTERNAL RENDERERAPI USE AT YOUR OWN RISK!//
	//////////////////////////////////////////////
	TRAP::Graphics::RendererAPI::BufferLoadDesc colorUniformBufferDesc{};
	colorUniformBufferDesc.Desc.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::UniformBuffer;
	colorUniformBufferDesc.Desc.MemoryUsage = TRAP::Graphics::RendererAPI::ResourceMemoryUsage::CPUToGPU;
	colorUniformBufferDesc.Desc.Size = sizeof(ColorData);
	colorUniformBufferDesc.Desc.Flags = TRAP::Graphics::RendererAPI::BufferCreationFlags::PersistentMap;
	colorUniformBufferDesc.Data = nullptr;
	colorUniformBufferDesc.Desc.Name = "Color Uniform Buffer";

	for(uint32_t i = 0; i < ImageCount; ++i)
	{
		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(colorUniformBufferDesc, nullptr);
		m_colorUniformBuffer[i] = colorUniformBufferDesc.Buffer;
	}

	TRAP::Graphics::RendererAPI::BufferLoadDesc sizeMultiplicatorUniformBufferDesc{};
	sizeMultiplicatorUniformBufferDesc.Desc.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::UniformBuffer;
	sizeMultiplicatorUniformBufferDesc.Desc.MemoryUsage = TRAP::Graphics::RendererAPI::ResourceMemoryUsage::CPUToGPU;
	sizeMultiplicatorUniformBufferDesc.Desc.Size = sizeof(SizeMultiplicatorData);
	sizeMultiplicatorUniformBufferDesc.Desc.Flags = TRAP::Graphics::RendererAPI::BufferCreationFlags::PersistentMap;
	sizeMultiplicatorUniformBufferDesc.Data = nullptr;
	sizeMultiplicatorUniformBufferDesc.Desc.Name = "Size Multiplicator Uniform Buffer";

	for(uint32_t i = 0; i < ImageCount; ++i)
	{
		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(sizeMultiplicatorUniformBufferDesc, nullptr);
		m_sizeMultiplicatorUniformBuffer[i] = sizeMultiplicatorUniformBufferDesc.Buffer;
	}
	//////////////////////////////////////////////

	TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnDetach()
{
	if (s_window)
		m_window.reset();

	m_indexBuffer.reset();
	m_vertexBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	if (s_window && m_window)
		TRAP::Graphics::RenderCommand::Present(m_window);

	if(m_wireFrame)
		TRAP::Graphics::RendererAPI::GetRenderer()->SetFillMode(TRAP::Graphics::RendererAPI::FillMode::WireFrame);
	else
		TRAP::Graphics::RendererAPI::GetRenderer()->SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Solid);

	if(!m_quad)
	{
		m_vertexBuffer->SetData(m_triangleVertices.data(), static_cast<uint32_t>(m_triangleVertices.size()) * sizeof(float));
		if (m_indexed)
			m_indexBuffer->SetData(m_triangleIndices.data(), static_cast<uint32_t>(m_triangleIndices.size()) * sizeof(uint16_t));
	}
	else
	{
		if(!m_indexed)
			m_vertexBuffer->SetData(m_quadVertices.data(), static_cast<uint32_t>(m_quadVertices.size()) * sizeof(float));
		else
		{
			m_vertexBuffer->SetData(m_quadVerticesIndexed.data(), static_cast<uint32_t>(m_quadVerticesIndexed.size()) * sizeof(float));
			m_indexBuffer->SetData(m_quadIndices.data(), static_cast<uint32_t>(m_quadIndices.size()) * sizeof(uint16_t));
		}
	}

	m_indexBuffer->AwaitLoading();
	m_indexBuffer->Use();
	m_vertexBuffer->AwaitLoading();
	m_vertexBuffer->Use();

	if(m_pushConstantOrUBO == 1)
	{
		if(m_colorTimer.Elapsed() > 2.5f)
		{
			for(uint32_t i = 0; i < m_colorData.Color.Length(); ++i)
				m_colorData.Color[i] = TRAP::Utils::Random::Get(0.0f, 1.0f);

			m_colorTimer.Reset();
		}

		TRAP::Graphics::ShaderManager::Get("VKTestPushConstant")->Use();

		TRAP::Graphics::RendererAPI::GetRenderer()->BindPushConstantsByIndex(0, &m_colorData);
	}
	else if(m_pushConstantOrUBO == 2)
	{
		if(!m_descriptorSet)
		{
			//////////////////////////////////////////////
			//INTERNAL RENDERERAPI USE AT YOUR OWN RISK!//
			//////////////////////////////////////////////
			TRAP::Graphics::RendererAPI::DescriptorSetDesc desc{};
			desc.RootSignature = TRAP::Graphics::RendererAPI::GetGraphicsRootSignature();
			desc.UpdateFrequency = TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency::PerFrame;
			desc.MaxSets = ImageCount;
			m_descriptorSet = TRAP::Graphics::RendererAPI::GetDescriptorPool()->RetrieveDescriptorSet(desc);

			for(uint32_t i = 0; i < ImageCount; ++i)
			{
				std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(2);
				params[0].Name = "SizeMultiplicator";
				params[0].Resource = std::vector<TRAP::Graphics::Buffer*>{m_sizeMultiplicatorUniformBuffer[i].get()};
				params[1].Name = "Color";
				params[1].Resource = std::vector<TRAP::Graphics::Buffer*>{m_colorUniformBuffer[i].get()};
				m_descriptorSet->Update(i, params);
			}
			//////////////////////////////////////////////

			TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();
		}

		uint32_t imageIndex = TRAP::Graphics::RendererAPI::GetPerWindowData(TRAP::Application::GetWindow().get())->ImageIndex;

		if(m_vertexTimer.Elapsed() > 2.0f)
		{
			m_sizeMultiplicatorData.Multiplier = TRAP::Math::Vec3(1.5f);
			if(m_vertexTimer.Elapsed() > 4.0f)
				m_vertexTimer.Reset();
		}
		else
			m_sizeMultiplicatorData.Multiplier = TRAP::Math::Vec3(1.0f);

		if(m_colorTimer.Elapsed() > 1.0f)
		{
			if(m_colorTimer.Elapsed() > 2.0f)
				m_colorData.Color = TRAP::Math::Vec3(0.0f, 0.0f, 1.0f);
			else
				m_colorData.Color = TRAP::Math::Vec3(0.0f, 1.0f, 0.0f);

			if(m_colorTimer.Elapsed() > 3.0f)
				m_colorTimer.Reset();
		}
		else
			m_colorData.Color = TRAP::Math::Vec3(1.0f, 0.0f, 0.0f);

		//////////////////////////////////////////////
		//INTERNAL RENDERERAPI USE AT YOUR OWN RISK!//
		//////////////////////////////////////////////
		TRAP::Graphics::RendererAPI::BufferUpdateDesc descOne{};
		descOne.Buffer = m_sizeMultiplicatorUniformBuffer[imageIndex];
		TRAP::Graphics::RendererAPI::GetResourceLoader()->BeginUpdateResource(descOne);
		*(SizeMultiplicatorData*)descOne.MappedData = m_sizeMultiplicatorData;
		TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(descOne, nullptr);

		TRAP::Graphics::RendererAPI::BufferUpdateDesc descTwo{};
		descTwo.Buffer = m_colorUniformBuffer[imageIndex];
		TRAP::Graphics::RendererAPI::GetResourceLoader()->BeginUpdateResource(descTwo);
		*(ColorData*)descTwo.MappedData = m_colorData;
		TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(descTwo, nullptr);
		//////////////////////////////////////////////

		TRAP::Graphics::RendererAPI::GetRenderer()->BindDescriptorSet(*m_descriptorSet, imageIndex);

		TRAP::Graphics::ShaderManager::Get("VKTestUBO")->Use();
	}
	else
		TRAP::Graphics::ShaderManager::Get("VKTest")->Use();

	if(!m_indexed)
		TRAP::Graphics::RendererAPI::GetRenderer()->Draw(m_quad ? 6 : 3);
	else
		TRAP::Graphics::RendererAPI::GetRenderer()->DrawIndexed(static_cast<uint32_t>(m_indexBuffer->GetCount()));

	if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
	{
		TP_INFO("[Sandbox] FPS: ", TRAP::Graphics::Renderer::GetFPS());
		TP_INFO("[Sandbox] FrameTime: ", TRAP::Graphics::Renderer::GetFrameTime(), "ms");
		m_fpsTimer.Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnImGuiRender()
{
	ImGui::Begin("LOL");
	ImGui::Text("HELLO IMGUI");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::WindowCloseEvent>([this](TRAP::Events::WindowCloseEvent& e) { return OnWindowClose(e); });
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e) { return OnKeyPress(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool VulkanTests::OnWindowClose(TRAP::Events::WindowCloseEvent& e)
{
	if (s_window)
	{
		if (e.GetWindow() == m_window.get())
			m_window.reset();
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool VulkanTests::OnKeyPress(TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::F1)
	{
		m_wireFrame = !m_wireFrame;
		TP_TRACE("[VulkanTests] WireFrame: ", m_wireFrame ? "Enabled" : "Disabled");
	}
	if (e.GetKey() == TRAP::Input::Key::F2)
	{
		m_quad = !m_quad;
		TP_TRACE("[VulkanTests] Geometry: ", m_quad ? "Quad" : "Triangle");
	}
	if(e.GetKey() == TRAP::Input::Key::F3)
	{
		m_indexed = !m_indexed;
		TP_TRACE("[VulkanTests] Indexed Drawing: ", m_indexed ? "On" : "Off");
	}
	if(e.GetKey() == TRAP::Input::Key::F4)
	{
		m_pushConstantOrUBO = ++m_pushConstantOrUBO % 3;
		TP_TRACE("[VulkanTests] Push Constant / Uniform Buffer: ", m_pushConstantOrUBO ? "On" : "Off");
	}
	if(e.GetKey() == TRAP::Input::Key::V)
	{
		m_vsync = !m_vsync;
		TP_TRACE("[VulkanTests] VSync: ", m_vsync ? "On" : "Off");
		e.GetWindow()->SetVSync(m_vsync);
	}
	if (e.GetKey() == TRAP::Input::Key::Escape)
	{
		if(s_window && m_window)
			m_window.reset();
		else
			TRAP::Application::Shutdown();
	}

	return false;
}