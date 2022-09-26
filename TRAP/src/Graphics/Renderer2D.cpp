#include "TRAPPCH.h"
#include "Renderer2D.h"

#include "Shaders/Shader.h"
#include "Buffers/UniformBuffer.h"
#include "RenderCommand.h"
#include "Application.h"
#include "Embed.h"
#include "API/Objects/Sampler.h"
#include "Buffers/IndexBuffer.h"
#include "Buffers/VertexBuffer.h"
#include "Cameras/Camera.h"
#include "Shaders/ShaderManager.h"
#include "Textures/TextureManager.h"
#include "Graphics/Cameras/Editor/EditorCamera.h"
#include "Scene/Components.h"

namespace TRAP::Graphics
{
	struct QuadVertex
	{
		Math::Vec3 Position;
		Math::Vec4 Color;
		Math::Vec2 TexCoord;
		float TexIndex;

		//Editor only
		int32_t EntityID;
	};

	struct CircleVertex
	{
		Math::Vec3 WorldPosition;
		Math::Vec3 LocalPosition;
		Math::Vec4 Color;
		float Thickness;
		float Fade;

		//Editor only
		int32_t EntityID;
	};

	struct LineVertex
	{
		Math::Vec3 Position;
		Math::Vec4 Color;

		//Editor only
		int32_t EntityID;
	};

	struct Renderer2DData
	{
		static constexpr uint32_t MaxQuads = 100000;
		static constexpr uint32_t MaxQuadVertices = MaxQuads * 4;
		static constexpr uint32_t MaxQuadIndices = MaxQuads * 6;
		static constexpr uint32_t MaxCircles = 100000;
		static constexpr uint32_t MaxCircleVertices = MaxCircles * 4;
		static constexpr uint32_t MaxCircleIndices = MaxCircles * 6;
		static constexpr uint32_t MaxLines = 100000;
		static constexpr uint32_t MaxLineVertices = MaxLines * 2;
		static constexpr uint32_t MaxTextureSlots = 32;

		struct QuadBuffers
		{
			Scope<VertexBuffer> QuadVertexBuffer = nullptr;
			std::vector<QuadVertex> QuadVertices = std::vector<QuadVertex>(MaxQuadVertices);
			std::vector<Ref<Texture>> TextureSlots = std::vector<Ref<Texture>>(MaxTextureSlots);
			uint32_t QuadCount = 0;
		};
		Scope<IndexBuffer> QuadIndexBuffer = nullptr;
		std::array<std::vector<QuadBuffers>, RendererAPI::ImageCount> QuadDataBuffers{};
		uint32_t QuadDataBufferIndex = 0;
		struct CircleBuffers
		{
			Scope<VertexBuffer> CircleVertexBuffer = nullptr;
			std::vector<CircleVertex> CircleVertices = std::vector<CircleVertex>(MaxQuadVertices);
			uint32_t CircleCount = 0;
		};
		Scope<IndexBuffer> CircleIndexBuffer = nullptr;
		std::array<std::vector<CircleBuffers>, RendererAPI::ImageCount> CircleDataBuffers{};
		uint32_t CircleDataBufferIndex = 0;
		struct LineBuffers
		{
			Scope<VertexBuffer> LineVertexBuffer = nullptr;
			std::vector<LineVertex> LineVertices = std::vector<LineVertex>(MaxLineVertices);
			uint32_t LineCount = 0;
		};
		std::array<std::vector<LineBuffers>, RendererAPI::ImageCount> LineDataBuffers{};
		uint32_t LineDataBufferIndex = 0;

		Scope<std::array<uint32_t, Renderer2DData::MaxQuadIndices>> QuadIndicesData = nullptr; //Used by ExtendBuffers()

		Ref<Shader> QuadShader;
		Ref<Shader> LineShader;
		Ref<Shader> CircleShader;
		Ref<Sampler> TextureSampler;
		Ref<Texture> WhiteTexture;

		QuadVertex* QuadVertexBufferPtr = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

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
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Renderer2DData TRAP::Graphics::Renderer2D::s_data{};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::Init()
{
	TP_PROFILE_FUNCTION();

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::Renderer2DPrefix, "Initializing");
#endif

	s_data.CameraUniformBuffer = UniformBuffer::Create(&s_data.UniformCamera,
	                                                   sizeof(Renderer2DData::UniformCamera),
		                                               UpdateFrequency::Dynamic);

	const Scope<Image> whiteImage = Image::LoadFromMemory(2, 2, Image::ColorFormat::RGBA,
	                                                      std::vector<uint8_t>{255, 255, 255, 255,
																			   255, 255, 255, 255,
																		       255, 255, 255, 255,
																			   255, 255, 255, 255 });
	s_data.WhiteTexture = Texture::CreateFromImage("Renderer2DWhite", whiteImage.get(), TextureType::Texture2D);

	const SamplerDesc samplerDesc{};
	s_data.TextureSampler = Sampler::Create(samplerDesc);

	InitQuads();
	InitCircles();
	InitLines();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::Shutdown()
{
	TP_PROFILE_FUNCTION();

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::Renderer2DPrefix, "Shutting down");
#endif

	s_data.QuadIndicesData.reset();
	s_data.CameraUniformBuffer.reset();
	s_data.TextureSampler.reset();
	s_data.CircleShader.reset();
	s_data.QuadShader.reset();
	s_data.LineShader.reset();
	s_data.WhiteTexture.reset();
	s_data.QuadDataBuffers = {};
	s_data.CircleDataBuffers = {};
	s_data.LineDataBuffers = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::BeginScene(const Camera& camera, const Math::Mat4& transform)
{
	TP_PROFILE_FUNCTION();

	s_data.UniformCamera.ProjectionMatrix = camera.GetProjectionMatrix();
	s_data.UniformCamera.ViewMatrix = Math::Inverse(transform);

	//Update Camera
	s_data.CameraUniformBuffer->SetData(&s_data.UniformCamera, sizeof(Renderer2DData::UniformCamera));
	s_data.CameraUniformBuffer->AwaitLoading();

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	//Reset Vertices & Indices
	s_data.QuadVertexBufferPtr = s_data.QuadDataBuffers[imageIndex][s_data.QuadDataBufferIndex].QuadVertices.data();
	s_data.CircleVertexBufferPtr = s_data.CircleDataBuffers[imageIndex][s_data.CircleDataBufferIndex].CircleVertices.data();
	s_data.LineVertexBufferPtr = s_data.LineDataBuffers[imageIndex][s_data.LineDataBufferIndex].LineVertices.data();

	for(auto& buffers : s_data.QuadDataBuffers[imageIndex])
		std::fill(buffers.TextureSlots.begin(), buffers.TextureSlots.end(), s_data.WhiteTexture);
	s_data.TextureSlotIndex = 1;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::BeginScene(const OrthographicCamera& camera)
{
	TP_PROFILE_FUNCTION();

	s_data.UniformCamera.ProjectionMatrix = camera.GetProjectionMatrix();
	s_data.UniformCamera.ViewMatrix = camera.GetViewMatrix();

	//Update Camera
	s_data.CameraUniformBuffer->SetData(&s_data.UniformCamera, sizeof(Renderer2DData::UniformCamera));
	s_data.CameraUniformBuffer->AwaitLoading();

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	//Reset Vertices & Indices
	s_data.QuadVertexBufferPtr = s_data.QuadDataBuffers[imageIndex][s_data.QuadDataBufferIndex].QuadVertices.data();
	s_data.CircleVertexBufferPtr = s_data.CircleDataBuffers[imageIndex][s_data.CircleDataBufferIndex].CircleVertices.data();
	s_data.LineVertexBufferPtr = s_data.LineDataBuffers[imageIndex][s_data.LineDataBufferIndex].LineVertices.data();

	//Reset textures
	for(auto& buffers : s_data.QuadDataBuffers[imageIndex])
		std::fill(buffers.TextureSlots.begin(), buffers.TextureSlots.end(), s_data.WhiteTexture);
	s_data.TextureSlotIndex = 1;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::BeginScene(const EditorCamera& camera)
{
	TP_PROFILE_FUNCTION();

	s_data.UniformCamera.ProjectionMatrix = camera.GetProjectionMatrix();
	s_data.UniformCamera.ViewMatrix = camera.GetViewMatrix();

	//Update Camera
	s_data.CameraUniformBuffer->SetData(&s_data.UniformCamera, sizeof(Renderer2DData::UniformCamera));
	s_data.CameraUniformBuffer->AwaitLoading();

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	//Reset Vertices & Indices
	s_data.QuadVertexBufferPtr = s_data.QuadDataBuffers[imageIndex][s_data.QuadDataBufferIndex].QuadVertices.data();
	s_data.CircleVertexBufferPtr = s_data.CircleDataBuffers[imageIndex][s_data.CircleDataBufferIndex].CircleVertices.data();
	s_data.LineVertexBufferPtr = s_data.LineDataBuffers[imageIndex][s_data.LineDataBufferIndex].LineVertices.data();

	//Reset textures
	for(auto& buffers : s_data.QuadDataBuffers[imageIndex])
		std::fill(buffers.TextureSlots.begin(), buffers.TextureSlots.end(), s_data.WhiteTexture);
	s_data.TextureSlotIndex = 1;
}

//-------------------------------------------------------------------------------------------------------------------//

void DrawQuadBuffer(TRAP::Graphics::Renderer2DData::QuadBuffers& buffers, TRAP::Graphics::Renderer2DData& renderer2DData)
{
	const uint32_t verticesSize = buffers.QuadCount * sizeof(TRAP::Graphics::QuadVertex) * 4;
	const uint32_t indicesCount = buffers.QuadCount * 6;

	//Update Vertices
	buffers.QuadVertexBuffer->SetData(reinterpret_cast<float*>(buffers.QuadVertices.data()), verticesSize);

	//Use dynamic shader resources
	renderer2DData.QuadShader->UseTextures(1, 1, buffers.TextureSlots);
	renderer2DData.QuadShader->UseUBO(1, 0, renderer2DData.CameraUniformBuffer.get());
	renderer2DData.QuadShader->UseSampler(0, 1, renderer2DData.TextureSampler.get());

	//Use Vertex & Index Buffer
	buffers.QuadVertexBuffer->Use();
	renderer2DData.QuadIndexBuffer->Use();
	// buffers.QuadIndexBuffer->Use();

	//Use Shader
	renderer2DData.QuadShader->Use();

	buffers.QuadVertexBuffer->AwaitLoading();
	TRAP::Graphics::RenderCommand::DrawIndexed(indicesCount);

	buffers.QuadCount = 0;

	renderer2DData.Stats.DrawCalls++;
}

//-------------------------------------------------------------------------------------------------------------------//

void DrawCircleBuffer(TRAP::Graphics::Renderer2DData::CircleBuffers& buffers, TRAP::Graphics::Renderer2DData& renderer2DData)
{
	const uint32_t verticesSize = buffers.CircleCount * sizeof(TRAP::Graphics::CircleVertex) * 4;
	const uint32_t indicesCount = buffers.CircleCount * 6;

	//Update Vertices
	buffers.CircleVertexBuffer->SetData(reinterpret_cast<float*>(buffers.CircleVertices.data()), verticesSize);

	//Use dynamic shader resources
	renderer2DData.CircleShader->UseUBO(1, 0, renderer2DData.CameraUniformBuffer.get());

	//Use Vertex & Index Buffer
	buffers.CircleVertexBuffer->Use();
	renderer2DData.CircleIndexBuffer->Use();

	//Use Shader
	renderer2DData.CircleShader->Use();

	buffers.CircleVertexBuffer->AwaitLoading();
	TRAP::Graphics::RenderCommand::DrawIndexed(indicesCount);

	buffers.CircleCount = 0;

	renderer2DData.Stats.DrawCalls++;
}

//-------------------------------------------------------------------------------------------------------------------//

void DrawLineBuffer(TRAP::Graphics::Renderer2DData::LineBuffers& buffers, TRAP::Graphics::Renderer2DData& renderer2DData)
{
	const uint32_t verticesSize = buffers.LineCount * sizeof(TRAP::Graphics::LineVertex) * 2;
	// const uint32_t verticesSize = buffers.LineCount * sizeof(TRAP::Graphics::LineVertex) * sizeof(float);

	TRAP::Graphics::RenderCommand::SetPrimitiveTopology(TRAP::Graphics::PrimitiveTopology::LineList);

	//Update Vertices
	buffers.LineVertexBuffer->SetData(reinterpret_cast<float*>(buffers.LineVertices.data()), verticesSize);

	//Use dynamic shader resources
	renderer2DData.LineShader->UseUBO(1, 0, renderer2DData.CameraUniformBuffer.get());

	//Use Vertex Buffer
	buffers.LineVertexBuffer->Use();

	//Use Shader
	renderer2DData.LineShader->Use();

	buffers.LineVertexBuffer->AwaitLoading();
	TRAP::Graphics::RenderCommand::Draw(buffers.LineCount * 2);

	buffers.LineCount = 0;

	TRAP::Graphics::RenderCommand::SetPrimitiveTopology(TRAP::Graphics::PrimitiveTopology::TriangleList);

	renderer2DData.Stats.DrawCalls++;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::EndScene()
{
	TP_PROFILE_FUNCTION();

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	for(auto& buffers : s_data.QuadDataBuffers[imageIndex])
	{
		if(buffers.QuadCount != 0)
			DrawQuadBuffer(buffers, s_data);
	}

	for(auto& buffers : s_data.CircleDataBuffers[imageIndex])
	{
		if(buffers.CircleCount != 0)
			DrawCircleBuffer(buffers, s_data);
	}

	for(auto& buffers : s_data.LineDataBuffers[imageIndex])
	{
		if(buffers.LineCount != 0)
			DrawLineBuffer(buffers, s_data);
	}

	s_data.QuadDataBufferIndex = 0;
	s_data.CircleDataBufferIndex = 0;
	s_data.LineDataBufferIndex = 0;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const Math::Vec4& color)
{
	DrawQuad(transform, color, s_data.WhiteTexture);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, Ref<Texture> texture)
{
	if(texture->GetType() != TextureType::Texture2D)
		return;

	DrawQuad(transform, Math::Vec4(1.0f), std::move(texture));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const Math::Vec4& color,
                                          Ref<Texture> texture)
{
	TP_PROFILE_FUNCTION();

	if(texture->GetType() != TextureType::Texture2D)
		return;

	Math::Mat4 transformation;
	if (transform.Rotation.x != 0.0f || transform.Rotation.y != 0.0f || transform.Rotation.z != 0.0f)
		transformation = Math::Translate(transform.Position) *
		                 Mat4Cast(Math::Quat(Radians(transform.Rotation))) * Math::Scale(transform.Scale);
	else
		transformation = Math::Translate(transform.Position) * Math::Scale(transform.Scale);

	DrawQuad(transformation, color, std::move(texture), nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const TRAP::Ref<SubTexture2D> texture)
{
	if(!texture || texture->GetTexture()->GetType() != TextureType::Texture2D)
		return;

	DrawQuad(transform, Math::Vec4(1.0f), std::move(texture));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const Math::Vec4& color,
                                          const TRAP::Ref<SubTexture2D> texture)
{
	TP_PROFILE_FUNCTION();

	if(!texture || texture->GetTexture()->GetType() != TextureType::Texture2D)
		return;

	Math::Mat4 transformation;
	if (transform.Rotation.x != 0.0f || transform.Rotation.y != 0.0f || transform.Rotation.z != 0.0f)
		transformation = Math::Translate(transform.Position) *
		                 Mat4Cast(Math::Quat(Radians(transform.Rotation))) * Math::Scale(transform.Scale);
	else
		transformation = Math::Translate(transform.Position) * Math::Scale(transform.Scale);

	DrawQuad(transformation, color, texture->GetTexture(), &texture->GetTexCoords());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Math::Mat4& transform, const Math::Vec4& color,
                                          Ref<Texture> texture, const std::array<Math::Vec2, 4>* const texCoords,
										  const int32_t entityID)
{
	constexpr uint64_t quadVertexCount = 4;
	constexpr std::array<Math::Vec2, 4> textureCoords = { {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}} };

	if(texture && texture->GetType() != TextureType::Texture2D)
		return;

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	if (s_data.QuadDataBuffers[imageIndex][s_data.QuadDataBufferIndex].QuadCount * 6 >= Renderer2DData::MaxQuadIndices ||
	    s_data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
	{
		ExtendQuadBuffers();
		ResetQuad();
	}

	Ref<Texture> tex = texture ? texture : s_data.WhiteTexture;
	const float textureIndex = GetTextureIndex(std::move(tex));

	for (uint64_t i = 0; i < quadVertexCount; i++)
	{
		s_data.QuadVertexBufferPtr->Position = Math::Vec3(transform * s_data.QuadVertexPositions[i]);
		s_data.QuadVertexBufferPtr->Color = color;
		s_data.QuadVertexBufferPtr->TexCoord = texCoords ? (*texCoords)[i] : textureCoords[i];
		s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_data.QuadVertexBufferPtr->EntityID = entityID;
		s_data.QuadVertexBufferPtr++;
	}

	s_data.QuadDataBuffers[imageIndex][s_data.QuadDataBufferIndex].QuadCount++;

	s_data.Stats.QuadCount++;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawCircle(const Math::Mat4& transform, const Math::Vec4& color,
                                            const float thickness, const float fade, const int32_t entityID)
{
	constexpr uint64_t circleVertexCount = 4;

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	if (s_data.CircleDataBuffers[imageIndex][s_data.CircleDataBufferIndex].CircleCount * 6 >= Renderer2DData::MaxCircleIndices)
	{
		ExtendCircleBuffers();
		ResetCircle();
	}

	for (uint64_t i = 0; i < circleVertexCount; i++)
	{
		s_data.CircleVertexBufferPtr->WorldPosition = Math::Vec3(transform * s_data.QuadVertexPositions[i]);
		s_data.CircleVertexBufferPtr->LocalPosition = Math::Vec3(s_data.QuadVertexPositions[i] * 2.0f); //Range [-1, 1]
		s_data.CircleVertexBufferPtr->Color = color;
		s_data.CircleVertexBufferPtr->Thickness = thickness;
		s_data.CircleVertexBufferPtr->Fade = fade;
		s_data.CircleVertexBufferPtr->EntityID = entityID;
		s_data.CircleVertexBufferPtr++;
	}

	s_data.CircleDataBuffers[imageIndex][s_data.CircleDataBufferIndex].CircleCount++;

	s_data.Stats.CircleCount++;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawSprite(const TRAP::Math::Mat4& transform,
                                            const TRAP::SpriteRendererComponent& sprite, const int32_t entityID)
{
	DrawQuad(transform, sprite.Color, nullptr, nullptr, entityID);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawLine(const TRAP::Math::Vec3& p0, const TRAP::Math::Vec3& p1,
                                          const TRAP::Math::Vec4& color, const int32_t entityID)
{
	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	if (s_data.LineDataBuffers[imageIndex][s_data.LineDataBufferIndex].LineCount * 2 >= Renderer2DData::MaxLineVertices)
	{
		ExtendLineBuffers();
		ResetLine();
	}

	s_data.LineVertexBufferPtr->Position = p0;
	s_data.LineVertexBufferPtr->Color = color;
	s_data.LineVertexBufferPtr->EntityID = entityID;
	s_data.LineVertexBufferPtr++;

	s_data.LineVertexBufferPtr->Position = p1;
	s_data.LineVertexBufferPtr->Color = color;
	s_data.LineVertexBufferPtr->EntityID = entityID;
	s_data.LineVertexBufferPtr++;

	s_data.LineDataBuffers[imageIndex][s_data.LineDataBufferIndex].LineCount++;

	s_data.Stats.LineCount++;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawRect(const TRAP::Math::Vec3& position, const TRAP::Math::Vec2& size,
                                          const TRAP::Math::Vec4& color, const int32_t entityID)
{
	const TRAP::Math::Vec3 p0 = position + TRAP::Math::Vec3(-size.x / 2.0f, -size.y / 2.0f, 0.0f);
	const TRAP::Math::Vec3 p1 = position + TRAP::Math::Vec3( size.x / 2.0f, -size.y / 2.0f, 0.0f);
	const TRAP::Math::Vec3 p2 = position + TRAP::Math::Vec3( size.x / 2.0f,  size.y / 2.0f, 0.0f);
	const TRAP::Math::Vec3 p3 = position + TRAP::Math::Vec3(-size.x / 2.0f,  size.y / 2.0f, 0.0f);

	DrawLine(p0, p1, color, entityID);
	DrawLine(p1, p2, color, entityID);
	DrawLine(p2, p3, color, entityID);
	DrawLine(p3, p0, color, entityID);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawRect(const TRAP::Math::Mat4& transform, const TRAP::Math::Vec4& color,
                                          const int32_t entityID)
{
	std::array<TRAP::Math::Vec3, 4> lineVertices{};
	for(std::size_t i = 0; i < lineVertices.size(); ++i)
		lineVertices[i] = Math::Vec3(transform * s_data.QuadVertexPositions[i]);

	DrawLine(lineVertices[0], lineVertices[1], color, entityID);
	DrawLine(lineVertices[1], lineVertices[2], color, entityID);
	DrawLine(lineVertices[2], lineVertices[3], color, entityID);
	DrawLine(lineVertices[3], lineVertices[0], color, entityID);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Renderer2D::Statistics::GetTotalVertexCount() const
{
	return (QuadCount * 4) + (CircleCount * 4) + (LineCount * 2);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Renderer2D::Statistics::GetTotalIndexCount() const
{
	return (QuadCount * 6) + (CircleCount * 6);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Renderer2D::Statistics TRAP::Graphics::Renderer2D::GetStats()
{
	return s_data.Stats;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::ResetStats()
{
	s_data.Stats = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::InitQuads()
{
	s_data.QuadShader = Shader::CreateFromSource("Renderer2D_Quad", std::string(Embed::Renderer2DQuadShader));

	//Initialize quad indices
	s_data.QuadIndicesData = MakeScope<std::array<uint32_t, Renderer2DData::MaxQuadIndices>>();
	for(uint32_t offset = 0, i = 0; i < Renderer2DData::MaxQuadIndices; i += 6)
	{
		(*s_data.QuadIndicesData)[i + 0] = offset + 0;
		(*s_data.QuadIndicesData)[i + 1] = offset + 1;
		(*s_data.QuadIndicesData)[i + 2] = offset + 2;

		(*s_data.QuadIndicesData)[i + 3] = offset + 2;
		(*s_data.QuadIndicesData)[i + 4] = offset + 3;
		(*s_data.QuadIndicesData)[i + 5] = offset + 0;

		offset += 4;
	}

	//Initialize quad vertices
	s_data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	s_data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
	s_data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
	s_data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

	s_data.QuadIndexBuffer = IndexBuffer::Create((*s_data.QuadIndicesData).data(),
												 Renderer2DData::MaxQuadIndices * sizeof(uint32_t),
												 UpdateFrequency::Dynamic);
	s_data.QuadIndexBuffer->AwaitLoading();

	//Create and fill quad index and vertex buffers
	for(auto& buffers : s_data.QuadDataBuffers)
	{
		buffers.emplace_back();

		buffers[s_data.QuadDataBufferIndex].QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxQuadVertices * sizeof(QuadVertex),
																						UpdateFrequency::Dynamic);
		buffers[s_data.QuadDataBufferIndex].QuadVertexBuffer->SetLayout
		({
			{ ShaderDataType::Float3, "Position" },
			{ ShaderDataType::Float4, "Color" },
			{ ShaderDataType::Float2, "TexCoord" },
			{ ShaderDataType::Float, "TexIndex" },
			{ ShaderDataType::Int, "EntityID" }
		});

		std::fill(buffers[s_data.QuadDataBufferIndex].TextureSlots.begin(),
	              buffers[s_data.QuadDataBufferIndex].TextureSlots.end(), s_data.WhiteTexture);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::InitCircles()
{
	s_data.CircleShader = Shader::CreateFromSource("Renderer2D_Circle", std::string(Embed::Renderer2DCircleShader));

	//This uses the same indices as for quads
	s_data.CircleIndexBuffer = IndexBuffer::Create((*s_data.QuadIndicesData).data(),
												   Renderer2DData::MaxCircleIndices * sizeof(uint32_t),
												   UpdateFrequency::Dynamic);
	s_data.CircleIndexBuffer->AwaitLoading();

	//Create and fill circle index and vertex buffers
	for(auto& buffers : s_data.CircleDataBuffers)
	{
		buffers.emplace_back();

		buffers[s_data.CircleDataBufferIndex].CircleVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxCircleVertices * sizeof(CircleVertex),
																						UpdateFrequency::Dynamic);
		buffers[s_data.CircleDataBufferIndex].CircleVertexBuffer->SetLayout
		({
			{ ShaderDataType::Float3, "WorldPosition" },
			{ ShaderDataType::Float3, "LocalPosition" },
			{ ShaderDataType::Float4, "Color" },
			{ ShaderDataType::Float, "Thickness" },
			{ ShaderDataType::Float, "Fade" },
			{ ShaderDataType::Int, "EntityID" }
		});
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::InitLines()
{
	s_data.LineShader = Shader::CreateFromSource("Renderer2D_Line", std::string(Embed::Renderer2DLineShader));

	//Create and fill line vertex buffer
	for(auto& buffers : s_data.LineDataBuffers)
	{
		buffers.emplace_back();

		buffers[s_data.LineDataBufferIndex].LineVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxLineVertices * sizeof(LineVertex),
																				UpdateFrequency::Dynamic);
		buffers[s_data.LineDataBufferIndex].LineVertexBuffer->SetLayout
		({
			{ ShaderDataType::Float3, "Position" },
			{ ShaderDataType::Float4, "Color" },
			{ ShaderDataType::Int, "EntityID" }
		});
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::ResetQuad()
{
	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	s_data.QuadVertexBufferPtr = s_data.QuadDataBuffers[imageIndex][s_data.QuadDataBufferIndex].QuadVertices.data();

	s_data.TextureSlotIndex = 1;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::ResetCircle()
{
	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	s_data.CircleVertexBufferPtr = s_data.CircleDataBuffers[imageIndex][s_data.CircleDataBufferIndex].CircleVertices.data();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::ResetLine()
{
	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	s_data.LineVertexBufferPtr = s_data.LineDataBuffers[imageIndex][s_data.LineDataBufferIndex].LineVertices.data();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::Renderer2D::GetTextureIndex(Ref<Texture> texture)
{
	TRAP_ASSERT(texture, "Texture is nullptr!");
	TRAP_ASSERT(texture->GetType() == TextureType::Texture2D, "Texture is not a Texture2D!");

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	const auto res = std::find(s_data.QuadDataBuffers[imageIndex][s_data.QuadDataBufferIndex].TextureSlots.begin(),
	                           s_data.QuadDataBuffers[imageIndex][s_data.QuadDataBufferIndex].TextureSlots.end(), texture);

	if(res == s_data.QuadDataBuffers[imageIndex][s_data.QuadDataBufferIndex].TextureSlots.end())
	{
		s_data.QuadDataBuffers[imageIndex][s_data.QuadDataBufferIndex].TextureSlots[s_data.TextureSlotIndex++] = texture;
		return s_data.TextureSlotIndex - 1;
	}

	return std::distance(s_data.QuadDataBuffers[imageIndex][s_data.QuadDataBufferIndex].TextureSlots.begin(), res);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::ExtendQuadBuffers()
{
	s_data.QuadDataBufferIndex++;

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	if(s_data.QuadDataBufferIndex < s_data.QuadDataBuffers[imageIndex].size()) //Already allocated
		return;

	s_data.QuadDataBuffers[imageIndex].emplace_back();

	auto& buffers = s_data.QuadDataBuffers[imageIndex][s_data.QuadDataBufferIndex];

	buffers.QuadVertexBuffer = TRAP::Graphics::VertexBuffer::Create(TRAP::Graphics::Renderer2DData::MaxQuadVertices * sizeof(TRAP::Graphics::QuadVertex),
	                                                                TRAP::Graphics::UpdateFrequency::Dynamic);
	buffers.QuadVertexBuffer->SetLayout
	({
		{ TRAP::Graphics::ShaderDataType::Float3, "Position" },
		{ TRAP::Graphics::ShaderDataType::Float4, "Color" },
		{ TRAP::Graphics::ShaderDataType::Float2, "TexCoord" },
		{ TRAP::Graphics::ShaderDataType::Float, "TexIndex" },
		{ TRAP::Graphics::ShaderDataType::Int, "EntityID" }
	});

	std::fill(s_data.QuadDataBuffers[imageIndex][s_data.QuadDataBufferIndex].TextureSlots.begin(),
	          s_data.QuadDataBuffers[imageIndex][s_data.QuadDataBufferIndex].TextureSlots.end(), s_data.WhiteTexture);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::ExtendCircleBuffers()
{
	s_data.CircleDataBufferIndex++;

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	if(s_data.CircleDataBufferIndex < s_data.CircleDataBuffers[imageIndex].size()) //Already allocated
		return;

	s_data.CircleDataBuffers[imageIndex].emplace_back();

	auto& buffers = s_data.CircleDataBuffers[imageIndex][s_data.CircleDataBufferIndex];

	buffers.CircleVertexBuffer = TRAP::Graphics::VertexBuffer::Create(TRAP::Graphics::Renderer2DData::MaxCircleVertices * sizeof(TRAP::Graphics::CircleVertex),
	                                                                  TRAP::Graphics::UpdateFrequency::Dynamic);
	buffers.CircleVertexBuffer->SetLayout
	({
		{ TRAP::Graphics::ShaderDataType::Float3, "WorldPosition" },
		{ TRAP::Graphics::ShaderDataType::Float3, "LocalPosition" },
		{ TRAP::Graphics::ShaderDataType::Float4, "Color" },
		{ TRAP::Graphics::ShaderDataType::Float, "Thickness" },
		{ TRAP::Graphics::ShaderDataType::Float, "Fade" },
		{ TRAP::Graphics::ShaderDataType::Int, "EntityID" }
	});
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::ExtendLineBuffers()
{
	s_data.LineDataBufferIndex++;

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	if(s_data.LineDataBufferIndex < s_data.LineDataBuffers[imageIndex].size()) //Already allocated
		return;

	s_data.LineDataBuffers[imageIndex].emplace_back();

	auto& buffers = s_data.LineDataBuffers[imageIndex][s_data.LineDataBufferIndex];

	buffers.LineVertexBuffer = TRAP::Graphics::VertexBuffer::Create(TRAP::Graphics::Renderer2DData::MaxLineVertices * sizeof(TRAP::Graphics::LineVertex),
	                                                                TRAP::Graphics::UpdateFrequency::Dynamic);
	buffers.LineVertexBuffer->SetLayout
	({
		{ TRAP::Graphics::ShaderDataType::Float3, "Position" },
		{ TRAP::Graphics::ShaderDataType::Float4, "Color" },
		{ TRAP::Graphics::ShaderDataType::Int, "EntityID" }
	});
}
