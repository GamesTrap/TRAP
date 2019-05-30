#pragma once

#include <TRAP.h>

#include "imgui.h"

class SandboxLayer : public TRAP::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox"), m_vertexArray(0), m_vertexBuffer(0), m_indexBuffer(0)
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

	void OnUpdate(float deltaTime) override
	{
		if (TRAP::Graphics::API::Context::GetRenderAPI() == TRAP::Graphics::API::RenderAPI::OPENGL)
		{
			OpenGLCall(glBindVertexArray(m_vertexArray));
			TRAP::Graphics::ShaderManager::Get("Debug")->Bind();
			OpenGLCall(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr));
		}

		//FPS & FrameTime
		if(timer.Elapsed() >= 10.0f) //Output Every Second
		{
			TP_INFO("[Sandbox] FPS: ", TRAP::Application::Get().GetFPS());
			TP_INFO("[Sandbox] FrameTime: ", TRAP::Application::Get().GetFrameTime(), "ms");
			timer.Reset();
		}
	}
	
	bool OnKeyPressed(TRAP::KeyPressedEvent& event) const
	{
		if (event.GetKeyCode() == TP_KEY_ESCAPE)
			TRAP::Application::Get().Shutdown();

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
};

