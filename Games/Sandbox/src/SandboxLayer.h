#ifndef GAMESTRAP_SANDBOXLAYER_H
#define GAMESTRAP_SANDBOXLAYER_H

#include <TRAP.h>

class SandboxLayer final : public TRAP::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox")
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
		ImGui::PlotLines("##frametimeHistory", m_frameTimeHistory.data(), NumericCast<i32>(m_frameTimeHistory.size()),
		                 0, nullptr, 0.0f, 33.0f, ImVec2(200.0f, 50.0f));
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
		m_shader = TRAP::Graphics::ShaderManager::LoadFile("./Assets/Shaders/TextureColor.shader", TRAP::Graphics::ShaderType::Graphics);

		//Load Texture
		m_texture = TRAP::Graphics::TextureManager::Load("./Assets/Textures/TRAPWhiteLogo2048x2048.png");

		//Indexed
		///////////////
		//    Quad   //
		///////////////
		//XYZ RGBA
		static constexpr std::array<f32, 9ull * 4u> indexedVertices //Quad
		{
			-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f,
			 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f, 1.0f,    1.0f, 0.0f,
			-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f, 1.0f,    0.0f, 0.0f
		};
		m_indexedVertexBuffer = TRAP::Graphics::VertexBuffer::Create(indexedVertices,
																	 TRAP::Graphics::UpdateFrequency::Static);
		m_indexedVertexBuffer->AwaitLoading();

		const TRAP::Graphics::VertexBufferLayout layout =
		{
			{TRAP::Graphics::ShaderDataType::Float3, "Position"},
			{TRAP::Graphics::ShaderDataType::Float4, "Color"},
			{TRAP::Graphics::ShaderDataType::Float2, "UV"}
		};
		m_indexedVertexBuffer->SetLayout(layout);

		static constexpr std::array<u16, 6u> indices //Quad
		{
			0u, 1u, 2u, 2u, 3u, 0u
		};
		m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(indices, TRAP::Graphics::UpdateFrequency::Static);
		m_indexBuffer->AwaitLoading();

		//Non-Indexed
		///////////////
		//    Quad   //
		///////////////
		//XYZ RGBA
		static constexpr std::array<f32, 9ull * 6u> vertices //Quad
		{
			-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f,
			 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f, 1.0f,    1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f, 1.0f,    1.0f, 0.0f,
			-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f, 1.0f,    0.0f, 0.0f,
			-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f
		};
		m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(vertices,
															  TRAP::Graphics::UpdateFrequency::Static);
		m_vertexBuffer->AwaitLoading();
		m_vertexBuffer->SetLayout(layout);

		const TRAP::Graphics::RendererAPI::SamplerDesc samplerDesc
		{
			.AddressU = TRAP::Graphics::AddressMode::Repeat,
			.AddressV = TRAP::Graphics::AddressMode::Repeat,
			.AddressW = TRAP::Graphics::AddressMode::Repeat,
			.EnableAnisotropy = false
		};
		m_sampler = TRAP::Graphics::Sampler::Create(samplerDesc);

		//Wait for all pending resources (just in case)
		TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

		//Use Shader & Resources
		m_indexedVertexBuffer->Use();
		m_indexBuffer->Use();
		m_shader->Use();
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
			const f32 time = TRAP::Application::GetTime();
			m_shader->UseTexture(0u, 0u, *m_texture);
			m_shader->UseSampler(0u, 1u, *m_sampler);
			if (m_indexedDrawing)
			{
				TRAP::Graphics::RenderCommand::SetPushConstants("TimeRootConstant", time);
				TRAP::Graphics::Renderer::Submit(*m_shader, *m_indexedVertexBuffer, *m_indexBuffer);
			}
			else
			{
				TRAP::Graphics::RenderCommand::SetPushConstants("TimeRootConstant", time);
				TRAP::Graphics::Renderer::Submit(*m_shader, *m_vertexBuffer);
			}
		}
		TRAP::Graphics::Renderer::EndScene();

		//Update FPS & FrameTime history
		if (m_titleTimer.Elapsed() >= 0.025f)
		{
			m_titleTimer.Reset();
			constinit static usize frameTimeIndex = 0u;
			if (frameTimeIndex < m_frameTimeHistory.size() - 1u)
			{
				m_frameTimeHistory[frameTimeIndex] = TRAP::Graphics::RenderCommand::GetCPUFrameTime();
				++frameTimeIndex;
			}
			else
			{
				std::shift_left(m_frameTimeHistory.begin(), m_frameTimeHistory.end(), 1);
				m_frameTimeHistory.back() = TRAP::Graphics::RenderCommand::GetCPUFrameTime();
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

	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event)
	{
		if (event.GetKey() == TRAP::Input::Key::Escape)
			TRAP::Application::Shutdown();

		if (event.GetKey() == TRAP::Input::Key::F5) //Make Window windowed
			TRAP::Application::GetWindow()->SetWindowed();
		if (event.GetKey() == TRAP::Input::Key::F6) //Make Window Borderless Fullscreen
			TRAP::Application::GetWindow()->SetFullscreenBorderless();
		if (event.GetKey() == TRAP::Input::Key::F7) //Make Window Exclusive Fullscreen
			TRAP::Application::GetWindow()->SetFullscreen();

		if (event.GetKey() == TRAP::Input::Key::F1) //Enable/Disable WireFrame Mode
			m_wireFrame = !m_wireFrame;
		if (event.GetKey() == TRAP::Input::Key::F2) //Enable/Disable Indexed drawing
			m_indexedDrawing = !m_indexedDrawing;

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool OnTextureReload(const TRAP::Events::TextureReloadEvent& event)
	{
		m_texture = event.GetTexture();
		m_shader->UseTexture(0u, 0u, *m_texture);
		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	bool OnShaderReload(const TRAP::Events::ShaderReloadEvent& event)
	{
		m_shader = event.GetShader();
		m_shader->UseTexture(0u, 0u, *m_texture);
		m_shader->UseSampler(0u, 1u, *m_sampler);
		m_shader->Use();
		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void OnEvent(TRAP::Events::Event& event) override
	{
		m_cameraController.OnEvent(event);

		TRAP::Events::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&SandboxLayer::OnKeyPress, this));
		dispatcher.Dispatch<TRAP::Events::TextureReloadEvent>(std::bind_front(&SandboxLayer::OnTextureReload, this));
		dispatcher.Dispatch<TRAP::Events::ShaderReloadEvent>(std::bind_front(&SandboxLayer::OnShaderReload, this));
	}

private:
	std::array<f32, 50u> m_frameTimeHistory{};
	TRAP::Utils::Timer m_fpsTimer{};
	TRAP::Utils::Timer m_titleTimer{};
	bool m_wireFrame = false;
	bool m_indexedDrawing = false;

	TRAP::Graphics::OrthographicCameraController m_cameraController{TRAP::Application::GetWindow()->GetAspectRatio(), false};

	TRAP::Scope<TRAP::Graphics::VertexBuffer> m_indexedVertexBuffer{};
	TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer{};
	TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer{};
	TRAP::Ref<TRAP::Graphics::Sampler> m_sampler{};
	TRAP::Ref<TRAP::Graphics::Texture> m_texture{};
	TRAP::Ref<TRAP::Graphics::Shader> m_shader{};
};

#endif /*GAMESTRAP_SANDBOXLAYER_H*/
