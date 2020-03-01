#include "TRAPPCH.h"
#include "Renderer2D.h"

#include "Buffers/VertexArray.h"
#include "Shaders/Shader.h"
#include "Buffers/UniformBuffer.h"
#include "Shaders/ShaderManager.h"
#include "RenderCommand.h"
#include "Application.h"
#include "Embed.h"
#include "Textures/TextureManager.h"

namespace TRAP::Graphics
{
	struct Renderer2DStorage
	{		
		Scope<VertexArray> QuadVertexArray;

		Scope<UniformBuffer> DataUniformBuffer;
		struct UniformData
		{
			Math::Vec4 Color{ 0.2f, 0.3f, 0.8f, 1.0f };
		} UniformData;

		Scope<UniformBuffer> CameraUniformBuffer;
		struct UniformCamera
		{
			Math::Mat4 ProjectionMatrix{ 1.0f };
			Math::Mat4 ViewMatrix{ 1.0f };
			Math::Mat4 ModelMatrix{ 1.0f };
		} UniformCamera;
	};

	static Scope<Renderer2DStorage> s_data;
}

void TRAP::Graphics::Renderer2D::Init()
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG("[Renderer2D] Initializing");
	
	s_data = MakeScope<Renderer2DStorage>();
	s_data->QuadVertexArray = VertexArray::Create();

	///////////////
	//    Quad   //
	///////////////
	//XYZ RGBA
	std::array<float, 5 * 4> vertices //Quad
	{
		-0.5f, -0.5f, 0.0f,    0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,    1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,    1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,    0.0f, 1.0f
	};
	Scope<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices.data(), static_cast<uint32_t>(vertices.size()));
	const BufferLayout layout =
	{
		{ShaderDataType::Float3, "Position"},
		{ShaderDataType::Float2, "UV"}
	};
	vertexBuffer->SetLayout(layout);
	s_data->QuadVertexArray->AddVertexBuffer(vertexBuffer);

	std::array<uint32_t, 6> indices //Quad
	{
		0, 1, 2, 2, 3, 0
	};
	Scope<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), static_cast<uint32_t>(indices.size()));
	s_data->QuadVertexArray->SetIndexBuffer(indexBuffer);

	s_data->CameraUniformBuffer = UniformBuffer::Create("CameraBuffer", &s_data->UniformCamera, sizeof(Renderer2DStorage::UniformCamera), BufferUsage::Stream);
	s_data->DataUniformBuffer = UniformBuffer::Create("DataBuffer", &s_data->UniformData, sizeof(Renderer2DStorage::UniformData), BufferUsage::Dynamic);

	ShaderManager::Load("Renderer2D", Embed::Renderer2DVS, Embed::Renderer2DFS);
	TextureManager::Load("Renderer2DWhite", 1, 1, 32, ImageFormat::RGBA, std::vector<uint8_t>{255, 255, 255, 255});
	
	TextureManager::Get2D("Fallback2D")->Bind(0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::Shutdown()
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG("[Renderer2D] Shutting down");
	s_data.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::BeginScene(const OrthographicCamera& camera)
{
	TP_PROFILE_FUNCTION();

	s_data->UniformCamera.ProjectionMatrix = camera.GetProjectionMatrix();
	s_data->UniformCamera.ViewMatrix = camera.GetViewMatrix();
	
	ShaderManager::Get("Renderer2D")->Bind();
	s_data->DataUniformBuffer->Bind(1);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::EndScene()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const Math::Vec4& color)
{
	TP_PROFILE_FUNCTION();

	//Bind and Update DataUniformBuffer if color changed
	s_data->DataUniformBuffer->Bind(1);
	if (s_data->UniformData.Color != color)
	{
		s_data->UniformData.Color = color;
		s_data->DataUniformBuffer->UpdateData(&s_data->UniformData);
	}

	//Bind White Texture
	TextureManager::Get2D("Renderer2DWhite")->Bind(0);

	DrawQuad(transform);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const Scope<Texture2D>& texture)
{
	TP_PROFILE_FUNCTION();

	//Bind Texture
	texture->Bind(0);

	//Bind and Update DataUniformBuffer if color changed
	s_data->DataUniformBuffer->Bind(1);
	if (s_data->UniformData.Color != Math::Vec4(1.0f))
	{
		s_data->UniformData.Color = Math::Vec4(1.0f);
		s_data->DataUniformBuffer->UpdateData(&s_data->UniformData);
	}

	DrawQuad(transform);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const Math::Vec4& color, const Scope<Texture2D>& texture)
{
	TP_PROFILE_FUNCTION();

	//Bind and Update DataUniformBuffer if color changed
	s_data->DataUniformBuffer->Bind(1);
	if (s_data->UniformData.Color != color)
	{
		s_data->UniformData.Color = color;
		s_data->DataUniformBuffer->UpdateData(&s_data->UniformData);
	}

	DrawQuad(transform, texture);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform)
{
	TP_PROFILE_FUNCTION();

	//For Draw call counter
	Application::AddSingleDrawCall();

	//Update CameraUniformBuffer
	//Position & Size & Rotation
	s_data->UniformCamera.ModelMatrix = Translate(transform.Position) *
		                                Math::Rotate(Math::Radians(transform.Rotation.x), {1.0f, 0.0f, 0.0f}) *
		                                Math::Rotate(Math::Radians(transform.Rotation.y), {0.0f, 1.0f, 0.0f}) *
		                                Math::Rotate(Math::Radians(transform.Rotation.z), {0.0f, 0.0f, 1.0f}) *
		                                Scale(transform.Scale); 
	s_data->CameraUniformBuffer->UpdateData(&s_data->UniformCamera);
	s_data->CameraUniformBuffer->Bind(0);

	//Bind Shader
	ShaderManager::Get("Renderer2D")->Bind();
	
	//Bind Vertex Array
	s_data->QuadVertexArray->Bind();

	//Render the Quad
	RenderCommand::DrawIndexed(s_data->QuadVertexArray);
}