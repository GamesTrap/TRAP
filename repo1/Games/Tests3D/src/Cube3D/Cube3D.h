#ifndef _GAMESTRAP_TITLETESTS_H_
#define _GAMESTRAP_TITLETESTS_H_

#include <TRAP.h>

class Cube3D final : public TRAP::Layer
{
public:
	Cube3D();

	void OnAttach() override;
	void OnDetach() override;
	void OnImGuiRender() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);
	bool OnMouseMove(TRAP::Events::MouseMoveEvent& event);
	bool OnFrameBufferResize(TRAP::Events::FrameBufferResizeEvent& event);
	
private:
	TRAP::Scope<TRAP::Graphics::VertexArray> m_cubeVertexArray;
	TRAP::Scope<TRAP::Graphics::VertexArray> m_skyBoxVertexArray;

	struct DiffuseReflectionDataBuffer
	{
		TRAP::Math::Vec4 LightPosition;
		TRAP::Math::Vec3 LightSourceIntensity;
		float UNUSED;
		TRAP::Math::Vec3 DiffuseReflectivity;
	} m_diffuseReflectionDataBuffer;
	TRAP::Scope<TRAP::Graphics::UniformBuffer> m_diffuseReflectionUniformBuffer;
	
	struct PhongLightningDataBuffer
	{
		TRAP::Math::Vec4 LightPosition;
		TRAP::Math::Vec3 LightLa;
		float UNUSED;
		TRAP::Math::Vec3 LightLd;
		float UNUSED1;
		TRAP::Math::Vec3 LightLs;
		float UNUSED2;
		TRAP::Math::Vec3 MaterialKa;
		float UNUSED3;
		TRAP::Math::Vec3 MaterialKd;
		float UNUSED4;
		TRAP::Math::Vec3 MaterialKs;
		float MaterialShininess;
	} m_phongLightningDataBuffer;
	TRAP::Scope<TRAP::Graphics::UniformBuffer> m_phongLightningUniformBuffer;
	TRAP::Math::Vec4 m_lightPosition = TRAP::Math::Vec4(5.0f, 5.0f, 2.0f, 1.0f);
	
	TRAP::Math::Vec3 m_cubePosition;
	TRAP::Math::Vec3 m_cubeRotation;
	TRAP::Math::Vec3 m_cubeScale;

	std::array<float, 50> m_frameTimeHistory;
	TRAP::Utils::Timer m_titleTimer;

	bool m_ignoreImGui;

	std::vector<std::string> m_shaderNames;
	uint32_t m_currentShader;
	bool m_wireFrame;
	bool m_drawSkyBox;

	TRAP::SceneCamera m_camera;
	TRAP::TransformComponent m_cameraTransform;
	float m_mouseSensitivity;
	float m_translationSpeed;
	bool m_firstMouse; //Camera Controls
};

#endif /*_GAMESTRAP_TITLETESTS_H_*/