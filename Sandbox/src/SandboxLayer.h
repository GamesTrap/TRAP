#pragma once

#include <TRAP.h>

#include "imgui.h"

class SandboxLayer : public TRAP::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox"), m_vertexArray(0), m_vertexBuffer(0), m_indexBuffer(0), m_usePassthrough(false)
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

		//Triangle for OpenGL to test Shader System
		if (TRAP::Graphics::API::Context::GetRenderAPI() == TRAP::Graphics::API::RenderAPI::OPENGL)
		{
			OpenGLCall(glGenVertexArrays(1, &m_vertexArray));
			OpenGLCall(glBindVertexArray(m_vertexArray));

			OpenGLCall(glGenBuffers(1, &m_vertexBuffer));
			OpenGLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer));

			std::array<float, 3 * 3> vertices
			{
				-0.5f, -0.5f, 0.0f,
				 0.5f, -0.5f, 0.0f,
				 0.0f,  0.5f, 0.0f
			};

			OpenGLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW));

			OpenGLCall(glEnableVertexAttribArray(0));
			OpenGLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr));

			OpenGLCall(glGenBuffers(1, &m_indexBuffer));
			OpenGLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer));

			std::array<unsigned int, 3> indices
			{
				0, 1, 2
			};

			OpenGLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW));
		}
	}

	void OnUpdate(TRAP::Utils::TimeStep deltaTime) override
	{
		if (TRAP::Graphics::API::Context::GetRenderAPI() == TRAP::Graphics::API::RenderAPI::OPENGL)
		{
			OpenGLCall(glBindVertexArray(m_vertexArray));
			if (m_usePassthrough)
				TRAP::Graphics::ShaderManager::Get("Passthrough")->Bind();
			else
				TRAP::Graphics::ShaderManager::Get("Debug")->Bind();
			OpenGLCall(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr));
		}

		//FPS & FrameTime
		if(timer.Elapsed() >= 5.0f) //Output Every 5 Seconds
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
	unsigned int m_vertexArray, m_vertexBuffer, m_indexBuffer;
	TRAP::Utils::Timer timer;
	bool m_usePassthrough;
};

