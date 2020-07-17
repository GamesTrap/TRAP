#include "Cube3D.h"

Cube3D::Cube3D()
    : Layer("Cube3D"),
      m_camera((static_cast<float>(TRAP::Application::GetWindow()->GetWidth()) / static_cast<float>(TRAP::Application::GetWindow()->GetHeight()))),
	  m_cubePosition(0.0f, 0.0f, -5.0f),
	  m_cubeRotation(0.0f, 0.0f, 0.0f),
	  m_cubeScale(1.0f, 1.0f, 1.0f),
	  m_frameTimeHistory(),
	  m_ignoreImGui(false),
      m_shaderNames{ "Base", "Color", "Texture"/*, "Diffuse Reflection"*/ },
      m_currentShader(0),
	  m_wireFrame(false)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void Cube3D::OnAttach()
{
	TRAP::Application::SetHotShaderReloading(true);
	TRAP::Application::SetHotTextureReloading(true);
	TRAP::Application::GetWindow()->SetTitle("Cube3D");

	//Mount & Load Shader(s)
	TRAP::VFS::MountShaders("Assets/Shaders");
    TRAP::Graphics::ShaderManager::Load("Base", "/shaders/base.shader");
    TRAP::Graphics::ShaderManager::Load("Color", "/shaders/color.shader");
    TRAP::Graphics::ShaderManager::Load("Texture", "/shaders/texture.shader");

	//Mount & Load Texture(s)
	TRAP::VFS::MountTextures("Assets/Textures");
    TRAP::Graphics::TextureManager::Load("BeeHive", "/textures/sphaxbdcraftbee.png")->Bind(0);

	//Cube
	//XYZ
    std::array<float, 11 * 24> cubeVertices
    {
        //Positions             //Colors             //Textures      //Normals
        -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f,     0.0f,  0.0f, -1.0f,
         1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,     0.0f,  0.0f, -1.0f,
         1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f,     0.0f,  0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,     0.0f,  0.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,     0.0f,  0.0f,  1.0f,
         1.0f, -1.0f,  1.0f,    1.0f, 0.0f, 1.0f,    1.0f, 0.0f,     0.0f,  0.0f,  1.0f,
         1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     0.0f,  0.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f,     0.0f,  0.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,    -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 0.0f,    0.0f, 0.0f,    -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,    -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f,    -1.0f,  0.0f,  0.0f,

         1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f,     1.0f,  0.0f,  0.0f,
         1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f,     1.0f,  0.0f,  0.0f,
         1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f,     1.0f,  0.0f,  0.0f,
         1.0f, -1.0f,  1.0f,    1.0f, 0.0f, 1.0f,    0.0f, 0.0f,     1.0f,  0.0f,  0.0f,

        -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 0.0f,    0.0f, 0.0f,     0.0f, -1.0f,  0.0f,
         1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f,     0.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  1.0f,    1.0f, 0.0f, 1.0f,    1.0f, 1.0f,     0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,     0.0f, -1.0f,  0.0f,

         1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f,     0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,     0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f,     0.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f,     0.0f,  1.0f,  0.0f
    };
    std::array<uint32_t, 36> cubeIndices
    {
         0,  3,  2,
         2,  1,  0,
        
         4,  5,  6,
         6,  7,  4,
        
        11,  8,  9,
         9, 10, 11,
        
        12, 13, 14,
        14, 15, 12,
        
        16, 17, 18,
        18, 19, 16,
        
        20, 21, 22,
        22, 23, 20
    };
    TRAP::Scope<TRAP::Graphics::VertexBuffer> vertexBuffer = TRAP::Graphics::VertexBuffer::Create(cubeVertices.data(), static_cast<uint32_t>(cubeVertices.size()) * sizeof(uint32_t));
    TRAP::Scope<TRAP::Graphics::IndexBuffer> indexBuffer = TRAP::Graphics::IndexBuffer::Create(cubeIndices.data(), static_cast<uint32_t>(cubeIndices.size()));
    const TRAP::Graphics::BufferLayout layout =
    {
    	{TRAP::Graphics::ShaderDataType::Float3, "Position"},
    	{TRAP::Graphics::ShaderDataType::Float3, "Color"},
    	{TRAP::Graphics::ShaderDataType::Float2, "UV"},
    	{TRAP::Graphics::ShaderDataType::Float3, "Normal"}
    };
    vertexBuffer->SetLayout(layout);
    m_vertexArray = TRAP::Graphics::VertexArray::Create();
    m_vertexArray->SetVertexBuffer(vertexBuffer);
    m_vertexArray->SetIndexBuffer(indexBuffer);

    TRAP::Graphics::RenderCommand::SetClearColor();
    TRAP::Graphics::RenderCommand::SetCull(true);
    TRAP::Graphics::RenderCommand::SetBlend(true);
    TRAP::Graphics::RenderCommand::SetDepthTesting(true);
}

//-------------------------------------------------------------------------------------------------------------------//

void Cube3D::OnDetach()
{
    m_vertexArray->Unbind();
    m_vertexArray.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void Cube3D::OnImGuiRender()
{
    const TRAP::Math::Vec3 pos = m_camera.GetCamera().GetPosition();
    const TRAP::Math::Vec3 rot = m_camera.GetCamera().GetRotation();
    float fov = m_camera.GetFoV();
    float sensitivity = m_camera.GetMouseSensitivity();
    float movement = m_camera.GetTranslationSpeed();
	const float fovCopy = fov;
    const float sensitivityCopy = sensitivity;
    const float movementCopy = movement;

    ImGuiWindowFlags flags;

	//Make sure ImGui Windows do not get focused while using mouse for Camera
    if (m_ignoreImGui)
        flags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoInputs;
    else
        flags = ImGuiWindowFlags_NoFocusOnAppearing;
	
    ImGui::Begin("Settings", nullptr, flags);
    ImGui::Text("Performance");
    ImGui::Text("GPU: %s", TRAP::Graphics::API::RendererAPI::GetRenderer()->GetCurrentGPUName().c_str());
    ImGui::Text("DrawCalls: %u", TRAP::Graphics::Renderer::GetDrawCalls());
    ImGui::Text("FPS: %u", TRAP::Graphics::Renderer::GetFPS());
    ImGui::Text("FrameTime: %.3fms", TRAP::Graphics::Renderer::GetFrameTime());
    ImGui::PlotLines("", m_frameTimeHistory.data(), static_cast<int>(m_frameTimeHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
    ImGui::Separator();
    ImGui::Text("Camera");
    ImGui::Text("Camera Position: %f %f %f", pos.x, pos.y, pos.z);
    ImGui::Text("Camera Rotation: %f %f %f", rot.x, rot.y, rot.z);
    ImGui::SliderFloat("Camera FoV", &fov, 1.0f, 100.0f);
    ImGui::SliderFloat("Camera Sensitivity", &sensitivity, 0.01f, 50.0f);
    ImGui::SliderFloat("Camera Movement", &movement, 0.25f, 100.0f);
    ImGui::Text("Press ALT to toggle Mouse Movement");
    ImGui::Separator();
    ImGui::Text("Cube");
    ImGui::DragFloat3("Cube Position", &m_cubePosition[0]);
    ImGui::DragFloat3("Cube Rotation", &m_cubeRotation[0], 1, -360.0f, 360.0f);
    ImGui::DragFloat3("Cube Scale", &m_cubeScale[0], 1, 0.1f, 1000.0f);
    ImGui::Separator();
    ImGui::Text("Press F1 to switch Shaders");
    ImGui::Text("Press F2 for WireFrame");
    ImGui::Text("Current Shader:");
    ImGui::Text("%s", m_shaderNames[m_currentShader].c_str());
    ImGui::Text("Wireframe: %s", m_wireFrame ? "Enabled" : "Disabled");
    ImGui::End();

    if (fov != fovCopy)
        m_camera.SetFoV(fov);
    if (sensitivity != sensitivityCopy)
        m_camera.SetMouseSensitivity(sensitivity);
    if (movement != movementCopy)
        m_camera.SetTranslationSpeed(movement);
}

//-------------------------------------------------------------------------------------------------------------------//

void Cube3D::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
    m_camera.OnUpdate(deltaTime);
	
	//Render
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color_Depth);

    TRAP::Graphics::Renderer::BeginScene(m_camera.GetCamera());
	{
        TRAP::Graphics::Renderer::Submit(TRAP::Graphics::ShaderManager::Get(m_shaderNames[m_currentShader]),
                                         m_vertexArray,
                                         TRAP::Math::Translate(m_cubePosition) *
                                         TRAP::Math::Rotate(TRAP::Math::Radians(-m_cubeRotation.z), { 0.0f, 0.0f, 1.0f }) *
                                         TRAP::Math::Rotate(TRAP::Math::Radians(-m_cubeRotation.y), { 0.0f, 1.0f, 0.0f }) *
                                         TRAP::Math::Rotate(TRAP::Math::Radians(m_cubeRotation.x), {1.0f, 0.0f, 0.0f}) *
                                         TRAP::Math::Scale(m_cubeScale));
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
}

//-------------------------------------------------------------------------------------------------------------------//

void Cube3D::OnEvent(TRAP::Events::Event& event)
{
    TRAP::Events::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e) { return OnKeyPress(e); });

    m_camera.OnEvent(event);
}

//-------------------------------------------------------------------------------------------------------------------//

bool Cube3D::OnKeyPress(TRAP::Events::KeyPressEvent& event)
{
    if (event.GetKey() == TRAP::Input::Key::Escape)
    {
        TRAP::Application::Shutdown();
        return true;
    }

	if(event.GetKey() == TRAP::Input::Key::F1 && event.GetRepeatCount() == 0)
	{
        m_currentShader = (m_currentShader + 1) % m_shaderNames.size();
        return true;
	}

	if(event.GetKey() == TRAP::Input::Key::F2 && event.GetRepeatCount() == 0)
	{
        m_wireFrame = !m_wireFrame;
        TRAP::Graphics::RenderCommand::SetWireFrame(m_wireFrame);
        TRAP::Graphics::RenderCommand::SetDepthTesting(!m_wireFrame);
        return true;
	}

	if(event.GetKey() == TRAP::Input::Key::Left_ALT && event.GetRepeatCount() == 0)
        m_ignoreImGui = !m_ignoreImGui;

    return false;
}