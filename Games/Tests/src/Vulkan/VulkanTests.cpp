#include "VulkanTests.h"

#include "Graphics/API/Objects/DescriptorPool.h"
#include "Graphics/API/Objects/DescriptorSet.h"

VulkanTests::VulkanTests()
	: Layer("VulkanTests"),
	  m_window(nullptr),
	  m_wireFrame(false),
	  m_quad(false),
	  m_indexed(false),
	  m_ubo(true),
	  m_vsync(true)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnAttach()
{
	TRAP::VFS::SetHotShaderReloading(true);
	TRAP::VFS::MountShaders("Assets/Shaders");
	
	TRAP::Application::GetWindow()->SetTitle("Vulkan Multi-Window Test");

	TRAP::Graphics::RendererAPI::GetRenderer()->SetVSync(m_vsync);

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

	TRAP::Graphics::ShaderManager::LoadFile("VKTest", "/shaders/testubo.shader")->Use();
	TRAP::Graphics::ShaderManager::LoadFile("VKTestNoUBO", "/shaders/test.shader")->Use();

	//m_uniformBufferFragment = TRAP::Graphics::UniformBuffer::Create("uboF", 0, sizeof(TRAP::Math::Vec4), TRAP::Graphics::BufferUsage::Stream);
	//m_uniformBufferVertex = TRAP::Graphics::UniformBuffer::Create("uboV", 1, sizeof(UniformBufferObject), TRAP::Graphics::BufferUsage::Stream);

	///////////////////////////////////////////////
	///INTERNAL ENGINE CODE - Usage at own risk!///
	///////////////////////////////////////////////
	m_uboVertexBuffer.resize(3);
	m_uboFragmentBuffer.resize(3);
	
	/*TRAP::Graphics::RendererAPI::DescriptorSetDesc desc{};
	desc.RootSignature = TRAP::Graphics::RendererAPI::GetRenderer()->GetGraphicsRootSignature();
	desc.UpdateFrequency = TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency::PerFrame;
	desc.MaxSets = 3u;
	m_uboSet = TRAP::Graphics::RendererAPI::GetDescriptorPool()->RetrieveDescriptorSet(desc);*/

	for(uint32_t i = 0; i < 3u; ++i)
	{
		TRAP::Graphics::RendererAPI::BufferLoadDesc ubDescV{};
		ubDescV.Desc.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::UniformBuffer;
		ubDescV.Desc.MemoryUsage = TRAP::Graphics::RendererAPI::ResourceMemoryUsage::CPUToGPU;
		ubDescV.Desc.Size = sizeof(TRAP::Math::Vec3);
		ubDescV.Desc.Flags = TRAP::Graphics::RendererAPI::BufferCreationFlags::PersistentMap;
		ubDescV.Data = nullptr;
		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(ubDescV, nullptr);
		m_uboVertexBuffer[i] = ubDescV.Buffer;

		TRAP::Graphics::RendererAPI::BufferLoadDesc ubDescF{};
		ubDescF.Desc.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::UniformBuffer;
		ubDescF.Desc.MemoryUsage = TRAP::Graphics::RendererAPI::ResourceMemoryUsage::CPUToGPU;
		ubDescF.Desc.Size = sizeof(TRAP::Math::Vec4);
		ubDescF.Desc.Flags = TRAP::Graphics::RendererAPI::BufferCreationFlags::PersistentMap;
		ubDescF.Data = nullptr;
		TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(ubDescF, nullptr);
		m_uboFragmentBuffer[i] = ubDescF.Buffer;
	}

	TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

	/*for(uint32_t i = 0; i < 3u; ++i)
	{
		std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(2);
		params[0].Name = "ColorV";
		params[0].Resource = std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>{ m_uboVertexBuffer[i] };
		params[1].Name = "ColorF";
		params[1].Resource = std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>{ m_uboFragmentBuffer[i] };
		m_uboSet->Update(i, params);
	}*/
	
	TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();
	///////////////////////////////////////////////
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnDetach()
{
	if (s_window)
		m_window.reset();

	//m_uniformBufferFragment.reset();
	//m_uniformBufferVertex.reset();
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

	if (m_ubo)
		TRAP::Graphics::ShaderManager::Get("VKTest")->Use();
	else
		TRAP::Graphics::ShaderManager::Get("VKTestNoUBO")->Use();

	if(m_ubo)
	{
		/*if(!m_uboSet || m_cachedRootSignature != TRAP::Graphics::RendererAPI::GetRenderer()->GetGraphicsRootSignature().get())
		{
			m_cachedRootSignature = TRAP::Graphics::RendererAPI::GetRenderer()->GetGraphicsRootSignature().get();
			TRAP::Graphics::RendererAPI::DescriptorSetDesc desc{};
			desc.RootSignature = TRAP::Graphics::RendererAPI::GetRenderer()->GetGraphicsRootSignature();
			desc.UpdateFrequency = TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency::PerFrame;
			desc.MaxSets = 3u;
			m_uboSet = TRAP::Graphics::RendererAPI::GetDescriptorPool()->RetrieveDescriptorSet(desc);

			for (uint32_t i = 0; i < 3; ++i)
			{
				std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(2);
				params[0].Name = "ColorV";
				params[0].Resource = std::vector<TRAP::Graphics::Buffer*>{ m_uboVertexBuffer[i].get() };
				params[1].Name = "ColorF";
				params[1].Resource = std::vector<TRAP::Graphics::Buffer*>{ m_uboFragmentBuffer[i].get() };
				m_uboSet->Update(i, params);
			}
		}*/
		
		
		/*UniformBufferObject ubo{};
		ubo.Model = TRAP::Math::Rotate(TRAP::Math::Mat4(1.0f), TRAP::Application::GetTime() * TRAP::Math::Radians(90.0f), TRAP::Math::Vec3(0.0f, 0.0f, 1.0f));
		ubo.View = TRAP::Math::LookAt(TRAP::Math::Vec3(2.0f), TRAP::Math::Vec3(0.0f), TRAP::Math::Vec3(0.0f, 0.0f, 1.0f));
		ubo.Proj = TRAP::Math::Perspective(TRAP::Math::Radians(45.0f), static_cast<float>(TRAP::Application::GetWindow()->GetWidth()) / static_cast<float>(TRAP::Application::GetWindow()->GetHeight()), 0.1f, 10.0f);*/
		//m_uniformBufferVertex->SetData(&ubo, sizeof(UniformBufferObject));

		const uint32_t currentImageIndex = TRAP::Graphics::RendererAPI::GetRenderer()->GetPerWindowData(TRAP::Application::GetWindow().get())->ImageIndex;
		TRAP::Math::Vec3 MultiplierV(1.0f);
		static float countVertex = 0.0f;
		countVertex += deltaTime.GetMilliseconds();
		if (countVertex > 2000.0f)
		{			
			MultiplierV = TRAP::Math::Vec3(1.5f);
			if (countVertex > 4000.0f)
				countVertex = 0.0f;
		}
		
		///////////////////////////////////////////////
		///INTERNAL ENGINE CODE - Usage at own risk!///
		///////////////////////////////////////////////
		TRAP::Graphics::RendererAPI::BufferUpdateDesc descV{ m_uboVertexBuffer[currentImageIndex] };
		descV.DstOffset = 0;
		TRAP::Graphics::RendererAPI::GetResourceLoader()->BeginUpdateResource(descV);
		//std::memcpy(descV.MappedData, &ubo, sizeof(UniformBufferObject));
		std::memcpy(descV.MappedData, &MultiplierV, sizeof(TRAP::Math::Vec3));
		TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(descV, nullptr);
		///////////////////////////////////////////////		

		/*TRAP::Math::Vec4 color{ 1.0f };
		for(uint32_t i = 0; i < TRAP::Math::Vec4::Length(); ++i)
			color[i] = TRAP::Utils::Random::Get(0.0f, 1.0f);*/
		//m_uniformBufferFragment->SetData(&color, sizeof(TRAP::Math::Vec4));

		TRAP::Math::Vec4 ColorF(1.0f);
		static float countFragment = 0.0f;
		countFragment += deltaTime.GetMilliseconds();
		if (countFragment > 1000.0f)
		{
			if (countFragment > 2000.0f)
				ColorF = TRAP::Math::Vec4(0.0f, 0.0f, 1.0f, 1.0f);
			else
				ColorF = TRAP::Math::Vec4(0.0f, 1.0f, 0.0f, 1.0f);

			if (countFragment > 3000.0f)
				countFragment = 0.0f;
		}

		///////////////////////////////////////////////
		///INTERNAL ENGINE CODE - Usage at own risk!///
		///////////////////////////////////////////////
		TRAP::Graphics::RendererAPI::BufferUpdateDesc descF{ m_uboFragmentBuffer[currentImageIndex] };
		descF.DstOffset = 0;
		TRAP::Graphics::RendererAPI::GetResourceLoader()->BeginUpdateResource(descF);
		//std::memcpy(descF.MappedData, &color, sizeof(TRAP::Math::Vec4));
		std::memcpy(descF.MappedData, &ColorF, sizeof(TRAP::Math::Vec4));
		TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(descF, nullptr);
		TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();
		
		//TRAP::Graphics::RendererAPI::GetRenderer()->BindDescriptorSet(*m_uboSet, 0);
		///////////////////////////////////////////////
	}
	//m_uniformBufferFragment->Use();
	
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
		if (e.GetTitle() == m_window->GetTitle())
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
		m_ubo = !m_ubo;
		TP_TRACE("[VulkanTests] UniformBuffer: ", m_ubo ? "On" : "Off");
	}
	if(e.GetKey() == TRAP::Input::Key::V)
	{
		m_vsync = !m_vsync;
		TP_TRACE("[VulkanTests] VSync: ", m_vsync ? "On" : "Off");
		TRAP::Graphics::RendererAPI::GetRenderer()->SetVSync(m_vsync);
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