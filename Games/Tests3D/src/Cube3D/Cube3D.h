#ifndef GAMESTRAP_CUBE3D_H
#define GAMESTRAP_CUBE3D_H

#include <TRAP.h>

class Cube3D final : public TRAP::Layer
{
public:
	Cube3D();

	void OnAttach() override;
	void OnImGuiRender() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	constexpr void OnEvent(TRAP::Events::Event& event) override;

	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
	bool OnMouseMove(const TRAP::Events::MouseMoveEvent& event);
	constexpr bool OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& event);

private:
	TRAP::Scope<TRAP::Graphics::VertexBuffer> m_cubeVertexBuffer = nullptr;
	TRAP::Scope<TRAP::Graphics::IndexBuffer> m_cubeIndexBuffer = nullptr;
	TRAP::Scope<TRAP::Graphics::VertexBuffer> m_skyBoxVertexBuffer = nullptr;

	TRAP::Graphics::RendererAPI::SamplerDesc m_textureSamplerDesc{};
	TRAP::Ref<TRAP::Graphics::Sampler> m_textureSampler = nullptr;

	struct DiffuseReflectionDataBuffer
	{
		TRAP::Math::Vec4 LightPosition;
		TRAP::Math::Vec3 LightSourceIntensity;
		f32 UNUSED;
		TRAP::Math::Vec3 DiffuseReflectivity;
	} m_diffuseReflectionDataBuffer{};
	TRAP::Scope<TRAP::Graphics::UniformBuffer> m_diffuseReflectionUniformBuffer = nullptr;

	struct PhongLightningDataBuffer
	{
		TRAP::Math::Vec4 LightPosition;
		TRAP::Math::Vec3 LightLa;
		f32 UNUSED;
		TRAP::Math::Vec3 LightLd;
		f32 UNUSED1;
		TRAP::Math::Vec3 LightLs;
		f32 UNUSED2;
		TRAP::Math::Vec3 MaterialKa;
		f32 UNUSED3;
		TRAP::Math::Vec3 MaterialKd;
		f32 UNUSED4;
		TRAP::Math::Vec3 MaterialKs;
		f32 MaterialShininess;
	} m_phongLightningDataBuffer{};
	TRAP::Scope<TRAP::Graphics::UniformBuffer> m_phongLightningUniformBuffer = nullptr;
	TRAP::Math::Vec4 m_lightPosition = TRAP::Math::Vec4(5.0f, 5.0f, 2.0f, 1.0f);

	TRAP::Math::Vec3 m_cubePosition{0.0f, 0.0f, -5.0f};
	TRAP::Math::Vec3 m_cubeRotation{0.0f, 0.0f, 0.0f};
	TRAP::Math::Vec3 m_cubeScale{1.0f, 1.0f, 1.0f};

	std::array<f32, 50u> m_frameTimeHistory{};
	TRAP::Utils::Timer m_titleTimer{};

	bool m_ignoreImGui = false;

	std::vector<std::string> m_shaderNames{"Base", "Color", "Texture", "Diffuse Reflection", "Phong Lightning"};
	u32 m_currentShader = 0u;
	bool m_wireFrame = false;
	bool m_drawSkyBox = true;

	TRAP::SceneCamera m_camera{};
	TRAP::TransformComponent m_cameraTransform{};
	f32 m_mouseSensitivity = 5.0f;
	f32 m_translationSpeed = 2.5f;
	bool m_firstMouse = true; //Camera Controls
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr void Cube3D::OnEvent(TRAP::Events::Event& event)
{
    const TRAP::Events::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&Cube3D::OnKeyPress, this));
    dispatcher.Dispatch<TRAP::Events::MouseMoveEvent>(std::bind_front(&Cube3D::OnMouseMove, this));
    dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>(std::bind_front(&Cube3D::OnFrameBufferResize, this));
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool Cube3D::OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& event)
{
    m_camera.SetViewportSize(event.GetWidth(), event.GetHeight());

    return true;
}

#endif /*GAMESTRAP_CUBE3D_H*/
