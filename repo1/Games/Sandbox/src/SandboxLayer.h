#ifndef _GAMESTRAP_SANDBOXLAYER_H_
#define _GAMESTRAP_SANDBOXLAYER_H_

#include <TRAP.h>

class SandboxLayer final : public TRAP::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox"),
		m_frameTimeHistory(),
		m_usePassthrough(false),
		m_wireFrame(false),
		m_indexedDrawing(true),
		m_cameraController(static_cast<float>(TRAP::Application::GetWindow()->GetWidth()) / static_cast<float>(TRAP::Application::GetWindow()->GetHeight()))
	{
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnImGuiRender() override
	{
		TP_PROFILE_FUNCTION();
		
		ImGui::SetNextWindowBgAlpha(0.3f);
		ImGui::Begin("Performance", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
		ImGui::Text("Performance");
		ImGui::Separator();
		ImGui::Text("CPU: %ix %s", TRAP::Application::GetCPUInfo().LogicalCores, TRAP::Application::GetCPUInfo().Model.c_str());
		ImGui::Text("GPU: %s", TRAP::Graphics::API::RendererAPI::GetRenderer()->GetCurrentGPUName().c_str());
		ImGui::Text("DrawCalls: %u", TRAP::Graphics::Renderer::GetDrawCalls());
		ImGui::Text("FPS: %u", TRAP::Graphics::Renderer::GetFPS());
		ImGui::Text("FrameTime: %.3fms", TRAP::Graphics::Renderer::GetFrameTime());
		ImGui::PlotLines("", m_frameTimeHistory.data(), static_cast<int>(m_frameTimeHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::End();

		ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav);
		ImGui::Checkbox("Indexed Drawing", &m_indexedDrawing);
		ImGui::End();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnAttach() override
	{
		TP_PROFILE_FUNCTION();

		TRAP::Application::SetHotShaderReloading(true);
		TRAP::Application::SetHotTextureReloading(true);
		TRAP::Application::GetWindow()->SetTitle("Sandbox");
		
		//Mount & Load Shader
		TRAP::VFS::MountShaders("Assets/Shaders");
		TRAP::Graphics::ShaderManager::Load("/Shaders/TextureColor.shader");
		//TRAP::Graphics::ShaderManager::Load("TextureColor", "/Shaders/TextureColorSPIRV.spirv");
		
		//Mount & Load Texture
		TRAP::VFS::MountTextures("Assets/Textures");
		TRAP::Graphics::TextureManager::Load("TRAP", "/Textures/TRAPWhiteLogo2048x2048.png");

		//Indexed
		///////////////
		//    Quad   //
		///////////////
		//XYZ RGBA
		std::array<float, 9 * 4> indexedVertices //Quad
		{
			-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f, 1.0f,    1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f
		};
		TRAP::Scope<TRAP::Graphics::VertexBuffer> indexedVertexBuffer = TRAP::Graphics::VertexBuffer::Create(indexedVertices.data(), static_cast<uint32_t>(indexedVertices.size()) * sizeof(uint32_t));
		const TRAP::Graphics::BufferLayout layout =
		{
			{TRAP::Graphics::ShaderDataType::Float3, "Position"},
			{TRAP::Graphics::ShaderDataType::Float4, "Color"},
			{TRAP::Graphics::ShaderDataType::Float2, "UV"}
		};
		indexedVertexBuffer->SetLayout(layout);
		m_indexedVertexArray = TRAP::Graphics::VertexArray::Create();
		m_indexedVertexArray->SetVertexBuffer(indexedVertexBuffer);

		std::array<uint32_t, 6> indices //Quad
		{
			0, 1, 2, 2, 3, 0
		};
		TRAP::Scope<TRAP::Graphics::IndexBuffer> indexBuffer = TRAP::Graphics::IndexBuffer::Create(indices.data(), static_cast<uint32_t>(indices.size()));
		m_indexedVertexArray->SetIndexBuffer(indexBuffer);

		//Non-Indexed
		///////////////
		//    Quad   //
		///////////////
		//XYZ RGBA
		std::array<float, 9 * 6> vertices //Quad
		{
			-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f, 1.0f,    1.0f, 1.0f,
			0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f, 1.0f,    1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f,
			-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 0.0f
		};
		TRAP::Scope<TRAP::Graphics::VertexBuffer> vertexBuffer = TRAP::Graphics::VertexBuffer::Create(vertices.data(), static_cast<uint32_t>(vertices.size()) * sizeof(uint32_t));
		vertexBuffer->SetLayout(layout);		
		m_vertexArray = TRAP::Graphics::VertexArray::Create();
		m_vertexArray->SetVertexBuffer(vertexBuffer);

		TRAP::Graphics::RenderCommand::SetClearColor();
		TRAP::Graphics::RenderCommand::SetCull(false);
		TRAP::Graphics::RenderCommand::SetBlend(true);
		TRAP::Graphics::RenderCommand::SetBlendFunction(TRAP::Graphics::RendererBlendFunction::Source_Alpha, TRAP::Graphics::RendererBlendFunction::One_Minus_Source_Alpha);

		UniformData data{ TRAP::Application::GetTime() };
		m_uniformBuffer = TRAP::Graphics::UniformBuffer::Create("ColorBuffer", &data, sizeof(UniformData), TRAP::Graphics::BufferUsage::Dynamic);

		m_uniformBuffer->Bind(1);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnDetach() override
	{
		TP_PROFILE_FUNCTION();
		
		m_uniformBuffer->Unbind();
		m_uniformBuffer.reset();
		
		m_indexedVertexArray->Unbind();
		m_indexedVertexArray.reset();

		m_vertexArray->Unbind();
		m_vertexArray.reset();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override
	{
		TP_PROFILE_FUNCTION();

		//Update
		m_cameraController.OnUpdate(deltaTime);

		//Render
		TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color_Depth);		
		
		TRAP::Graphics::Renderer::BeginScene(m_cameraController.GetCamera());
		{
			if (m_usePassthrough)
			{
				if (m_indexedDrawing)
	 				TRAP::Graphics::Renderer::Submit(TRAP::Graphics::ShaderManager::Get("Fallback"), m_indexedVertexArray);
				else
	 				TRAP::Graphics::Renderer::Submit(TRAP::Graphics::ShaderManager::Get("Fallback"), m_vertexArray);
			}
			else
			{
				TRAP::Graphics::TextureManager::Get2D("TRAP")->Bind(0);
				float time = TRAP::Application::GetTime();
				m_uniformBuffer->UpdateData(&time);
				if (m_indexedDrawing)
					TRAP::Graphics::Renderer::Submit(TRAP::Graphics::ShaderManager::Get("TextureColor"), m_indexedVertexArray);
				else
					TRAP::Graphics::Renderer::Submit(TRAP::Graphics::ShaderManager::Get("TextureColor"), m_vertexArray);
			}
		}
		TRAP::Graphics::Renderer::EndScene();

		//Update FPS & FrameTIme history
		if (m_titleTimer.Elapsed() >= 0.025f)
		{
			m_titleTimer.Reset();
			static int frameTimeIndex = 0;
			if (frameTimeIndex < static_cast<int>(m_frameTimeHistory.size() - 1))
			{
				m_frameTimeHistory[frameTimeIndex] = TRAP::Graphics::Renderer::GetFrameTime();
				frameTimeIndex++;
			}
			else
			{
				std::move(m_frameTimeHistory.begin() + 1, m_frameTimeHistory.end(), m_frameTimeHistory.begin());
				m_frameTimeHistory[m_frameTimeHistory.size() - 1] = TRAP::Graphics::Renderer::GetFrameTime();
			}
		}
		if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
		{
			TP_INFO("[Sandbox] DrawCall(s): ", TRAP::Graphics::Renderer::GetDrawCalls());
			TP_INFO("[Sandbox] FPS: ", TRAP::Graphics::Renderer::GetFPS());
			TP_INFO("[Sandbox] FrameTime: ", TRAP::Graphics::Renderer::GetFrameTime(), "ms");
			m_fpsTimer.Reset();
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool OnKeyPress(TRAP::Events::KeyPressEvent& event)
	{
		if (event.GetKey() == TRAP::Input::Key::Escape)
			TRAP::Application::Shutdown();
		
		if (event.GetKey() == TRAP::Input::Key::F1 && event.GetRepeatCount() < 1) //Switch to Vulkan
			TRAP::Graphics::API::Context::SwitchRenderAPI(TRAP::Graphics::API::RenderAPI::Vulkan);
		if (event.GetKey() == TRAP::Input::Key::F2 && event.GetRepeatCount() < 1) //Switch to OpenGL
			TRAP::Graphics::API::Context::SwitchRenderAPI(TRAP::Graphics::API::RenderAPI::OpenGL);

		if (event.GetKey() == TRAP::Input::Key::F4 && event.GetRepeatCount() < 1) //Use Fallback Shader
			m_usePassthrough = !m_usePassthrough;

		if (event.GetKey() == TRAP::Input::Key::F5 && event.GetRepeatCount() < 1) //Make Window windowed
			TRAP::Application::GetWindow()->SetDisplayMode(TRAP::Window::DisplayMode::Windowed);
		if (event.GetKey() == TRAP::Input::Key::F6 && event.GetRepeatCount() < 1) //Make Window Borderless Fullscreen
			TRAP::Application::GetWindow()->SetDisplayMode(TRAP::Window::DisplayMode::Borderless);
		if (event.GetKey() == TRAP::Input::Key::F7 && event.GetRepeatCount() < 1) //Make Window Exclusive Fullscreen
			TRAP::Application::GetWindow()->SetDisplayMode(TRAP::Window::DisplayMode::Fullscreen);

		if (event.GetKey() == TRAP::Input::Key::F10 && event.GetRepeatCount() < 1) //Enable/Disable WireFrame Mode
		{
			m_wireFrame = !m_wireFrame;
			TRAP::Graphics::RenderCommand::SetWireFrame(m_wireFrame);
		}

		if (event.GetKey() == TRAP::Input::Key::F11 && event.GetRepeatCount() < 1)
			TRAP::Utils::Dialogs::MsgBox::Show("Just a prank bro!", "Critical Error");

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnEvent(TRAP::Events::Event& event) override
	{
		m_cameraController.OnEvent(event);

		TRAP::Events::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e) { return OnKeyPress(e); });
	}

private:
	std::array<float, 50> m_frameTimeHistory;
	TRAP::Utils::Timer m_fpsTimer;
	TRAP::Utils::Timer m_titleTimer;
	bool m_usePassthrough;
	bool m_wireFrame;
	bool m_indexedDrawing;

	TRAP::Graphics::OrthographicCameraController m_cameraController;

	TRAP::Scope<TRAP::Graphics::VertexArray> m_indexedVertexArray{};
	TRAP::Scope<TRAP::Graphics::VertexArray> m_vertexArray{};

	TRAP::Scope<TRAP::Graphics::UniformBuffer> m_uniformBuffer{};
	struct UniformData
	{
		float Time{};
	};
};

#endif /*_GAMESTRAP_SANDBOXLAYER_H_*/