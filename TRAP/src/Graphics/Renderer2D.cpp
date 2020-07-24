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
	struct Renderer2DData
	{		
		Scope<VertexArray> QuadVertexArray;
		Scope<VertexArray> TriangleVertexArray;

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

	static Scope<Renderer2DData> s_data;
}

void TRAP::Graphics::Renderer2D::Init()
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG(Log::Renderer2DPrefix, "Initializing");
	
	s_data = MakeScope<Renderer2DData>();
	s_data->QuadVertexArray = VertexArray::Create();
	s_data->TriangleVertexArray = VertexArray::Create();

	///////////////
	//    Quad   //
	///////////////
	//XYZ UV
	std::array<float, 5 * 4> quadVertices
	{
		-0.5f, -0.5f, 0.0f,    0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,    1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,    1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,    0.0f, 1.0f
	};
	Scope<VertexBuffer> quadVertexBuffer = VertexBuffer::Create(quadVertices.data(), static_cast<uint32_t>(quadVertices.size()) * sizeof(uint32_t));
	const BufferLayout layout =
	{
		{ShaderDataType::Float3, "Position"},
		{ShaderDataType::Float2, "UV"}
	};
	quadVertexBuffer->SetLayout(layout);
	s_data->QuadVertexArray->SetVertexBuffer(quadVertexBuffer);

	std::array<uint32_t, 6> quadIndices
	{
		0, 1, 2, 2, 3, 0
	};
	Scope<IndexBuffer> quadIndexBuffer = IndexBuffer::Create(quadIndices.data(), static_cast<uint32_t>(quadIndices.size()));
	s_data->QuadVertexArray->SetIndexBuffer(quadIndexBuffer);

	///////////////
	//    Quad   //
	///////////////
	//XYZ UV
	std::array<float, 5 * 3> triangleVertices
	{
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.5f, 1.0f
	};
	Scope<VertexBuffer> triangleVertexBuffer = VertexBuffer::Create(triangleVertices.data(), static_cast<uint32_t>(triangleVertices.size()) * sizeof(uint32_t));
	triangleVertexBuffer->SetLayout(layout);
	s_data->TriangleVertexArray->SetVertexBuffer(triangleVertexBuffer);

	std::array<uint32_t, 3> triangleIndices
	{
		0, 1, 2
	};
	Scope<IndexBuffer> triangleIndexBuffer = IndexBuffer::Create(triangleIndices.data(), static_cast<uint32_t>(triangleIndices.size()));
	s_data->TriangleVertexArray->SetIndexBuffer(triangleIndexBuffer);
	
	s_data->CameraUniformBuffer = UniformBuffer::Create("CameraBuffer", &s_data->UniformCamera, sizeof(Renderer2DData::UniformCamera), BufferUsage::Stream);
	s_data->DataUniformBuffer = UniformBuffer::Create("DataBuffer", &s_data->UniformData, sizeof(Renderer2DData::UniformData), BufferUsage::Dynamic);

	ShaderManager::Load("Renderer2D", Embed::Renderer2DVS, Embed::Renderer2DFS);
	const Scope<Image> whiteImage = Image::LoadFromMemory(1, 1, 32, Image::ColorFormat::RGBA, std::vector<uint8_t>{255, 255, 255, 255});
	TextureManager::Load("Renderer2DWhite", whiteImage);
	
	TextureManager::Get2D("Fallback2D")->Bind(0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::Shutdown()
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG(Log::Renderer2DPrefix, "Shutting down");
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

void TRAP::Graphics::Renderer2D::DrawTriangle(const Transform& transform, const Math::Vec4& color)
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

	DrawTriangle(transform);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawTriangle(const Transform& transform, const Scope<Texture2D>& texture)
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

	DrawTriangle(transform);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawTriangle(const Transform& transform, const Math::Vec4& color, const Scope<Texture2D>& texture)
{
	TP_PROFILE_FUNCTION();

	//Bind and Update DataUniformBuffer if color changed
	s_data->DataUniformBuffer->Bind(1);
	if (s_data->UniformData.Color != color)
	{
		s_data->UniformData.Color = color;
		s_data->DataUniformBuffer->UpdateData(&s_data->UniformData);
	}

	DrawTriangle(transform, texture);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform)
{
	TP_PROFILE_FUNCTION();

	//Update CameraUniformBuffer
	//Position & Size & Rotation
	s_data->UniformCamera.ModelMatrix = Translate(transform.Position) * Mat4Cast(Math::Quaternion(Radians(transform.Rotation))) *
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

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawTriangle(const Transform& transform)
{
	TP_PROFILE_FUNCTION();

	//Update CameraUniformBuffer
	//Position & Size & Rotation
	s_data->UniformCamera.ModelMatrix = Translate(transform.Position) * Mat4Cast(Math::Quaternion(Radians(transform.Rotation))) *
		Scale(transform.Scale);
	s_data->CameraUniformBuffer->UpdateData(&s_data->UniformCamera);
	s_data->CameraUniformBuffer->Bind(0);

	//Bind Shader
	ShaderManager::Get("Renderer2D")->Bind();

	//Bind Vertex Array
	s_data->TriangleVertexArray->Bind();

	//Render the Quad
	RenderCommand::DrawIndexed(s_data->TriangleVertexArray);
}