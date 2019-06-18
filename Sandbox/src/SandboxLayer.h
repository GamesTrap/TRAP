#pragma once

#include <TRAP.h>

#include "imgui.h"

class SandboxLayer : public TRAP::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox"), m_usePassthrough(false), m_wireFrame(false)
	{
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Hello World");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	void OnAttach() override
	{
		TRAP::VFS::Get()->MountShaders("Assets/Shaders");
		TRAP::Graphics::ShaderManager::Add(TRAP::Graphics::API::Shader::CreateFromFile("Color", "/Shaders/Color.vert", "/Shaders/Color.frag", "", "", "", ""));
		TRAP::Graphics::ShaderManager::Add(TRAP::Graphics::API::Shader::CreateFromFile("Texture", "/Shaders/Texture.vert", "/Shaders/Texture.frag", "", "", "", ""));

		TRAP::VFS::Get()->MountTextures("Assets/Textures");
		TRAP::Graphics::TextureManager::Add(TRAP::Graphics::API::Texture2D::CreateFromFile("Test", "/Textures/Test.bmp"));

		//////////////
		// Triangle //
		//////////////
		m_triangleVertexArray = TRAP::Graphics::API::VertexArray::Create();

		//XYZ RGBA
		std::array<float, 3 * 7> triangleVertices
		{
			-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f, 1.0f,
			 0.0f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f, 1.0f
		};
		std::unique_ptr<TRAP::Graphics::API::VertexBuffer> triangleVertexBuffer = TRAP::Graphics::API::VertexBuffer::Create(triangleVertices.data(), static_cast<uint32_t>(triangleVertices.size()), TRAP::Graphics::API::BufferUsage::STATIC);
		const TRAP::Graphics::API::BufferLayout triangleLayout =
		{
			{TRAP::Graphics::API::ShaderDataType::Float3, "Position"},
			{TRAP::Graphics::API::ShaderDataType::Float4, "Color"}
		};
		triangleVertexBuffer->SetLayout(triangleLayout);
		m_triangleVertexArray->AddVertexBuffer(triangleVertexBuffer);

		std::array<uint32_t, 3> triangleIndices
		{
			0, 1, 2
		};
		std::unique_ptr<TRAP::Graphics::API::IndexBuffer> triangleIndexBuffer = TRAP::Graphics::API::IndexBuffer::Create(triangleIndices.data(), static_cast<uint32_t>(triangleIndices.size()), TRAP::Graphics::API::BufferUsage::STATIC);
		m_triangleVertexArray->SetIndexBuffer(triangleIndexBuffer);

		/////////////////////
		// Textured Square //
		/////////////////////
		m_squareVertexArray = TRAP::Graphics::API::VertexArray::Create();

		//XYZ UV RGBA
		std::array<float, 4 * 5> squareVertices
		{
			-0.75f, -0.75f, 0.0f,    0.0f, 0.0f,
			 0.75f, -0.75f, 0.0f,    1.0f, 0.0f,
			 0.75f,  0.75f, 0.0f,    1.0f, 1.0f,
			-0.75f,  0.75f, 0.0f,    0.0f, 1.0f
		};
		std::unique_ptr<TRAP::Graphics::API::VertexBuffer> squareVertexBuffer = TRAP::Graphics::API::VertexBuffer::Create(squareVertices.data(), static_cast<uint32_t>(squareVertices.size()), TRAP::Graphics::API::BufferUsage::STATIC);
		const TRAP::Graphics::API::BufferLayout squareLayout =
		{
			{TRAP::Graphics::API::ShaderDataType::Float3, "Position"},
			{TRAP::Graphics::API::ShaderDataType::Float2, "TexCoord"}
		};
		squareVertexBuffer->SetLayout(squareLayout);
		m_squareVertexArray->AddVertexBuffer(squareVertexBuffer);
		std::array<uint32_t, 6> squareIndices
		{
			0, 1, 2, 2, 3, 0
		};
		std::unique_ptr<TRAP::Graphics::API::IndexBuffer> squareIndexBuffer = TRAP::Graphics::API::IndexBuffer::Create(squareIndices.data(), static_cast<uint32_t>(squareIndices.size()), TRAP::Graphics::API::BufferUsage::STATIC);
		m_squareVertexArray->SetIndexBuffer(squareIndexBuffer);
	}

	void OnDetach() override
	{
		m_triangleVertexArray->Unbind();
		m_squareVertexArray->Unbind();

		m_triangleVertexArray.reset();
		m_squareVertexArray.reset();
	}

	void OnUpdate(TRAP::Utils::TimeStep deltaTime) override
	{
		if (m_usePassthrough)
			TRAP::Graphics::ShaderManager::Get("Passthrough")->Bind();
		else
			TRAP::Graphics::ShaderManager::Get("Color")->Bind();

		m_triangleVertexArray->Bind();
		m_triangleVertexArray->Draw();

		TRAP::Graphics::TextureManager::Get("Test")->Bind();
		TRAP::Graphics::ShaderManager::Get("Texture")->Bind();
		m_squareVertexArray->Bind();
		m_squareVertexArray->Draw();

		//FPS & FrameTime
		if (timer.Elapsed() >= 5.0f) //Output Every 5 Seconds
		{
			TP_INFO("[Sandbox] FPS: ", TRAP::Application::Get().GetFPS());
			TP_INFO("[Sandbox] FrameTime: ", TRAP::Application::Get().GetFrameTime(), "ms");
			timer.Reset();
		}
	}

	bool OnKeyPressed(TRAP::KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == TP_KEY_ESCAPE)
			TRAP::Application::Get().Shutdown();

		if (event.GetKeyCode() == TP_KEY_F1 && event.GetRepeatCount() < 1) //Switch to D3D12
			TRAP::Graphics::API::Context::SwitchRenderAPI(TRAP::Graphics::API::RenderAPI::D3D12);
		if (event.GetKeyCode() == TP_KEY_F2 && event.GetRepeatCount() < 1) //Switch to Vulkan
			TRAP::Graphics::API::Context::SwitchRenderAPI(TRAP::Graphics::API::RenderAPI::VULKAN);
		if (event.GetKeyCode() == TP_KEY_F3 && event.GetRepeatCount() < 1) //Switch to OpenGL
			TRAP::Graphics::API::Context::SwitchRenderAPI(TRAP::Graphics::API::RenderAPI::OPENGL);

		if (event.GetKeyCode() == TP_KEY_F4 && event.GetRepeatCount() < 1) //Use Default/Passthrough Shader
			m_usePassthrough = !m_usePassthrough;

		if (event.GetKeyCode() == TP_KEY_F5 && event.GetRepeatCount() < 1) //Make Window windowed
			TRAP::Application::Get().GetWindow()->SetWindowMode(TRAP::DisplayMode::WINDOWED);
		if (event.GetKeyCode() == TP_KEY_F6 && event.GetRepeatCount() < 1) //Make Window Borderless Fullscreen
			TRAP::Application::Get().GetWindow()->SetWindowMode(TRAP::DisplayMode::BORDERLESS);
		if (event.GetKeyCode() == TP_KEY_F7 && event.GetRepeatCount() < 1) //Make Window Exclusive Fullscreen
			TRAP::Application::Get().GetWindow()->SetWindowMode(TRAP::DisplayMode::FULLSCREEN);

		if (event.GetKeyCode() == TP_KEY_F10 && event.GetRepeatCount() < 1) //WireFrame Mode
		{
			m_wireFrame = !m_wireFrame;
			TRAP::Graphics::API::Renderer::SetWireFrame(m_wireFrame);
		}

		if (event.GetKeyCode() == TP_KEY_F11 && event.GetRepeatCount() < 1)
			TRAP::Utils::MsgBox::Show("Testing MsgBox System", "Test MsgBox");

		return true;
	}

	void OnEvent(TRAP::Event& event) override
	{
		TRAP::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<TRAP::KeyPressedEvent>(TP_BIND_EVENT_FN(SandboxLayer::OnKeyPressed));
	}

private:
	TRAP::Utils::Timer timer;
	bool m_usePassthrough;
	bool m_wireFrame;

	std::unique_ptr<TRAP::Graphics::API::VertexArray> m_triangleVertexArray;
	std::unique_ptr<TRAP::Graphics::API::VertexArray> m_squareVertexArray;
};