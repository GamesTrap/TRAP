#include "Cube3D.h"

namespace
{
    constexpr std::array<f32, 11ull * 24ull> CubeVertices
    {
        //Positions             //Colors             //Textures      //Normals
        -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 1.0f,     0.0f,  0.0f, -1.0f,
         1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,     0.0f,  0.0f, -1.0f,
         1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 0.0f,    0.0f, 0.0f,     0.0f,  0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,     0.0f,  0.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,     0.0f,  0.0f,  1.0f,
         1.0f, -1.0f,  1.0f,    1.0f, 0.0f, 1.0f,    1.0f, 1.0f,     0.0f,  0.0f,  1.0f,
         1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f,     0.0f,  0.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f,     0.0f,  0.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,    -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 0.0f,    0.0f, 1.0f,    -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,    -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 1.0f,    1.0f, 0.0f,    -1.0f,  0.0f,  0.0f,

         1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,     1.0f,  0.0f,  0.0f,
         1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f,     1.0f,  0.0f,  0.0f,
         1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 0.0f,     1.0f,  0.0f,  0.0f,
         1.0f, -1.0f,  1.0f,    1.0f, 0.0f, 1.0f,    0.0f, 1.0f,     1.0f,  0.0f,  0.0f,

        -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 0.0f,    0.0f, 1.0f,     0.0f, -1.0f,  0.0f,
         1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,     0.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  1.0f,    1.0f, 0.0f, 1.0f,    1.0f, 0.0f,     0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,     0.0f, -1.0f,  0.0f,

         1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f,     0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,     0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f,     0.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     0.0f,  1.0f,  0.0f
    };

    constexpr std::array<u16, 36u> CubeIndices
    {
         0u,  3u,  2u,
         2u,  1u,  0u,

         4u,  5u,  6u,
         6u,  7u,  4u,

        11u,  8u,  9u,
         9u, 10u, 11u,

        12u, 13u, 14u,
        14u, 15u, 12u,

        16u, 17u, 18u,
        18u, 19u, 16u,

        20u, 21u, 22u,
        22u, 23u, 20u
    };

    constexpr std::array<f32, 3ull * 36ull> SkyBoxVertices
    {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
}

//-------------------------------------------------------------------------------------------------------------------//

Cube3D::Cube3D()
    : Layer("Cube3D")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void Cube3D::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Cube3D");

	//Load Shader(s)
    TRAP::Graphics::ShaderManager::LoadFile("Base", "./Assets/Shaders/Base.shader", TRAP::Graphics::ShaderType::Graphics);
    TRAP::Graphics::ShaderManager::LoadFile("Color", "./Assets/Shaders/Color.shader", TRAP::Graphics::ShaderType::Graphics);
    TRAP::Graphics::ShaderManager::LoadFile("Texture", "./Assets/Shaders/Texture.shader", TRAP::Graphics::ShaderType::Graphics);
    TRAP::Graphics::ShaderManager::LoadFile("Diffuse Reflection", "./Assets/Shaders/DiffuseReflection.shader", TRAP::Graphics::ShaderType::Graphics);
    TRAP::Graphics::ShaderManager::LoadFile("Phong Lightning", "./Assets/Shaders/PhongLightning.shader", TRAP::Graphics::ShaderType::Graphics);
    TRAP::Graphics::ShaderManager::LoadFile("SkyBox", "./Assets/Shaders/SkyBox.shader", TRAP::Graphics::ShaderType::Graphics);

	//Load Texture(s)
    TRAP::Graphics::TextureManager::Load("SkyBox", "./Assets/Textures/SkyboxCubic.png",
                                         TRAP::Graphics::TextureCubeFormat::Cross);
    TRAP::Graphics::TextureManager::Load("UVGrid", "./Assets/Textures/UVGrid.png");

    m_cubeVertexBuffer = TRAP::Graphics::VertexBuffer::Create(CubeVertices,
                                                              TRAP::Graphics::UpdateFrequency::Static);
    m_cubeIndexBuffer = TRAP::Graphics::IndexBuffer::Create(CubeIndices,
                                                            TRAP::Graphics::UpdateFrequency::Static);
    const TRAP::Graphics::VertexBufferLayout cubeLayout =
    {
    	{TRAP::Graphics::ShaderDataType::Float3, "Position"},
    	{TRAP::Graphics::ShaderDataType::Float3, "Color"},
    	{TRAP::Graphics::ShaderDataType::Float2, "UV"},
    	{TRAP::Graphics::ShaderDataType::Float3, "Normal"}
    };
    m_cubeVertexBuffer->SetLayout(cubeLayout);

    m_skyBoxVertexBuffer = TRAP::Graphics::VertexBuffer::Create(SkyBoxVertices,
                                                                TRAP::Graphics::UpdateFrequency::Static);
    const TRAP::Graphics::VertexBufferLayout skyBoxLayout =
    {
        {TRAP::Graphics::ShaderDataType::Float3, "Position"}
    };
    m_skyBoxVertexBuffer->SetLayout(skyBoxLayout);

    m_textureSamplerDesc = TRAP::Graphics::RendererAPI::SamplerDesc
    {
        .MinFilter = TRAP::Graphics::FilterType::Linear,
        .MagFilter = TRAP::Graphics::FilterType::Linear,
        .MipMapMode = TRAP::Graphics::MipMapMode::Linear,
        .AddressU = TRAP::Graphics::AddressMode::ClampToEdge,
        .AddressV = TRAP::Graphics::AddressMode::ClampToEdge,
        .AddressW = TRAP::Graphics::AddressMode::ClampToEdge,
        .EnableAnisotropy = true
    };
    m_textureSampler = TRAP::Graphics::Sampler::Create(m_textureSamplerDesc);

	//Camera setup
    m_camera.SetPerspective(TRAP::Math::Radians(45.0f), 0.01f);
    m_camera.SetViewportSize(TRAP::Application::GetWindow()->GetFrameBufferSize().x(),
                             TRAP::Application::GetWindow()->GetFrameBufferSize().y());

    //Load diffuse reflection UniformBuffer
    const TRAP::Math::Mat4 inverseView = TRAP::Math::Inverse(m_cameraTransform.GetTransform());
    m_diffuseReflectionDataBuffer.LightPosition = inverseView * m_lightPosition;
    m_diffuseReflectionDataBuffer.LightSourceIntensity = { 1.0f, 1.0f, 1.0f, };
    m_diffuseReflectionDataBuffer.DiffuseReflectivity = { 0.9f, 0.5f, 0.3f };
    m_diffuseReflectionUniformBuffer = TRAP::Graphics::UniformBuffer::Create(&m_diffuseReflectionDataBuffer,
                                                                             sizeof(DiffuseReflectionDataBuffer),
                                                                             TRAP::Graphics::UpdateFrequency::Dynamic);
    m_diffuseReflectionUniformBuffer->AwaitLoading();

    //Load phong lightning UniformBuffer
    m_phongLightningDataBuffer.LightPosition = inverseView * m_lightPosition;
    m_phongLightningDataBuffer.LightLa = { 0.4f, 0.4f, 0.4f };
    m_phongLightningDataBuffer.LightLd = { 1.0f, 1.0f, 1.0f };
    m_phongLightningDataBuffer.LightLs = { 1.0f, 1.0f, 1.0f };
    m_phongLightningDataBuffer.MaterialKa = { 0.9f, 0.5f, 0.3f };
    m_phongLightningDataBuffer.MaterialKd = { 0.9f, 0.5f, 0.3f };
    m_phongLightningDataBuffer.MaterialKs = { 0.8f, 0.8f, 0.8f };
    m_phongLightningDataBuffer.MaterialShininess = 100.0f;
    m_phongLightningUniformBuffer = TRAP::Graphics::UniformBuffer::Create(&m_phongLightningDataBuffer,
                                                                          sizeof(PhongLightningDataBuffer),
                                                                          TRAP::Graphics::UpdateFrequency::Dynamic);
    m_phongLightningUniformBuffer->AwaitLoading();

    //Wait for all pending resources (just in case)
    TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

    TRAP::Graphics::RenderCommand::SetDepthTesting(true);
    TRAP::Graphics::RenderCommand::SetDepthWriting(true);
    TRAP::Graphics::RenderCommand::SetDepthFunction(TRAP::Graphics::CompareMode::GreaterOrEqual);
}

//-------------------------------------------------------------------------------------------------------------------//

void Cube3D::OnImGuiRender()
{
    const TRAP::Math::Vec3 pos = m_cameraTransform.Position;
    const TRAP::Math::Vec3 rot = TRAP::Math::Degrees(m_cameraTransform.Rotation);
    f32 fov = TRAP::Math::Degrees(m_camera.GetPerspectiveVerticalFOV());
    f32 sensitivity = m_mouseSensitivity;
    f32 movement = m_translationSpeed;

    ImGuiWindowFlags flags = ImGuiWindowFlags_None;

	//Make sure ImGui Windows do not get focused while using mouse for Camera
    if (m_ignoreImGui)
        flags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoInputs;
    else
        flags = ImGuiWindowFlags_NoFocusOnAppearing;

    ImGui::Begin("Settings", nullptr, flags);
    ImGui::Text("Performance");
    ImGui::Text("CPU: %ix %s", TRAP::Utils::GetCPUInfo().LogicalCores, TRAP::Utils::GetCPUInfo().Model.c_str());
	ImGui::Text("GPU: %s", TRAP::Graphics::RenderCommand::GetGPUName().c_str());
    ImGui::Text("CPU FPS: %u", TRAP::Graphics::RenderCommand::GetCPUFPS());
    ImGui::Text("GPU FPS: %u", TRAP::Graphics::RenderCommand::GetGPUFPS());
    ImGui::Text("CPU FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetCPUFrameTime());
    ImGui::Text("GPU Graphics FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUGraphicsFrameTime());
    ImGui::Text("GPU Compute FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUComputeFrameTime());
    ImGui::PlotLines("##frametimeHistory", m_frameTimeHistory.data(), NumericCast<i32>(m_frameTimeHistory.size()),
                     0, nullptr, 0.0f, 33.0f, ImVec2(200.0f, 50.0f));
    ImGui::Separator();
    ImGui::Text("Camera");
    ImGui::Text("Camera Position: %f %f %f", pos.x(), pos.y(), pos.z());
    ImGui::Text("Camera Rotation: %f %f %f", rot.x(), rot.y(), rot.z());
    if(ImGui::SliderFloat("Camera FoV", &fov, 1.0f, 100.0f))
        m_camera.SetPerspectiveVerticalFOV(TRAP::Math::Radians(fov));
    if(ImGui::SliderFloat("Camera Sensitivity", &sensitivity, 0.01f, 50.0f))
        m_mouseSensitivity = sensitivity;
    if(ImGui::SliderFloat("Camera Movement", &movement, 0.25f, 100.0f))
        m_translationSpeed = movement;
    ImGui::Text("Press ALT to toggle Mouse Movement");
    ImGui::Separator();
    ImGui::Text("Cube");
    ImGui::DragFloat3("Cube Position", &std::get<0u>(m_cubePosition));
    ImGui::DragFloat3("Cube Rotation", &std::get<0u>(m_cubeRotation), 1, -360.0f, 360.0f);
    ImGui::DragFloat3("Cube Scale", &std::get<0u>(m_cubeScale), 1, 0.1f, 1000.0f);
    ImGui::Separator();
	if (m_shaderNames[m_currentShader] == "Diffuse Reflection")
	{
        DiffuseReflectionDataBuffer diffuseReflectionDataBuffer = m_diffuseReflectionDataBuffer;

        ImGui::Text("Diffuse Reflection");
        ImGui::DragFloat3("Light Position", &std::get<0u>(m_lightPosition), 0.1f);
        ImGui::SliderFloat3("Light Source Intensity", &std::get<0u>(diffuseReflectionDataBuffer.LightSourceIntensity), 0.0f,
                            1.0f);
        ImGui::SliderFloat3("Diffuse Reflectivity", &std::get<0u>(diffuseReflectionDataBuffer.DiffuseReflectivity), 0.0f,
                            1.0f);
        ImGui::Separator();

		if (diffuseReflectionDataBuffer.LightSourceIntensity != m_diffuseReflectionDataBuffer.LightSourceIntensity ||
            diffuseReflectionDataBuffer.DiffuseReflectivity != m_diffuseReflectionDataBuffer.DiffuseReflectivity)
		{
            m_diffuseReflectionDataBuffer.DiffuseReflectivity = diffuseReflectionDataBuffer.DiffuseReflectivity;
            m_diffuseReflectionDataBuffer.LightSourceIntensity = diffuseReflectionDataBuffer.LightSourceIntensity;
		}
	}
	else if (m_shaderNames[m_currentShader] == "Phong Lightning")
    {
        PhongLightningDataBuffer phongLightningDataBuffer = m_phongLightningDataBuffer;

        ImGui::Text("Phong Lightning");
        ImGui::Text("Light");
        ImGui::DragFloat3("Light Position", &std::get<0u>(m_lightPosition), 0.1f);
        ImGui::SliderFloat3("Light Ambient", &std::get<0u>(phongLightningDataBuffer.LightLa), 0.0f, 1.0f);
        ImGui::SliderFloat3("Light Diffuse", &std::get<0u>(phongLightningDataBuffer.LightLd), 0.0f, 1.0f);
        ImGui::SliderFloat3("Light Specular", &std::get<0u>(phongLightningDataBuffer.LightLs), 0.0f, 1.0f);
        ImGui::Text("Material");
        ImGui::SliderFloat3("Material Ambient", &std::get<0u>(phongLightningDataBuffer.MaterialKa), 0.0f, 1.0f);
        ImGui::SliderFloat3("Material Diffuse", &std::get<0u>(phongLightningDataBuffer.MaterialKd), 0.0f, 1.0f);
        ImGui::SliderFloat3("Material Specular", &std::get<0u>(phongLightningDataBuffer.MaterialKs), 0.0f, 1.0f);
        ImGui::SliderFloat("Material Shininess", &phongLightningDataBuffer.MaterialShininess, 1.0f, 100.0f);
        ImGui::Separator();

		if (phongLightningDataBuffer.LightLa != m_phongLightningDataBuffer.LightLa ||
            phongLightningDataBuffer.LightLd != m_phongLightningDataBuffer.LightLd ||
            phongLightningDataBuffer.LightLs != m_phongLightningDataBuffer.LightLs ||
            phongLightningDataBuffer.MaterialKa != m_phongLightningDataBuffer.MaterialKa ||
            phongLightningDataBuffer.MaterialKd != m_phongLightningDataBuffer.MaterialKd ||
            phongLightningDataBuffer.MaterialKs != m_phongLightningDataBuffer.MaterialKs ||
            phongLightningDataBuffer.MaterialShininess != m_phongLightningDataBuffer.MaterialShininess)
		{
            m_phongLightningDataBuffer.LightLa = phongLightningDataBuffer.LightLa;
            m_phongLightningDataBuffer.LightLd = phongLightningDataBuffer.LightLd;
            m_phongLightningDataBuffer.LightLs = phongLightningDataBuffer.LightLs;
            m_phongLightningDataBuffer.MaterialKa = phongLightningDataBuffer.MaterialKa;
            m_phongLightningDataBuffer.MaterialKd = phongLightningDataBuffer.MaterialKd;
            m_phongLightningDataBuffer.MaterialKs = phongLightningDataBuffer.MaterialKs;
            m_phongLightningDataBuffer.MaterialShininess = phongLightningDataBuffer.MaterialShininess;
		}
    }
    ImGui::Text("Press F1 to switch Shaders");
    ImGui::Text("Current Shader:");
    ImGui::Text("%s", m_shaderNames[m_currentShader].c_str());
    ImGui::Checkbox("WireFrame", &m_wireFrame);
    ImGui::Checkbox("Skybox", &m_drawSkyBox);
    ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void Cube3D::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
    //Update sampler
	if(m_textureSampler->UsesEngineAnisotropyLevel() &&
	   static_cast<f32>(TRAP::Graphics::RenderCommand::GetAnisotropyLevel()) != m_textureSampler->GetAnisotropyLevel())
	{
		m_textureSampler = TRAP::Graphics::Sampler::Create(m_textureSamplerDesc);
	}

    //Use Textures
    TRAP::Graphics::ShaderManager::GetGraphics("Texture")->UseTexture(0u, 0u, *TRAP::Graphics::TextureManager::Get2D("UVGrid"));
    TRAP::Graphics::ShaderManager::GetGraphics("Texture")->UseSampler(0u, 1u, *m_textureSampler);
    TRAP::Graphics::ShaderManager::GetGraphics("SkyBox")->UseTexture(0u, 0u, *TRAP::Graphics::TextureManager::GetCube("SkyBox"));
    TRAP::Graphics::ShaderManager::GetGraphics("SkyBox")->UseSampler(0u, 1u, *m_textureSampler);

    if(m_wireFrame)
    {
        TRAP::Graphics::RenderCommand::SetFillMode(TRAP::Graphics::FillMode::Line);
        TRAP::Graphics::RenderCommand::SetCullMode(TRAP::Graphics::CullMode::None);
    }
    else
    {
        TRAP::Graphics::RenderCommand::SetFillMode(TRAP::Graphics::FillMode::Solid);
        TRAP::Graphics::RenderCommand::SetCullMode(TRAP::Graphics::CullMode::Back);
    }

    TRAP::Graphics::Renderer::BeginScene(m_camera, m_cameraTransform.GetTransform());
	{
        if(m_drawSkyBox)
    	{
            TRAP::Graphics::Renderer::Submit(*TRAP::Graphics::ShaderManager::GetGraphics("SkyBox"),
                                             *m_skyBoxVertexBuffer);
    	}

        if (m_shaderNames[m_currentShader] == "Diffuse Reflection")
        {
            m_diffuseReflectionDataBuffer.LightPosition = TRAP::Math::Inverse(m_cameraTransform.GetTransform()) *
                                                          m_lightPosition;
            m_diffuseReflectionUniformBuffer->SetData(&m_diffuseReflectionDataBuffer,
                                                      sizeof(DiffuseReflectionDataBuffer));
            m_diffuseReflectionUniformBuffer->AwaitLoading();
            TRAP::Graphics::ShaderManager::GetGraphics("Diffuse Reflection")->UseUBO(1u, 2u, *m_diffuseReflectionUniformBuffer);

            TRAP::Graphics::Renderer::Submit(*TRAP::Graphics::ShaderManager::GetGraphics(m_shaderNames[0u]),
                                             *m_cubeVertexBuffer, *m_cubeIndexBuffer,
                                             TRAP::Math::Translate(TRAP::Math::Vec3(m_lightPosition)) *
                                             TRAP::Math::Scale(TRAP::Math::Vec3(0.1f, 0.1f, 0.1f)));
        }
        else if (m_shaderNames[m_currentShader] == "Phong Lightning")
        {
            m_phongLightningDataBuffer.LightPosition = TRAP::Math::Inverse(m_cameraTransform.GetTransform()) *
                                                       m_lightPosition;
            m_phongLightningUniformBuffer->SetData(&m_phongLightningDataBuffer,
                                                   sizeof(PhongLightningDataBuffer));
            m_phongLightningUniformBuffer->AwaitLoading();
            TRAP::Graphics::ShaderManager::GetGraphics("Phong Lightning")->UseUBO(1u, 2u, *m_phongLightningUniformBuffer);

            TRAP::Graphics::Renderer::Submit(*TRAP::Graphics::ShaderManager::GetGraphics(m_shaderNames[0u]),
                                             *m_cubeVertexBuffer, *m_cubeIndexBuffer,
                                             TRAP::Math::Translate(TRAP::Math::Vec3(m_lightPosition)) *
                                             TRAP::Math::Scale(TRAP::Math::Vec3(0.1f, 0.1f, 0.1f)));
        }

        TRAP::Graphics::Renderer::Submit(*TRAP::Graphics::ShaderManager::GetGraphics(m_shaderNames[m_currentShader]),
                                         *m_cubeVertexBuffer, *m_cubeIndexBuffer,
                                         TRAP::Math::Translate(m_cubePosition) *
                                         TRAP::Math::Rotate(TRAP::Math::Radians(-m_cubeRotation.z()), { 0.0f, 0.0f, 1.0f }) *
                                         TRAP::Math::Rotate(TRAP::Math::Radians(-m_cubeRotation.y()), { 0.0f, 1.0f, 0.0f }) *
                                         TRAP::Math::Rotate(TRAP::Math::Radians(m_cubeRotation.x()), {1.0f, 0.0f, 0.0f}) *
                                         TRAP::Math::Scale(m_cubeScale));
	}
    TRAP::Graphics::Renderer::EndScene();

    const TRAP::Math::Vec3 rotVec{ m_cameraTransform.Rotation.x(), m_cameraTransform.Rotation.y(),
                                   m_cameraTransform.Rotation.z() };
    const TRAP::Math::Quat orientation = TRAP::Math::Quat(rotVec);
    const TRAP::Math::Quat qF = orientation * TRAP::Math::Quat(0.0f, 0.0f, 0.0f, -1.0f) *
                                      TRAP::Math::Conjugate(orientation);
    const TRAP::Math::Vec3 front = { qF.x(), qF.y(), qF.z() };
    const TRAP::Math::Vec3 right = TRAP::Math::Normalize(TRAP::Math::Cross(front,
                                                                           TRAP::Math::YAxis<f32>()));

    //Keyboard Position
    if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::A))
        m_cameraTransform.Position -= right * (m_translationSpeed * deltaTime);
    if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::D))
        m_cameraTransform.Position += right * (m_translationSpeed * deltaTime);
    if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::W))
        m_cameraTransform.Position += front * (m_translationSpeed * deltaTime);
    if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::S))
        m_cameraTransform.Position -= front * (m_translationSpeed * deltaTime);
    if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::Space))
        m_cameraTransform.Position.y() += m_translationSpeed * deltaTime;
    if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::Left_Shift))
        m_cameraTransform.Position.y() -= m_translationSpeed * deltaTime;

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
}

//-------------------------------------------------------------------------------------------------------------------//

bool Cube3D::OnMouseMove(const TRAP::Events::MouseMoveEvent& event)
{
    if(!m_ignoreImGui)
        return false;

    constinit static f32 lastX = 0.0f;
    constinit static f32 lastY = 0.0f;
    if (m_firstMouse)
    {
        lastX = event.GetX();
        lastY = event.GetY();
        m_firstMouse = false;
    }

    f32 pitch = event.GetX() - lastX;
    f32 yaw = lastY - event.GetY();
    lastX = event.GetX();
    lastY = event.GetY();

    pitch *= TRAP::Math::Radians(m_mouseSensitivity);
    yaw *= TRAP::Math::Radians(m_mouseSensitivity);

    m_cameraTransform.Rotation.x() += TRAP::Math::Radians(yaw);
    m_cameraTransform.Rotation.y() -= TRAP::Math::Radians(pitch);

    //Limit pitch movement
    if (m_cameraTransform.Rotation.x() > TRAP::Math::Radians(89.0f))
        m_cameraTransform.Rotation.x() = TRAP::Math::Radians(89.0f);
    if (m_cameraTransform.Rotation.x() < TRAP::Math::Radians(-89.0f))
        m_cameraTransform.Rotation.x() = TRAP::Math::Radians(-89.0f);

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool Cube3D::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
{
    if (event.GetKey() == TRAP::Input::Key::Escape)
    {
        TRAP::Application::Shutdown();
        return true;
    }

    if(event.GetKey() == TRAP::Input::Key::F1)
	{
        m_currentShader = (m_currentShader + 1u) % NumericCast<u32>(m_shaderNames.size());
        return true;
	}

	if(event.GetKey() == TRAP::Input::Key::F2)
	{
        m_wireFrame = !m_wireFrame;
        return true;
	}

	if(event.GetKey() == TRAP::Input::Key::F3)
	{
        m_drawSkyBox = !m_drawSkyBox;
        return true;
	}

	if(event.GetKey() == TRAP::Input::Key::Left_ALT)
	{
        m_ignoreImGui = !m_ignoreImGui;

        if (m_ignoreImGui)
            TRAP::Application::GetWindow()->SetCursorMode(TRAP::Window::CursorMode::Disabled);
        else
        {
            m_firstMouse = true;
            TRAP::Application::GetWindow()->SetCursorMode(TRAP::Window::CursorMode::Normal);
        }
	}

    return false;
}
