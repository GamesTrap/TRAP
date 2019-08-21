#pragma once

#include <TRAP.h>

class SandboxLayer final : public TRAP::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox"),
		m_frameTimeHistory(),
		m_usePassthrough(false),
		m_wireFrame(false),
		m_show(true),
		m_vertexArray(nullptr),
		m_camera
		(-(static_cast<float>(TRAP::Application::Get().GetWindow()->GetWidth()) / static_cast<float>(TRAP::Application::Get().GetWindow()->GetHeight())),
			static_cast<float>(TRAP::Application::Get().GetWindow()->GetWidth()) / static_cast<float>(TRAP::Application::Get().GetWindow()->GetHeight()),
			-1.0f,
			1.0f,
			-1.0f,
			1.0f
		),
		m_cameraPosition(0.0f),
		m_cameraRotation(0.0f)
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
		TRAP::VFS::Get()->MountShaders("Assets/Shaders");
		TRAP::Graphics::ShaderManager::Add(TRAP::Graphics::Shader::CreateFromFile("Color", "/Shaders/Color.shader"));
		TRAP::Graphics::ShaderManager::Add(TRAP::Graphics::Shader::CreateFromFile("Texture", "/Shaders/Texture.shader"));

		//EXPERIMENTAL
		TRAP::VFS::Get()->MountTextures("Assets/Textures");
		TRAP::Graphics::TextureManager::Add(TRAP::Graphics::Texture2D::CreateFromFile("Debug", "/Textures/Debug3.tga"));
		//////////////

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
		std::unique_ptr<TRAP::Graphics::VertexBuffer> vertexBuffer = TRAP::Graphics::VertexBuffer::Create(vertices.data(), static_cast<uint32_t>(vertices.size()));
		const TRAP::Graphics::BufferLayout layout =
		{
			{TRAP::Graphics::ShaderDataType::Float3, "Position"},
			{TRAP::Graphics::ShaderDataType::Float4, "Color"},
			{TRAP::Graphics::ShaderDataType::Float2, "UV"}
		};
		vertexBuffer->SetLayout(layout);
		m_vertexArray->AddVertexBuffer(vertexBuffer);

		std::array<uint32_t, 6> indices //Quad
		{
			0, 1, 2, 2, 3, 0
		};
		std::unique_ptr<TRAP::Graphics::IndexBuffer> indexBuffer = TRAP::Graphics::IndexBuffer::Create(indices.data(), static_cast<uint32_t>(indices.size()));
		m_vertexArray->SetIndexBuffer(indexBuffer);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnDetach() override
	{
		m_vertexArray->Unbind();
		m_vertexArray.reset();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnUpdate(const TRAP::Utils::TimeStep deltaTime) override
	{
		m_camera.SetPosition(m_cameraPosition);
		m_camera.SetRotation(m_cameraRotation);

		TRAP::Graphics::RenderCommand::SetClearColor();
		TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::RENDERER_BUFFER_COLOR | TRAP::Graphics::RendererBufferType::RENDERER_BUFFER_DEPTH);
		TRAP::Graphics::RenderCommand::SetCull(false); //Disables Culling

		TRAP::Graphics::Renderer::BeginScene(m_camera);
		{
			if (m_show)
			{
				/*static TRAP::Math::Mat4 scale = TRAP::Math::Mat4::Scale(TRAP::Math::Vec3(0.1f));

				for (int y = 0; y < 10; y++)
				{
					for (int x = 0; x < 10; x++)
					{
						TRAP::Math::Vec3 position(static_cast<float>(x) * 0.11f, static_cast<float>(y) * 0.11f, 0.0f);
						TRAP::Math::Mat4 transform = TRAP::Math::Mat4::Translate(position) * scale;

						if (m_usePassthrough)
							TRAP::Graphics::Renderer::Submit(TRAP::Graphics::ShaderManager::Get("Passthrough"), m_vertexArray, transform);
						else
							TRAP::Graphics::Renderer::Submit(TRAP::Graphics::ShaderManager::Get("Color"), m_vertexArray, transform);
					}
				}*/

				if (m_usePassthrough)
					TRAP::Graphics::Renderer::Submit(TRAP::Graphics::ShaderManager::Get("Passthrough"), m_vertexArray);
				else
				{
					TRAP::Graphics::TextureManager::Get("Debug")->Bind();					
					TRAP::Graphics::Renderer::Submit(TRAP::Graphics::ShaderManager::Get("Texture"), m_vertexArray);
				}
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
				m_frameTimeHistory[frameTimeIndex] = TRAP::Application::Get().GetFrameTime();
				frameTimeIndex++;
			}
			else
			{
				std::move(m_frameTimeHistory.begin() + 1, m_frameTimeHistory.end(), m_frameTimeHistory.begin());
				m_frameTimeHistory[m_frameTimeHistory.size() - 1] = TRAP::Application::Get().GetFrameTime();
			}
		}
		if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
		{
			TP_INFO("[Sandbox] DrawCall(s): ", TRAP::Graphics::Renderer::GetDrawCalls());
			TP_INFO("[Sandbox] FPS: ", TRAP::Graphics::Renderer::GetFPS());
			TP_INFO("[Sandbox] FrameTime: ", TRAP::Graphics::Renderer::GetFrameTime(), "ms");
			m_fpsTimer.Reset();
		}

		///////////////////
		//Camera Controls//
		///////////////////		
		if (TRAP::Input::IsKeyPressed(TP_KEY_A))
			m_cameraPosition.x -= m_cameraMovementSpeed * deltaTime;
		if (TRAP::Input::IsKeyPressed(TP_KEY_D))
			m_cameraPosition.x += m_cameraMovementSpeed * deltaTime;
		if (TRAP::Input::IsKeyPressed(TP_KEY_W))
			m_cameraPosition.y += m_cameraMovementSpeed * deltaTime;
		if (TRAP::Input::IsKeyPressed(TP_KEY_S))
			m_cameraPosition.y -= m_cameraMovementSpeed * deltaTime;

		if (TRAP::Input::IsKeyPressed(TP_KEY_KP_4))
			m_cameraRotation.z += m_cameraRotationSpeed * deltaTime;
		if (TRAP::Input::IsKeyPressed(TP_KEY_KP_6))
			m_cameraRotation.z -= m_cameraRotationSpeed * deltaTime;
		if (TRAP::Input::IsKeyPressed(TP_KEY_KP_8))
			m_cameraRotation.x += m_cameraRotationSpeed * deltaTime;
		if (TRAP::Input::IsKeyPressed(TP_KEY_KP_2))
			m_cameraRotation.x -= m_cameraRotationSpeed * deltaTime;
		if (TRAP::Input::IsKeyPressed(TP_KEY_KP_7))
			m_cameraRotation.y += m_cameraRotationSpeed * deltaTime;
		if (TRAP::Input::IsKeyPressed(TP_KEY_KP_9))
			m_cameraRotation.y -= m_cameraRotationSpeed * deltaTime;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool OnKeyPressed(TRAP::KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == TP_KEY_ESCAPE)
			TRAP::Application::Get().Shutdown();

		if (event.GetKeyCode() == TP_KEY_F1 && event.GetRepeatCount() < 1) //Switch to D3D12
			TRAP::Graphics::API::Context::SwitchRenderAPI(TRAP::Graphics::API::RenderAPI::D3D12);
		if (event.GetKeyCode() == TP_KEY_F2 && event.GetRepeatCount() < 1) //Switch to Vulkan
			TRAP::Graphics::API::Context::SwitchRenderAPI(TRAP::Graphics::API::RenderAPI::Vulkan);
		if (event.GetKeyCode() == TP_KEY_F3 && event.GetRepeatCount() < 1) //Switch to OpenGL
			TRAP::Graphics::API::Context::SwitchRenderAPI(TRAP::Graphics::API::RenderAPI::OpenGL);

		if (event.GetKeyCode() == TP_KEY_F4 && event.GetRepeatCount() < 1) //Use Default/Passthrough Shader
			m_usePassthrough = !m_usePassthrough;

		if (event.GetKeyCode() == TP_KEY_F5 && event.GetRepeatCount() < 1) //Make Window windowed
			TRAP::Application::Get().GetWindow()->SetWindowMode(TRAP::DisplayMode::Windowed);
		if (event.GetKeyCode() == TP_KEY_F6 && event.GetRepeatCount() < 1) //Make Window Borderless Fullscreen
			TRAP::Application::Get().GetWindow()->SetWindowMode(TRAP::DisplayMode::Borderless);
		if (event.GetKeyCode() == TP_KEY_F7 && event.GetRepeatCount() < 1) //Make Window Exclusive Fullscreen
			TRAP::Application::Get().GetWindow()->SetWindowMode(TRAP::DisplayMode::Fullscreen);

		if (event.GetKeyCode() == TP_KEY_F9 && event.GetRepeatCount() < 1) //Enable/Disable
			m_show = !m_show;
		if (event.GetKeyCode() == TP_KEY_F10 && event.GetRepeatCount() < 1) //Enable/Disable WireFrame Mode
		{
			m_wireFrame = !m_wireFrame;
			TRAP::Graphics::RenderCommand::SetWireFrame(m_wireFrame);
		}

		if (event.GetKeyCode() == TP_KEY_F11 && event.GetRepeatCount() < 1)
			TRAP::Utils::MsgBox::Show("Just a prank bro!", "Critical Error");

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnEvent(TRAP::Event& event) override
	{
		TRAP::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<TRAP::KeyPressedEvent>(TP_BIND_EVENT_FN(SandboxLayer::OnKeyPressed));

		dispatcher.Dispatch<TRAP::WindowResizeEvent>(TP_BIND_EVENT_FN(SandboxLayer::OnResize));
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool OnResize(TRAP::WindowResizeEvent& event)
	{
		TRAP::Graphics::RenderCommand::SetViewport(0, 0, event.GetWidth(), event.GetHeight());
		const float aspectRatio = static_cast<float>(event.GetWidth()) / static_cast<float>(event.GetHeight());
		m_camera = TRAP::Graphics::OrthographicCamera
		{
			-aspectRatio,
			aspectRatio,
			-1.0f,
			1.0f,
			-1.0f,
			1.0f
		};

		return true;
	}

private:
	std::array<float, 50> m_frameTimeHistory;
	TRAP::Utils::Timer m_fpsTimer;
	TRAP::Utils::Timer m_titleTimer;
	bool m_usePassthrough;
	bool m_wireFrame;
	bool m_show;

	std::unique_ptr<TRAP::Graphics::VertexArray> m_vertexArray;

	TRAP::Graphics::OrthographicCamera m_camera;
	TRAP::Math::Vec3 m_cameraPosition;
	TRAP::Math::Vec3 m_cameraRotation;
	float m_cameraMovementSpeed = 2.5f;
	float m_cameraRotationSpeed = 180.0f;
};