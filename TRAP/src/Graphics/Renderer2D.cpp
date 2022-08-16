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

namespace TRAP::Graphics
{
	struct QuadVertex
	{
		Math::Vec3 Position;
		Math::Vec4 Color;
		Math::Vec2 TexCoord;
		float TexIndex;
	};

	struct Renderer2DData
	{
		static constexpr uint32_t MaxQuads = 100000;
		static constexpr uint32_t MaxVertices = MaxQuads * 4;
		static constexpr uint32_t MaxIndices = MaxQuads * 6;
		static constexpr uint32_t MaxTextureSlots = 32;

		struct Buffers
		{
			Scope<VertexBuffer> QuadVertexBuffer = nullptr;
			Scope<IndexBuffer> QuadIndexBuffer = nullptr;
			std::vector<QuadVertex> QuadVertices = std::vector<QuadVertex>(MaxVertices);
			std::vector<Ref<Texture>> TextureSlots = std::vector<Ref<Texture>>(MaxTextureSlots);
			uint32_t QuadCount = 0;
		};
		std::array<std::vector<Buffers>, RendererAPI::ImageCount> DataBuffers{};
		uint32_t DataBufferIndex = 0;

		Scope<std::array<uint32_t, Renderer2DData::MaxIndices>> QuadIndicesData = nullptr; //Used by ExtendBuffers()

		Ref<Shader> TextureShader;
		Ref<Sampler> TextureSampler;
		Ref<Texture> WhiteTexture;

		QuadVertex* QuadVertexBufferPtr = nullptr;

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

	s_data.QuadIndicesData = MakeScope<std::array<uint32_t, Renderer2DData::MaxIndices>>();
	for(uint32_t offset = 0, i = 0; i < Renderer2DData::MaxIndices; i += 6)
	{
		(*s_data.QuadIndicesData)[i + 0] = offset + 0;
		(*s_data.QuadIndicesData)[i + 1] = offset + 1;
		(*s_data.QuadIndicesData)[i + 2] = offset + 2;

		(*s_data.QuadIndicesData)[i + 3] = offset + 2;
		(*s_data.QuadIndicesData)[i + 4] = offset + 3;
		(*s_data.QuadIndicesData)[i + 5] = offset + 0;

		offset += 4;
	}

	s_data.CameraUniformBuffer = UniformBuffer::Create(&s_data.UniformCamera,
	                                                   sizeof(Renderer2DData::UniformCamera),
		                                               UpdateFrequency::Dynamic);

	s_data.TextureShader = Shader::CreateFromSource("Renderer2D", std::string(Embed::Renderer2DShader));
	const Scope<Image> whiteImage = Image::LoadFromMemory(2, 2, Image::ColorFormat::RGBA,
	                                                      std::vector<uint8_t>{255, 255, 255, 255,
																			   255, 255, 255, 255,
																		       255, 255, 255, 255,
																			   255, 255, 255, 255 });
	s_data.WhiteTexture = Texture::CreateFromImage("Renderer2DWhite", whiteImage.get(), TextureType::Texture2D);

	const SamplerDesc samplerDesc{};
	s_data.TextureSampler = Sampler::Create(samplerDesc);

	s_data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	s_data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
	s_data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
	s_data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

	for(auto& buffers : s_data.DataBuffers)
	{
		buffers.emplace_back();

		buffers[s_data.DataBufferIndex].QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(QuadVertex),
																						UpdateFrequency::Dynamic);
		buffers[s_data.DataBufferIndex].QuadVertexBuffer->SetLayout
		({
			{ ShaderDataType::Float3, "Position" },
			{ ShaderDataType::Float4, "Color" },
			{ ShaderDataType::Float2, "TexCoord" },
			{ ShaderDataType::Float, "TexIndex" }
		});

		buffers[s_data.DataBufferIndex].QuadIndexBuffer = IndexBuffer::Create((*s_data.QuadIndicesData).data(),
																	Renderer2DData::MaxIndices * sizeof(uint32_t),
																	UpdateFrequency::Dynamic);
		buffers[s_data.DataBufferIndex].QuadIndexBuffer->AwaitLoading();

		std::fill(buffers[s_data.DataBufferIndex].TextureSlots.begin(),
	              buffers[s_data.DataBufferIndex].TextureSlots.end(), s_data.WhiteTexture);
	}
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
	s_data.TextureShader.reset();
	s_data.WhiteTexture.reset();
	s_data.DataBuffers = {};
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
	s_data.QuadVertexBufferPtr = s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].QuadVertices.data();

	for(auto& buffers : s_data.DataBuffers[imageIndex])
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
	s_data.QuadVertexBufferPtr = s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].QuadVertices.data();

	//Reset textures
	for(auto& buffers : s_data.DataBuffers[imageIndex])
		std::fill(buffers.TextureSlots.begin(), buffers.TextureSlots.end(), s_data.WhiteTexture);
	s_data.TextureSlotIndex = 1;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::EndScene()
{
	TP_PROFILE_FUNCTION();

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	for(auto& buffers : s_data.DataBuffers[imageIndex])
	{
		if(buffers.QuadCount == 0)
			continue;

		const uint32_t verticesSize = buffers.QuadCount * sizeof(QuadVertex) * sizeof(float);
		const uint32_t indicesCount = buffers.QuadCount * 6;

		//Update Vertices
		buffers.QuadVertexBuffer->SetData(reinterpret_cast<float*>(buffers.QuadVertices.data()), verticesSize);
		buffers.QuadVertexBuffer->AwaitLoading();

		//Use dynamic shader resources
		s_data.TextureShader->UseTextures(1, 1, buffers.TextureSlots);
		s_data.TextureShader->UseUBO(1, 0, s_data.CameraUniformBuffer.get());
		s_data.TextureShader->UseSampler(0, 1, s_data.TextureSampler.get());

		//Use Vertex & Index Buffer
		buffers.QuadVertexBuffer->Use();
		buffers.QuadIndexBuffer->Use();

		//Use Shader
		s_data.TextureShader->Use();

		RenderCommand::DrawIndexed(indicesCount);

		buffers.QuadCount = 0;

		s_data.Stats.DrawCalls++;
	}

	s_data.DataBufferIndex = 0;
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
	if(texture->GetTexture()->GetType() != TextureType::Texture2D)
		return;

	DrawQuad(transform, Math::Vec4(1.0f), std::move(texture));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const Math::Vec4& color,
                                          const TRAP::Ref<SubTexture2D> texture)
{
	TP_PROFILE_FUNCTION();

	if(texture->GetTexture()->GetType() != TextureType::Texture2D)
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
                                          Ref<Texture> texture, const std::array<Math::Vec2, 4>* texCoords)
{
	constexpr uint64_t quadVertexCount = 4;
	constexpr std::array<Math::Vec2, 4> textureCoords = { {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}} };

	if(texture && texture->GetType() != TextureType::Texture2D)
		return;

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	if (s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].QuadCount * 6 >= Renderer2DData::MaxIndices ||
	    s_data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
	{
		ExtendBuffers();
		Reset();
	}

	Ref<Texture> tex = texture ? texture : s_data.WhiteTexture;
	const float textureIndex = GetTextureIndex(std::move(tex));

	for (uint64_t i = 0; i < quadVertexCount; i++)
	{
		s_data.QuadVertexBufferPtr->Position = Math::Vec3(transform * s_data.QuadVertexPositions[i]);
		s_data.QuadVertexBufferPtr->Color = color;
		s_data.QuadVertexBufferPtr->TexCoord = texCoords ? (*texCoords)[i] : textureCoords[i];
		s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_data.QuadVertexBufferPtr++;
	}

	s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].QuadCount++;

	s_data.Stats.QuadCount++;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::Renderer2D::GetTextureIndex(Ref<Texture> texture)
{
	TRAP_ASSERT(texture, "Texture is nullptr!");
	TRAP_ASSERT(texture->GetType() == TextureType::Texture2D, "Texture is not a Texture2D!");

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	const auto res = std::find(s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].TextureSlots.begin(),
	                           s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].TextureSlots.end(), texture);

	if(res == s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].TextureSlots.end())
	{
		s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].TextureSlots[s_data.TextureSlotIndex++] = texture;
		return s_data.TextureSlotIndex - 1;
	}

	return std::distance(s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].TextureSlots.begin(), res);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::ExtendBuffers()
{
	s_data.DataBufferIndex++;

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	if(s_data.DataBufferIndex < s_data.DataBuffers[imageIndex].size()) //Already allocated
		return;

	s_data.DataBuffers[imageIndex].emplace_back();

	s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(QuadVertex),
	                                                                                   UpdateFrequency::Dynamic);
	s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].QuadVertexBuffer->SetLayout
	({
		{ ShaderDataType::Float3, "Position" },
		{ ShaderDataType::Float4, "Color" },
		{ ShaderDataType::Float2, "TexCoord" },
		{ ShaderDataType::Float, "TexIndex" }
	});

	s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].QuadIndexBuffer = IndexBuffer::Create((*s_data.QuadIndicesData).data(),
		                                                        Renderer2DData::MaxIndices * sizeof(uint32_t),
		                                                        UpdateFrequency::Dynamic);
	s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].QuadIndexBuffer->AwaitLoading();

	std::fill(s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].TextureSlots.begin(),
	          s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].TextureSlots.end(), s_data.WhiteTexture);
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
	s_data.Stats = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::Reset()
{
	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	s_data.QuadVertexBufferPtr = s_data.DataBuffers[imageIndex][s_data.DataBufferIndex].QuadVertices.data();

	s_data.TextureSlotIndex = 1;
}
