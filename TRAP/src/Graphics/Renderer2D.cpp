#include "TRAPPCH.h"
#include "Renderer2D.h"

#include "Buffers/VertexArray.h"
#include "Shaders/Shader.h"
#include "Buffers/UniformBuffer.h"
#include "RenderCommand.h"
#include "Application.h"
#include "Embed.h"
#include "Cameras/Camera.h"
#include "Textures/TextureManager.h"

namespace TRAP::Graphics
{
	//TODO Maybe multi thread transformation calculation and insertion of vertex data with own ThreadPool
	struct QuadVertex
	{
		Math::Vec3 Position;
		Math::Vec4 Color;
		Math::Vec2 TexCoord;
		float TexIndex;
	};
	
	struct Renderer2DData
	{
		static constexpr uint32_t MaxQuads = 20000;
		static constexpr uint32_t MaxVertices = MaxQuads * 4;
		static constexpr uint32_t MaxIndices = MaxQuads * 6;
		static constexpr uint32_t MaxTextureSlots = 32; //TODO: RenderCaps (OpenGL already has GetMaxTextureUnits())
		
		Scope<VertexArray> QuadVertexArray;
		Scope<VertexBuffer> QuadVertexBuffer;
		Scope<Shader> TextureShader;
		Scope<Texture2D> WhiteTexture;

		Scope<std::array<QuadVertex, MaxVertices>> QuadVertices = MakeScope<std::array<QuadVertex, MaxVertices>>();
		
		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Texture2D*, MaxTextureSlots> TextureSlots{};
		uint32_t TextureSlotIndex = 1; //0 = White texture

		std::array<Math::Vec4, 4> QuadVertexPositions{};

		Renderer2D::Statistics Stats;
		
		Scope<UniformBuffer> CameraUniformBuffer;
		struct UniformCamera
		{
			Math::Mat4 ProjectionMatrix{ 1.0f };
			Math::Mat4 ViewMatrix{ 1.0f };
		} UniformCamera;
	};

	static Renderer2DData s_data;
}

void TRAP::Graphics::Renderer2D::Init()
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG(Log::Renderer2DPrefix, "Initializing");
	
	s_data.QuadVertexArray = VertexArray::Create();
	s_data.QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(QuadVertex));
	s_data.QuadVertexBuffer->SetLayout
	({
		{ ShaderDataType::Float3, "Position" },
		{ ShaderDataType::Float4, "Color" },
		{ ShaderDataType::Float2, "TexCoord" },
		{ ShaderDataType::Float, "TexIndex" }
	});
	s_data.QuadVertexArray->SetVertexBuffer(s_data.QuadVertexBuffer);

	s_data.QuadVertexBufferBase = (*s_data.QuadVertices).data();
	
	Scope<std::array<uint32_t, Renderer2DData::MaxIndices>> quadIndices = MakeScope<std::array<uint32_t, Renderer2DData::MaxIndices>>();
	for(uint32_t offset = 0, i = 0; i < Renderer2DData::MaxIndices; i += 6)
	{
		(*quadIndices)[i + 0] = offset + 0;
		(*quadIndices)[i + 1] = offset + 1;
		(*quadIndices)[i + 2] = offset + 2;
		
		(*quadIndices)[i + 3] = offset + 2;
		(*quadIndices)[i + 4] = offset + 3;
		(*quadIndices)[i + 5] = offset + 0;

		offset += 4;
	}

	Scope<IndexBuffer> quadIndexBuffer = IndexBuffer::Create((*quadIndices).data(), Renderer2DData::MaxIndices);
	s_data.QuadVertexArray->SetIndexBuffer(quadIndexBuffer);
	quadIndices.reset();
	
	s_data.CameraUniformBuffer = UniformBuffer::Create("CameraBuffer", &s_data.UniformCamera, sizeof(Renderer2DData::UniformCamera), BufferUsage::Stream);

	s_data.TextureShader = Shader::CreateFromSource("Renderer2D", Embed::Renderer2DVS, Embed::Renderer2DFS);
	const Scope<Image> whiteImage = Image::LoadFromMemory(1, 1, Image::ColorFormat::RGBA, std::vector<uint8_t>{255, 255, 255, 255});
	s_data.WhiteTexture = Texture2D::CreateFromImage("Renderer2DWhite", whiteImage);

	std::array<int32_t, Renderer2DData::MaxTextureSlots> samplers{};
	for (uint32_t i = 0; i < Renderer2DData::MaxTextureSlots; i++)
		samplers[i] = i;

	//Set first texture slot to 0
	s_data.TextureSlots[0] = s_data.WhiteTexture.get();

	s_data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	s_data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
	s_data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
	s_data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

	TextureManager::Get2D("Fallback2D")->Bind(0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::Shutdown()
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG(Log::Renderer2DPrefix, "Shutting down");
	s_data.TextureShader.reset();
	s_data.WhiteTexture.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::BeginScene(const Camera& camera, const Math::Mat4& transform)
{
	TP_PROFILE_FUNCTION();

	s_data.UniformCamera.ProjectionMatrix = camera.GetProjectionMatrix();
	s_data.UniformCamera.ViewMatrix = Math::Inverse(transform);

	//Bind Shader
	s_data.TextureShader->Bind();

	StartBatch();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::BeginScene(const OrthographicCamera& camera)
{
	TP_PROFILE_FUNCTION();

	s_data.UniformCamera.ProjectionMatrix = camera.GetProjectionMatrix();
	s_data.UniformCamera.ViewMatrix = camera.GetViewMatrix();

	//Bind Shader
	s_data.TextureShader->Bind();

	StartBatch();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::EndScene()
{
	TP_PROFILE_FUNCTION();

	Flush();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::Flush()
{
	if (s_data.QuadIndexCount == 0)
		return; //Nothing to render
	
	//Update Camera
	s_data.CameraUniformBuffer->UpdateData(&s_data.UniformCamera);
	s_data.CameraUniformBuffer->Bind(0);

	//Bind Vertex Array
	s_data.QuadVertexArray->Bind();

	const uint32_t dataSize = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(s_data.QuadVertexBufferPtr) -
		reinterpret_cast<uint8_t*>(s_data.QuadVertexBufferBase));
	s_data.QuadVertexArray->GetVertexBuffer()->SetData(s_data.QuadVertexBufferBase, dataSize);

	//Bind textures
	for (uint32_t i = 0; i < s_data.TextureSlotIndex; i++)
		s_data.TextureSlots[i]->Bind(i);
	
	RenderCommand::DrawIndexed(s_data.QuadVertexArray, s_data.QuadIndexCount);

	s_data.Stats.DrawCalls++;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::StartBatch()
{
	s_data.QuadIndexCount = 0;
	s_data.QuadVertexBufferPtr = s_data.QuadVertexBufferBase;

	s_data.TextureSlotIndex = 1;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::NextBatch()
{
	Flush();
	StartBatch();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const Math::Vec4& color)
{
	DrawQuad(transform, color, s_data.WhiteTexture);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const Scope<Texture2D>& texture)
{
	DrawQuad(transform, Math::Vec4(1.0f), texture);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const Math::Vec4& color, const Scope<Texture2D>& texture)
{
	TP_PROFILE_FUNCTION();

	Math::Mat4 transformation;
	if (transform.Rotation.x != 0.0f || transform.Rotation.y != 0.0f || transform.Rotation.z != 0.0f)
		transformation = Math::Translate(transform.Position) * Mat4Cast(Math::Quaternion(Radians(transform.Rotation))) * Math::Scale(transform.Scale);
	else
		transformation = Math::Translate(transform.Position) * Math::Scale(transform.Scale);

	DrawQuad(transformation, color, texture);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Math::Mat4& transform, const Math::Vec4& color, const Scope<Texture2D>& texture)
{
	constexpr uint64_t quadVertexCount = 4;
	constexpr std::array<Math::Vec2, 4> textureCoords = { {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}} };

	if (s_data.QuadIndexCount >= Renderer2DData::MaxIndices)
		NextBatch();

	const float textureIndex = GetTextureIndex(texture);

	for (uint64_t i = 0; i < quadVertexCount; i++)
	{
		s_data.QuadVertexBufferPtr->Position = Math::Vec3(transform * s_data.QuadVertexPositions[i]);
		s_data.QuadVertexBufferPtr->Color = color;
		s_data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_data.QuadVertexBufferPtr++;
	}

	s_data.QuadIndexCount += 6;

	s_data.Stats.QuadCount++;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::Renderer2D::GetTextureIndex(const Scope<Texture2D>& texture)
{
	float textureIndex = 0.0f;

	if (!texture)
		return textureIndex;
	
	for (uint32_t i = 1; i < s_data.TextureSlotIndex; i++)
	{
		if (s_data.TextureSlots[i] == texture.get())
		{
			textureIndex = static_cast<float>(i);
			break;
		}
	}

	if (textureIndex == 0.0f)
	{
		if (s_data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
			NextBatch();
		
		textureIndex = static_cast<float>(s_data.TextureSlotIndex);
		s_data.TextureSlots[s_data.TextureSlotIndex] = texture.get();
		s_data.TextureSlotIndex++;
	}

	return textureIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Renderer2D::Statistics::GetTotalVertexCount() const
{
	return QuadCount * 4;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Renderer2D::Statistics::GetTotalIndexCount() const
{
	return QuadCount * 6;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Renderer2D::Statistics TRAP::Graphics::Renderer2D::GetStats()
{
	return s_data.Stats;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::ResetStats()
{
	std::memset(&s_data.Stats, 0, sizeof(Statistics));
}