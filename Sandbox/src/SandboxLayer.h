#pragma once

#include <TRAP.h>

#include "imgui.h"

class SandboxLayer : public TRAP::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox"), m_usePassthrough(false)
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
		TRAP::Graphics::ShaderManager::Add(TRAP::Graphics::API::Shader::CreateFromFile("Debug", "/Shaders/Debug.vert", "/Shaders/Debug.frag", "", "", "", ""));

		std::array<float, 3 * 3> vertices
		{
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		std::array<uint32_t, 3> indices
		{
			0, 1, 2
		};

		m_vertexArray = TRAP::Graphics::API::VertexArray::Create();
		m_vertexBuffer = TRAP::Graphics::API::VertexBuffer::Create(vertices.data(), static_cast<uint32_t>(vertices.size()), TRAP::Graphics::API::BufferUsage::STATIC);
		{
			const TRAP::Graphics::API::BufferLayout layout =
			{
				{TRAP::Graphics::API::ShaderDataType::Float3, "Position"}
			};

			m_vertexBuffer->SetLayout(layout);
		}
		m_vertexArray->AddBuffer(m_vertexBuffer);
		m_indexBuffer = TRAP::Graphics::API::IndexBuffer::Create(indices.data(), static_cast<uint32_t>(indices.size()), TRAP::Graphics::API::BufferUsage::STATIC);
	}

	void OnDetach() override
	{
		m_indexBuffer->Unbind();
		m_vertexBuffer->Unbind();
		m_vertexArray->Unbind();

		m_indexBuffer.reset();
		m_vertexBuffer.reset();
		m_vertexArray.reset();
	}

	void OnUpdate(TRAP::Utils::TimeStep deltaTime) override
	{
		if (m_usePassthrough)
			TRAP::Graphics::ShaderManager::Get("Passthrough")->Bind();
		else
			TRAP::Graphics::ShaderManager::Get("Debug")->Bind();
		m_vertexArray->Draw(m_indexBuffer->GetCount());

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

	std::unique_ptr<TRAP::Graphics::API::VertexBuffer> m_vertexBuffer;
	std::unique_ptr<TRAP::Graphics::API::VertexArray> m_vertexArray;
	std::unique_ptr<TRAP::Graphics::API::IndexBuffer> m_indexBuffer;
};