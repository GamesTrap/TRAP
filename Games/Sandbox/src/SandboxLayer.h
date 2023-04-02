#ifndef GAMESTRAP_SANDBOXLAYER_H
#define GAMESTRAP_SANDBOXLAYER_H

#include <TRAP.h>

class SandboxLayer final : public TRAP::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox"),
		m_frameTimeHistory(),
		m_cameraController(TRAP::Application::GetWindow()->GetAspectRatio())
	{
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnImGuiRender() override
	{
		ImGui::SetNextWindowBgAlpha(0.3f);
		ImGui::Begin("Performance", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
		                                     ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("CPU: %ix %s", TRAP::Utils::GetCPUInfo().LogicalCores, TRAP::Utils::GetCPUInfo().Model.c_str());
		ImGui::Text("GPU: %s", TRAP::Graphics::RenderCommand::GetGPUName().c_str());
		ImGui::Text("CPU FPS: %u", TRAP::Graphics::RenderCommand::GetCPUFPS());
		ImGui::Text("GPU FPS: %u", TRAP::Graphics::RenderCommand::GetGPUFPS());
		ImGui::Text("CPU FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetCPUFrameTime());
		ImGui::Text("GPU Graphics FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUGraphicsFrameTime());
		ImGui::Text("GPU Compute FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUComputeFrameTime());
		ImGui::PlotLines("", m_frameTimeHistory.data(), static_cast<int>(m_frameTimeHistory.size()), 0, nullptr, 0,
		                 33, ImVec2(200, 50));
		ImGui::End();

		ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
		                                    ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Checkbox("WireFrame (F1)", &m_wireFrame);
		ImGui::Checkbox("Indexed Drawing (F2)", &m_indexedDrawing);
		ImGui::End();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnAttach() override
	{
		TRAP::Application::SetHotReloading(true);

		TRAP::Application::GetWindow()->SetTitle("Sandbox");

		//Load Shader
		m_shader = TRAP::Graphics::ShaderManager::LoadFile("./Assets/Shaders/TextureColor.shader");

		//Load Texture
		m_texture = TRAP::Graphics::TextureManager::Load("./Assets/Textures/TRAPWhiteLogo2048x2048.png");

		//Indexed
		///////////////
		//    Quad   //
		///////////////
		//XYZ RGBA
		constexpr std::array<float, 9ull * 4> indexedVertices //Quad
		{
			-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f,
			 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f, 1.0f,    1.0f, 0.0f,
			-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f, 1.0f,    0.0f, 0.0f
		};
		m_indexedVertexBuffer = TRAP::Graphics::VertexBuffer::Create(indexedVertices.data(),
																	 static_cast<uint32_t>(indexedVertices.size()) *
																	 sizeof(uint32_t),
																	 TRAP::Graphics::UpdateFrequency::Static);
		m_indexedVertexBuffer->AwaitLoading();

		const TRAP::Graphics::VertexBufferLayout layout =
		{
			{TRAP::Graphics::ShaderDataType::Float3, "Position"},
			{TRAP::Graphics::ShaderDataType::Float4, "Color"},
			{TRAP::Graphics::ShaderDataType::Float2, "UV"}
		};
		m_indexedVertexBuffer->SetLayout(layout);

		constexpr std::array<uint16_t, 6> indices //Quad
		{
			0, 1, 2, 2, 3, 0
		};
		m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(indices.data(),
		                                                    static_cast<uint32_t>(indices.size()) *
															sizeof(uint16_t),
															TRAP::Graphics::UpdateFrequency::Static);
		m_indexBuffer->AwaitLoading();

		//Non-Indexed
		///////////////
		//    Quad   //
		///////////////
		//XYZ RGBA
		constexpr std::array<float, 9ull * 6> vertices //Quad
		{
			-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f,
			 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f, 1.0f,    1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f, 1.0f,    1.0f, 0.0f,
			-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f, 1.0f,    0.0f, 0.0f,
			-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f
		};
		m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(vertices.data(),
		 													  static_cast<uint32_t>(vertices.size()) *
															  sizeof(uint32_t),
															  TRAP::Graphics::UpdateFrequency::Static);
		m_vertexBuffer->AwaitLoading();
		m_vertexBuffer->SetLayout(layout);

		TRAP::Graphics::RendererAPI::SamplerDesc samplerDesc{};
		samplerDesc.AddressU = TRAP::Graphics::AddressMode::Repeat;
		samplerDesc.AddressV = TRAP::Graphics::AddressMode::Repeat;
		samplerDesc.AddressW = TRAP::Graphics::AddressMode::Repeat;
		samplerDesc.EnableAnisotropy = false;
		m_sampler = TRAP::Graphics::Sampler::Create(samplerDesc);

		//Wait for all pending resources (just in case)
		TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

		//Use Shader & Resources
		m_indexedVertexBuffer->Use();
		m_indexBuffer->Use();
		m_shader->Use();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnDetach() override
	{
		m_sampler.reset();
		m_indexBuffer.reset();
		m_indexedVertexBuffer.reset();
		m_vertexBuffer.reset();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override
	{
		//Update
		m_cameraController.OnUpdate(deltaTime);

		TRAP::Graphics::RenderCommand::SetBlendMode(TRAP::Graphics::BlendMode::Add, TRAP::Graphics::BlendMode::Add);
		TRAP::Graphics::RenderCommand::SetBlendConstant(TRAP::Graphics::BlendConstant::SrcAlpha,
														TRAP::Graphics::BlendConstant::One,
														TRAP::Graphics::BlendConstant::OneMinusSrcAlpha,
														TRAP::Graphics::BlendConstant::OneMinusSrcAlpha);

		//Render
		if(m_wireFrame)
			TRAP::Graphics::RenderCommand::SetFillMode(TRAP::Graphics::FillMode::Line);
		else
			TRAP::Graphics::RenderCommand::SetFillMode(TRAP::Graphics::FillMode::Solid);

		TRAP::Graphics::Renderer::BeginScene(m_cameraController.GetCamera());
		{
			float time = TRAP::Application::GetTime();
			m_shader->UseTexture(0, 0, m_texture);
			m_shader->UseSampler(0, 1, m_sampler.get());
			if (m_indexedDrawing)
			{
				TRAP::Graphics::RenderCommand::SetPushConstants("TimeRootConstant", &time);
				TRAP::Graphics::Renderer::Submit(m_shader, m_indexedVertexBuffer.get(), m_indexBuffer.get());
			}
			else
			{
				TRAP::Graphics::RenderCommand::SetPushConstants("TimeRootConstant", &time);
				TRAP::Graphics::Renderer::Submit(m_shader, m_vertexBuffer.get());
			}
		}
		TRAP::Graphics::Renderer::EndScene();

		//Update FPS & FrameTime history
		if (m_titleTimer.Elapsed() >= 0.025f)
		{
			m_titleTimer.Reset();
			static int frameTimeIndex = 0;
			if (frameTimeIndex < static_cast<int>(m_frameTimeHistory.size() - 1))
			{
				m_frameTimeHistory[frameTimeIndex] = TRAP::Graphics::RenderCommand::GetCPUFrameTime();
				frameTimeIndex++;
			}
			else
			{
				std::move(m_frameTimeHistory.begin() + 1, m_frameTimeHistory.end(), m_frameTimeHistory.begin());
				m_frameTimeHistory[m_frameTimeHistory.size() - 1] = TRAP::Graphics::RenderCommand::GetCPUFrameTime();
			}
		}

		//Simple performance metrics
		if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
		{
			//TP_INFO("[Sandbox] DrawCall(s): ", TRAP::Graphics::Renderer::GetDrawCalls());
			TP_INFO("[Sandbox] FPS: ", TRAP::Graphics::RenderCommand::GetCPUFPS());
			TP_INFO("[Sandbox] CPU FrameTime: ", TRAP::Graphics::RenderCommand::GetCPUFrameTime(), "ms");
			m_fpsTimer.Reset();
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool OnKeyPress(TRAP::Events::KeyPressEvent& event)
	{
		if (event.GetKey() == TRAP::Input::Key::Escape)
			TRAP::Application::Shutdown();

		if(event.GetRepeatCount() != 0)
			return false;

		if (event.GetKey() == TRAP::Input::Key::F5) //Make Window windowed
			TRAP::Application::GetWindow()->SetDisplayMode(TRAP::Window::DisplayMode::Windowed, 1280, 720);
		if (event.GetKey() == TRAP::Input::Key::F6) //Make Window Borderless Fullscreen
			TRAP::Application::GetWindow()->SetDisplayMode(TRAP::Window::DisplayMode::Borderless);
		if (event.GetKey() == TRAP::Input::Key::F7) //Make Window Exclusive Fullscreen
			TRAP::Application::GetWindow()->SetDisplayMode(TRAP::Window::DisplayMode::Fullscreen);

		if (event.GetKey() == TRAP::Input::Key::F1) //Enable/Disable WireFrame Mode
			m_wireFrame = !m_wireFrame;
		if (event.GetKey() == TRAP::Input::Key::F2) //Enable/Disable Indexed drawing
			m_indexedDrawing = !m_indexedDrawing;

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool OnTextureReload(TRAP::Events::TextureReloadEvent& event)
	{
		m_texture = event.GetTexture();
		m_shader->UseTexture(0, 0, m_texture);
		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool OnShaderReload(TRAP::Events::ShaderReloadEvent& event)
	{
		m_shader = event.GetShader();
		m_shader->UseTexture(0, 0, m_texture);
		m_shader->UseSampler(0, 1, m_sampler.get());
		m_shader->Use();
		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnEvent(TRAP::Events::Event& event) override
	{
		m_cameraController.OnEvent(event);

		TRAP::Events::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e)
		{
			return OnKeyPress(e);
		});
		dispatcher.Dispatch<TRAP::Events::TextureReloadEvent>([this](TRAP::Events::TextureReloadEvent& e)
		{
			return OnTextureReload(e);
		});
		dispatcher.Dispatch<TRAP::Events::ShaderReloadEvent>([this](TRAP::Events::ShaderReloadEvent& e)
		{
			return OnShaderReload(e);
		});
	}

private:
	std::array<float, 50> m_frameTimeHistory;
	TRAP::Utils::Timer m_fpsTimer;
	TRAP::Utils::Timer m_titleTimer;
	bool m_wireFrame = false;
	bool m_indexedDrawing = false;

	TRAP::Graphics::OrthographicCameraController m_cameraController;

	TRAP::Scope<TRAP::Graphics::VertexBuffer> m_indexedVertexBuffer{};
	TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer{};
	TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer{};
	TRAP::Ref<TRAP::Graphics::Sampler> m_sampler{};
	TRAP::Ref<TRAP::Graphics::Texture> m_texture{};
	TRAP::Ref<TRAP::Graphics::Shader> m_shader{};
};

#endif /*GAMESTRAP_SANDBOXLAYER_H*/