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
		m_cameraController(static_cast<float>(TRAP::Application::GetWindow()->GetWidth()) / static_cast<float>(TRAP::Application::GetWindow()->GetHeight()))
	{
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnImGuiRender() override
	{
		ImGui::SetNextWindowBgAlpha(0.3f);
		ImGui::Begin("Performance", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
		ImGui::Text("Performance");
		ImGui::Separator();
		ImGui::Text("DrawCalls: %u", TRAP::Graphics::Renderer::GetDrawCalls());
		ImGui::Text("FPS: %u", TRAP::Graphics::Renderer::GetFPS());
		ImGui::Text("FrameTime: %.3fms", TRAP::Graphics::Renderer::GetFrameTime());
		ImGui::PlotLines("", m_frameTimeHistory.data(), static_cast<int>(m_frameTimeHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::End();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnAttach() override
	{
		//Mount & Load Shaders
		TRAP::VFS::Get()->MountShaders("Assets/Shaders");
		TRAP::Graphics::ShaderManager::Load("/Shaders/Color.shader");
		TRAP::Graphics::ShaderManager::Load("/Shaders/Texture.shader");
		TRAP::Graphics::ShaderManager::Load("/Shaders/TextureColor.shader");

		//Mount & Load Textures
		TRAP::VFS::Get()->MountTextures("Assets/Textures");
		TRAP::Graphics::TextureManager::Load("TRAP", "/Textures/TRAPWhiteLogo2048x2048.tga");

		///////////////
		//    Quad   //
		///////////////
		m_vertexArray = TRAP::Graphics::VertexArray::Create();

		//XYZ RGBA
		std::array<float, 9 * 4> vertices //Quad
		{
			-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f, 1.0f,    1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f
		};
		TRAP::Scope<TRAP::Graphics::VertexBuffer> vertexBuffer = TRAP::Graphics::VertexBuffer::Create(vertices.data(), static_cast<uint32_t>(vertices.size()));
		const TRAP::Graphics::BufferLayout layout =
		{
			{TRAP::Graphics::ShaderDataType::Float3, "Position"},
			{TRAP::Graphics::ShaderDataType::Float4, "Color"},
			{TRAP::Graphics::ShaderDataType::Float2, "UV"}
		};
		vertexBuffer->SetLayout(layout);
		vertexBuffer->SetLayout(layout);
		m_vertexArray->AddVertexBuffer(vertexBuffer);

		std::array<uint32_t, 6> indices //Quad
		{
			0, 1, 2, 2, 3, 0
		};
		TRAP::Scope<TRAP::Graphics::IndexBuffer> indexBuffer = TRAP::Graphics::IndexBuffer::Create(indices.data(), static_cast<uint32_t>(indices.size()));
		m_vertexArray->SetIndexBuffer(indexBuffer);

		TRAP::Graphics::RenderCommand::SetClearColor();
		TRAP::Graphics::RenderCommand::SetCull(false);
		TRAP::Graphics::RenderCommand::SetBlend(true);
		TRAP::Graphics::RenderCommand::SetBlendFunction(TRAP::Graphics::RendererBlendFunction::Source_Alpha, TRAP::Graphics::RendererBlendFunction::One_Minus_Source_Alpha);

		UniformData data{ TRAP::Application::GetTime() };
		m_uniformBuffer = TRAP::Graphics::UniformBuffer::Create("ColorBuffer", &data, sizeof(UniformData), TRAP::Graphics::BufferUsage::Stream);

		TRAP::Graphics::TextureManager::Get2D("TRAP")->Bind();
		m_uniformBuffer->Bind(1);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnDetach() override
	{
		m_uniformBuffer->Unbind();
		m_uniformBuffer.reset();

		m_vertexArray->Unbind();
		m_vertexArray.reset();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnUpdate(const TRAP::Utils::TimeStep deltaTime) override
	{
		m_cameraController.OnUpdate(deltaTime);

		TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color_Depth);

		TRAP::Graphics::Renderer::BeginScene(m_cameraController.GetCamera());
		{
			if (m_usePassthrough)
				TRAP::Graphics::Renderer::Submit(TRAP::Graphics::ShaderManager::Get("Passthrough"), m_vertexArray);
			else
			{
				float time = TRAP::Application::GetTime();
				m_uniformBuffer->UpdateData(&time);
				TRAP::Graphics::Renderer::Submit(TRAP::Graphics::ShaderManager::Get("TextureColor"), m_vertexArray);
			}
		}
		TRAP::Graphics::Renderer::EndScene();

		//FPS & FrameTime
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

	bool OnKeyPressed(TRAP::KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == TRAP::Input::Key::Escape)
			TRAP::Application::Shutdown();

		if (event.GetKeyCode() == TRAP::Input::Key::F1 && event.GetRepeatCount() < 1) //Switch to D3D12
			TRAP::Graphics::API::Context::SwitchRenderAPI(TRAP::Graphics::API::RenderAPI::D3D12);
		if (event.GetKeyCode() == TRAP::Input::Key::F2 && event.GetRepeatCount() < 1) //Switch to Vulkan
			TRAP::Graphics::API::Context::SwitchRenderAPI(TRAP::Graphics::API::RenderAPI::Vulkan);
		if (event.GetKeyCode() == TRAP::Input::Key::F3 && event.GetRepeatCount() < 1) //Switch to OpenGL
			TRAP::Graphics::API::Context::SwitchRenderAPI(TRAP::Graphics::API::RenderAPI::OpenGL);

		if (event.GetKeyCode() == TRAP::Input::Key::F4 && event.GetRepeatCount() < 1) //Use Default/Passthrough Shader
			m_usePassthrough = !m_usePassthrough;

		if (event.GetKeyCode() == TRAP::Input::Key::F5 && event.GetRepeatCount() < 1) //Make Window windowed
			TRAP::Application::GetWindow()->SetDisplayMode(TRAP::Window::DisplayMode::Windowed);
		if (event.GetKeyCode() == TRAP::Input::Key::F6 && event.GetRepeatCount() < 1) //Make Window Borderless Fullscreen
			TRAP::Application::GetWindow()->SetDisplayMode(TRAP::Window::DisplayMode::Borderless);
		if (event.GetKeyCode() == TRAP::Input::Key::F7 && event.GetRepeatCount() < 1) //Make Window Exclusive Fullscreen
			TRAP::Application::GetWindow()->SetDisplayMode(TRAP::Window::DisplayMode::Fullscreen);

		if (event.GetKeyCode() == TRAP::Input::Key::F10 && event.GetRepeatCount() < 1) //Enable/Disable WireFrame Mode
		{
			m_wireFrame = !m_wireFrame;
			TRAP::Graphics::RenderCommand::SetWireFrame(m_wireFrame);
		}

		if (event.GetKeyCode() == TRAP::Input::Key::F11 && event.GetRepeatCount() < 1)
			TRAP::Utils::MsgBox::Show("Just a prank bro!", "Critical Error");

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnEvent(TRAP::Event& event) override
	{
		m_cameraController.OnEvent(event);

		TRAP::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<TRAP::KeyPressedEvent>(TRAP_BIND_EVENT_FN(SandboxLayer::OnKeyPressed));
	}

private:
	std::array<float, 50> m_frameTimeHistory;
	TRAP::Utils::Timer m_fpsTimer;
	TRAP::Utils::Timer m_titleTimer;
	bool m_usePassthrough;
	bool m_wireFrame;

	TRAP::Graphics::OrthographicCameraController m_cameraController;

	TRAP::Scope<TRAP::Graphics::VertexArray> m_vertexArray{};

	TRAP::Scope<TRAP::Graphics::UniformBuffer> m_uniformBuffer{};
	struct UniformData
	{
		float Time{};
	};
};

#endif /*_GAMESTRAP_SANDBOXLAYER_H_*/