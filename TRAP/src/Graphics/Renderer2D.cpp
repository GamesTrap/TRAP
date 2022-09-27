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
	struct Renderer2DData
	{
		struct QuadData
		{
			static constexpr uint32_t MaxQuads = 100000;
			static constexpr uint32_t MaxQuadVertices = MaxQuads * 4;
			static constexpr uint32_t MaxQuadIndices = MaxQuads * 6;
			static constexpr uint32_t MaxTextureSlots = 32;

			struct QuadVertex
			{
				Math::Vec3 Position;
				Math::Vec4 Color;
				Math::Vec2 TexCoord;
				float TexIndex;

				//Editor only
				int32_t EntityID;
			};

			//Per draw call data
			struct QuadBuffers
			{
				Scope<Graphics::VertexBuffer> VertexBuffer = nullptr;
				std::vector<QuadVertex> Vertices = std::vector<QuadVertex>(MaxQuadVertices);
				std::vector<Ref<Texture>> TextureSlots = std::vector<Ref<Texture>>(MaxTextureSlots);
				uint32_t QuadCount = 0;
			};
			std::array<std::vector<QuadBuffers>, RendererAPI::ImageCount> DataBuffers{};
			uint32_t DataBufferIndex = 0;
			QuadVertex* VertexBufferPtr = nullptr;
			uint32_t TextureSlotIndex = 1; //0 = White texture

			Scope<Graphics::IndexBuffer> IndexBuffer = nullptr;

			Ref<Graphics::Shader> Shader;
			Ref<Sampler> TextureSampler;
			Ref<Texture> WhiteTexture;

			inline static constexpr std::array<Math::Vec4, 4> VertexPositions
			{
				{
					{ -0.5f, -0.5f, 0.0f, 1.0f },
					{  0.5f, -0.5f, 0.0f, 1.0f },
					{  0.5f,  0.5f, 0.0f, 1.0f },
					{ -0.5f,  0.5f, 0.0f, 1.0f }
				}
			};

			inline static const TRAP::Graphics::VertexBufferLayout VertexLayout
			{
				{ ShaderDataType::Float3, "Position" },
				{ ShaderDataType::Float4, "Color" },
				{ ShaderDataType::Float2, "TexCoord" },
				{ ShaderDataType::Float, "TexIndex" },
				{ ShaderDataType::Int, "EntityID" }
			};

			void Init();
			void Shutdown();
			void Reset();
			void ExtendBuffers(); //Extend buffers to allow for an additional draw call
			uint32_t DrawBuffers(UniformBuffer* camera);

			float GetTextureIndex(Ref<Texture> texture);
		} QuadData;

		//-------------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//

		struct CircleData
		{
			static constexpr uint32_t MaxCircles = 100000;
			static constexpr uint32_t MaxCircleVertices = MaxCircles * 4;
			static constexpr uint32_t MaxCircleIndices = MaxCircles * 6;

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

			//Per draw call data
			struct CircleBuffers
			{
				Scope<Graphics::VertexBuffer> VertexBuffer = nullptr;
				std::vector<CircleVertex> Vertices = std::vector<CircleVertex>(MaxCircleVertices);
				uint32_t CircleCount = 0;
			};
			std::array<std::vector<CircleBuffers>, RendererAPI::ImageCount> DataBuffers{};
			uint32_t DataBufferIndex = 0;
			CircleVertex* VertexBufferPtr = nullptr;

			Scope<Graphics::IndexBuffer> IndexBuffer = nullptr;

			Ref<Graphics::Shader> Shader;

			inline static const TRAP::Graphics::VertexBufferLayout VertexLayout
			{
				{ ShaderDataType::Float3, "WorldPosition" },
				{ ShaderDataType::Float3, "LocalPosition" },
				{ ShaderDataType::Float4, "Color" },
				{ ShaderDataType::Float, "Thickness" },
				{ ShaderDataType::Float, "Fade" },
				{ ShaderDataType::Int, "EntityID" }
			};

			void Init();
			void Shutdown();
			void Reset();
			void ExtendBuffers(); //Extend buffers to allow for an additional draw call
			uint32_t DrawBuffers(UniformBuffer* camera);
		} CircleData;

		//-------------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//

		struct LineData
		{
			static constexpr uint32_t MaxLines = 100000;
			static constexpr uint32_t MaxLineVertices = MaxLines * 2;

			struct LineVertex
			{
				Math::Vec3 Position;
				Math::Vec4 Color;

				//Editor only
				int32_t EntityID;
			};

			struct LineBuffers
			{
				Scope<Graphics::VertexBuffer> VertexBuffer = nullptr;
				std::vector<LineVertex> Vertices = std::vector<LineVertex>(MaxLineVertices);
				uint32_t LineCount = 0;
			};
			std::array<std::vector<LineBuffers>, RendererAPI::ImageCount> DataBuffers{};
			uint32_t DataBufferIndex = 0;
			LineVertex* VertexBufferPtr = nullptr;

			Ref<Graphics::Shader> Shader;

			inline static const TRAP::Graphics::VertexBufferLayout VertexLayout
			{
				{ ShaderDataType::Float3, "Position" },
				{ ShaderDataType::Float4, "Color" },
				{ ShaderDataType::Int, "EntityID" }
			};

			void Init();
			void Shutdown();
			void Reset();
			void ExtendBuffers(); //Extend buffers to allow for an additional draw call
			uint32_t DrawBuffers(UniformBuffer* camera);
		} LineData;

		//-------------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//

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
//QuadData-----------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::QuadData::Init()
{
	Shader = Shader::CreateFromSource("Renderer2D_Quad", std::string(Embed::Renderer2DQuadShader));

	const Scope<Image> whiteImage = Image::LoadFromMemory(2, 2, Image::ColorFormat::RGBA,
	                                                      std::vector<uint8_t>{255, 255, 255, 255,
																			   255, 255, 255, 255,
																		       255, 255, 255, 255,
																			   255, 255, 255, 255 });
	WhiteTexture = Texture::CreateFromImage("Renderer2DWhite", whiteImage.get(), TextureType::Texture2D);

	TextureSampler = Sampler::Create({});

	//Initialize quad indices
	auto IndicesData = MakeScope<std::array<uint32_t, MaxQuadIndices>>();
	for(uint32_t offset = 0, i = 0; i < MaxQuadIndices; i += 6)
	{
		(*IndicesData)[i + 0] = offset + 0;
		(*IndicesData)[i + 1] = offset + 1;
		(*IndicesData)[i + 2] = offset + 2;

		(*IndicesData)[i + 3] = offset + 2;
		(*IndicesData)[i + 4] = offset + 3;
		(*IndicesData)[i + 5] = offset + 0;

		offset += 4;
	}

	IndexBuffer = IndexBuffer::Create((*IndicesData).data(),
									  MaxQuadIndices * sizeof(uint32_t),
									  UpdateFrequency::Dynamic);
	IndexBuffer->AwaitLoading();
	IndicesData.reset();

	//Create and fill quad index and vertex buffers
	for(auto& buffers : DataBuffers)
	{
		buffers.emplace_back();

		buffers[DataBufferIndex].VertexBuffer = VertexBuffer::Create(MaxQuadVertices * sizeof(QuadVertex),
																	 UpdateFrequency::Dynamic);
		buffers[DataBufferIndex].VertexBuffer->SetLayout(VertexLayout);

		std::fill(buffers[DataBufferIndex].TextureSlots.begin(),
	              buffers[DataBufferIndex].TextureSlots.end(), WhiteTexture);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::QuadData::Shutdown()
{
	DataBuffers = {};
	IndexBuffer.reset();
	Shader.reset();
	TextureSampler.reset();
	WhiteTexture.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::QuadData::Reset()
{
	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	VertexBufferPtr = DataBuffers[imageIndex][DataBufferIndex].Vertices.data();

	TextureSlotIndex = 1;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::QuadData::ExtendBuffers()
{
	DataBufferIndex++;

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	if(DataBufferIndex < DataBuffers[imageIndex].size()) //Already allocated
		return;

	DataBuffers[imageIndex].emplace_back();

	auto& buffers = DataBuffers[imageIndex][DataBufferIndex];

	buffers.VertexBuffer = TRAP::Graphics::VertexBuffer::Create(MaxQuadVertices * sizeof(QuadVertex),
	                                                            TRAP::Graphics::UpdateFrequency::Dynamic);
	buffers.VertexBuffer->SetLayout(VertexLayout);

	std::fill(DataBuffers[imageIndex][DataBufferIndex].TextureSlots.begin(),
	          DataBuffers[imageIndex][DataBufferIndex].TextureSlots.end(), WhiteTexture);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Renderer2DData::QuadData::DrawBuffers(UniformBuffer* camera)
{
	TRAP_ASSERT(camera, "Invalid UniformBuffer!");

	uint32_t drawcalls = 0;

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	for(auto& buffers : DataBuffers[imageIndex])
	{
		if(buffers.QuadCount == 0)
			continue;

		const uint32_t verticesSize = buffers.QuadCount * sizeof(TRAP::Graphics::Renderer2DData::QuadData::QuadVertex) * 4;
		const uint32_t indicesCount = buffers.QuadCount * 6;

		//Update Vertices
		buffers.VertexBuffer->SetData(reinterpret_cast<float*>(buffers.Vertices.data()), verticesSize);

		//Use dynamic shader resources
		Shader->UseTextures(1, 1, buffers.TextureSlots);
		Shader->UseUBO(1, 0, camera);
		Shader->UseSampler(0, 1, TextureSampler.get());

		//Use Vertex & Index Buffer
		buffers.VertexBuffer->Use();
		IndexBuffer->Use();
		// buffers.QuadIndexBuffer->Use();

		//Use Shader
		Shader->Use();

		buffers.VertexBuffer->AwaitLoading();
		TRAP::Graphics::RenderCommand::DrawIndexed(indicesCount);

		buffers.QuadCount = 0;

		drawcalls++;
	}

	DataBufferIndex = 0;

	return drawcalls;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::Renderer2DData::QuadData::GetTextureIndex(Ref<Texture> texture)
{
	TRAP_ASSERT(texture, "Texture is nullptr!");
	TRAP_ASSERT(texture->GetType() == TextureType::Texture2D, "Texture is not a Texture2D!");

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	const auto res = std::find(DataBuffers[imageIndex][DataBufferIndex].TextureSlots.begin(),
	                           DataBuffers[imageIndex][DataBufferIndex].TextureSlots.end(), texture);

	if(res == DataBuffers[imageIndex][DataBufferIndex].TextureSlots.end())
	{
		DataBuffers[imageIndex][DataBufferIndex].TextureSlots[TextureSlotIndex++] = texture;
		return TextureSlotIndex - 1;
	}

	return std::distance(DataBuffers[imageIndex][DataBufferIndex].TextureSlots.begin(), res);
}

//-------------------------------------------------------------------------------------------------------------------//
//CircleData---------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::CircleData::Init()
{
	Shader = Shader::CreateFromSource("Renderer2D_Circle", std::string(Embed::Renderer2DCircleShader));

	//Initialize circle indices (actually quad indices)
	auto IndicesData = MakeScope<std::array<uint32_t, MaxCircleIndices>>();
	for(uint32_t offset = 0, i = 0; i < MaxCircleIndices; i += 6)
	{
		(*IndicesData)[i + 0] = offset + 0;
		(*IndicesData)[i + 1] = offset + 1;
		(*IndicesData)[i + 2] = offset + 2;

		(*IndicesData)[i + 3] = offset + 2;
		(*IndicesData)[i + 4] = offset + 3;
		(*IndicesData)[i + 5] = offset + 0;

		offset += 4;
	}

	//This uses the same indices as for quads
	IndexBuffer = IndexBuffer::Create((*IndicesData).data(),
									  MaxCircleIndices * sizeof(uint32_t),
									  UpdateFrequency::Dynamic);
	IndexBuffer->AwaitLoading();
	IndicesData.reset();

	//Create and fill circle index and vertex buffers
	for(auto& buffers : DataBuffers)
	{
		buffers.emplace_back();

		buffers[DataBufferIndex].VertexBuffer = VertexBuffer::Create(MaxCircleVertices * sizeof(CircleVertex),
																	 UpdateFrequency::Dynamic);
		buffers[DataBufferIndex].VertexBuffer->SetLayout(VertexLayout);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::CircleData::Shutdown()
{
	DataBuffers = {};
	IndexBuffer.reset();
	Shader.reset();

}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::CircleData::Reset()
{
	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	VertexBufferPtr = DataBuffers[imageIndex][DataBufferIndex].Vertices.data();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::CircleData::ExtendBuffers()
{
	DataBufferIndex++;

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	if(DataBufferIndex < DataBuffers[imageIndex].size()) //Already allocated
		return;

	DataBuffers[imageIndex].emplace_back();

	auto& buffers = DataBuffers[imageIndex][DataBufferIndex];

	buffers.VertexBuffer = TRAP::Graphics::VertexBuffer::Create(MaxCircleVertices * sizeof(CircleVertex),
	                                                            TRAP::Graphics::UpdateFrequency::Dynamic);
	buffers.VertexBuffer->SetLayout(VertexLayout);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Renderer2DData::CircleData::DrawBuffers(UniformBuffer* camera)
{
	TRAP_ASSERT(camera, "Invalid UniformBuffer!");

	uint32_t drawcalls = 0;

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	for(auto& buffers : DataBuffers[imageIndex])
	{
		if(buffers.CircleCount == 0)
			continue;

		const uint32_t verticesSize = buffers.CircleCount * sizeof(TRAP::Graphics::Renderer2DData::CircleData::CircleVertex) * 4;
		const uint32_t indicesCount = buffers.CircleCount * 6;

		//Update Vertices
		buffers.VertexBuffer->SetData(reinterpret_cast<float*>(buffers.Vertices.data()), verticesSize);

		//Use dynamic shader resources
		Shader->UseUBO(1, 0, camera);

		//Use Vertex & Index Buffer
		buffers.VertexBuffer->Use();
		IndexBuffer->Use();

		//Use Shader
		Shader->Use();

		buffers.VertexBuffer->AwaitLoading();
		TRAP::Graphics::RenderCommand::DrawIndexed(indicesCount);

		buffers.CircleCount = 0;

		drawcalls++;
	}

	DataBufferIndex = 0;

	return drawcalls;
}

//-------------------------------------------------------------------------------------------------------------------//
//LineData-----------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::LineData::Init()
{
	Shader = Shader::CreateFromSource("Renderer2D_Line", std::string(Embed::Renderer2DLineShader));

	//Create and fill line vertex buffer
	for(auto& buffers : DataBuffers)
	{
		buffers.emplace_back();

		buffers[DataBufferIndex].VertexBuffer = VertexBuffer::Create(MaxLineVertices * sizeof(LineVertex),
																	 UpdateFrequency::Dynamic);
		buffers[DataBufferIndex].VertexBuffer->SetLayout(VertexLayout);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::LineData::Shutdown()
{
	DataBuffers = {};
	Shader.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::LineData::Reset()
{
	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	VertexBufferPtr = DataBuffers[imageIndex][DataBufferIndex].Vertices.data();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::LineData::ExtendBuffers()
{
	DataBufferIndex++;

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	if(DataBufferIndex < DataBuffers[imageIndex].size()) //Already allocated
		return;

	DataBuffers[imageIndex].emplace_back();

	auto& buffers = DataBuffers[imageIndex][DataBufferIndex];

	buffers.VertexBuffer = TRAP::Graphics::VertexBuffer::Create(MaxLineVertices * sizeof(LineVertex),
	                                                            TRAP::Graphics::UpdateFrequency::Dynamic);
	buffers.VertexBuffer->SetLayout(VertexLayout);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Renderer2DData::LineData::DrawBuffers(UniformBuffer* camera)
{
	TRAP_ASSERT(camera, "Invalid UniformBuffer!");

	uint32_t drawcalls = 0;

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	for(auto& buffers : DataBuffers[imageIndex])
	{
		if(buffers.LineCount == 0)
			continue;

		const uint32_t verticesSize = buffers.LineCount * sizeof(TRAP::Graphics::Renderer2DData::LineData::LineVertex) * 2;

		TRAP::Graphics::RenderCommand::SetPrimitiveTopology(TRAP::Graphics::PrimitiveTopology::LineList);

		//Update Vertices
		buffers.VertexBuffer->SetData(reinterpret_cast<float*>(buffers.Vertices.data()), verticesSize);

		//Use dynamic shader resources
		Shader->UseUBO(1, 0, camera);

		//Use Vertex Buffer
		buffers.VertexBuffer->Use();

		//Use Shader
		Shader->Use();

		buffers.VertexBuffer->AwaitLoading();
		TRAP::Graphics::RenderCommand::Draw(buffers.LineCount * 2);

		buffers.LineCount = 0;

		TRAP::Graphics::RenderCommand::SetPrimitiveTopology(TRAP::Graphics::PrimitiveTopology::TriangleList);

		drawcalls++;
	}

	DataBufferIndex = 0;

	return drawcalls;
}

//-------------------------------------------------------------------------------------------------------------------//
//Renderer2D---------------------------------------------------------------------------------------------------------//
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

	s_data.QuadData.Init();
	s_data.CircleData.Init();
	s_data.LineData.Init();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::Shutdown()
{
	TP_PROFILE_FUNCTION();

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::Renderer2DPrefix, "Shutting down");
#endif

	s_data.QuadData.Shutdown();
	s_data.CircleData.Shutdown();
	s_data.LineData.Shutdown();

	s_data.CameraUniformBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::BeginScene(const Camera& camera, const Math::Mat4& transform)
{
	TP_PROFILE_FUNCTION();

	//Update Camera
	s_data.UniformCamera.ProjectionMatrix = camera.GetProjectionMatrix();
	s_data.UniformCamera.ViewMatrix = Math::Inverse(transform);
	s_data.CameraUniformBuffer->SetData(&s_data.UniformCamera, sizeof(Renderer2DData::UniformCamera));
	s_data.CameraUniformBuffer->AwaitLoading();

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	//Reset Vertices & Indices
	s_data.LineData.Reset();
	s_data.CircleData.Reset();
	s_data.QuadData.Reset();
	for(auto& buffers : s_data.QuadData.DataBuffers[imageIndex])
		std::fill(buffers.TextureSlots.begin(), buffers.TextureSlots.end(), s_data.QuadData.WhiteTexture);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::BeginScene(const OrthographicCamera& camera)
{
	TP_PROFILE_FUNCTION();

	//Update Camera
	s_data.UniformCamera.ProjectionMatrix = camera.GetProjectionMatrix();
	s_data.UniformCamera.ViewMatrix = camera.GetViewMatrix();
	s_data.CameraUniformBuffer->SetData(&s_data.UniformCamera, sizeof(Renderer2DData::UniformCamera));
	s_data.CameraUniformBuffer->AwaitLoading();

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	//Reset Vertices & Indices
	s_data.LineData.Reset();
	s_data.CircleData.Reset();
	s_data.QuadData.Reset();
	for(auto& buffers : s_data.QuadData.DataBuffers[imageIndex])
		std::fill(buffers.TextureSlots.begin(), buffers.TextureSlots.end(), s_data.QuadData.WhiteTexture);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::BeginScene(const EditorCamera& camera)
{
	TP_PROFILE_FUNCTION();

	//Update Camera
	s_data.UniformCamera.ProjectionMatrix = camera.GetProjectionMatrix();
	s_data.UniformCamera.ViewMatrix = camera.GetViewMatrix();
	s_data.CameraUniformBuffer->SetData(&s_data.UniformCamera, sizeof(Renderer2DData::UniformCamera));
	s_data.CameraUniformBuffer->AwaitLoading();

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	//Reset Vertices & Indices
	s_data.LineData.Reset();
	s_data.CircleData.Reset();
	s_data.QuadData.Reset();
	for(auto& buffers : s_data.QuadData.DataBuffers[imageIndex])
		std::fill(buffers.TextureSlots.begin(), buffers.TextureSlots.end(), s_data.QuadData.WhiteTexture);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::EndScene()
{
	TP_PROFILE_FUNCTION();

	s_data.Stats.DrawCalls += s_data.QuadData.DrawBuffers(s_data.CameraUniformBuffer.get());
	s_data.Stats.DrawCalls += s_data.CircleData.DrawBuffers(s_data.CameraUniformBuffer.get());
	s_data.Stats.DrawCalls += s_data.LineData.DrawBuffers(s_data.CameraUniformBuffer.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const Math::Vec4& color)
{
	DrawQuad(transform, color, s_data.QuadData.WhiteTexture);
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

	if (s_data.QuadData.DataBuffers[imageIndex][s_data.QuadData.DataBufferIndex].QuadCount * 6 >= Renderer2DData::QuadData::MaxQuadIndices ||
	    s_data.QuadData.TextureSlotIndex >= Renderer2DData::QuadData::MaxTextureSlots)
	{
		s_data.QuadData.ExtendBuffers();
		s_data.QuadData.Reset();
	}

	Ref<Texture> tex = texture ? texture : s_data.QuadData.WhiteTexture;
	const float textureIndex = s_data.QuadData.GetTextureIndex(std::move(tex));

	for (uint64_t i = 0; i < quadVertexCount; i++)
	{
		s_data.QuadData.VertexBufferPtr->Position = Math::Vec3(transform * Renderer2DData::QuadData::VertexPositions[i]);
		s_data.QuadData.VertexBufferPtr->Color = color;
		s_data.QuadData.VertexBufferPtr->TexCoord = texCoords ? (*texCoords)[i] : textureCoords[i];
		s_data.QuadData.VertexBufferPtr->TexIndex = textureIndex;
		s_data.QuadData.VertexBufferPtr->EntityID = entityID;
		s_data.QuadData.VertexBufferPtr++;
	}

	s_data.QuadData.DataBuffers[imageIndex][s_data.QuadData.DataBufferIndex].QuadCount++;

	s_data.Stats.QuadCount++;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawCircle(const Math::Mat4& transform, const Math::Vec4& color,
                                            const float thickness, const float fade, const int32_t entityID)
{
	constexpr uint64_t circleVertexCount = 4;

	const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());

	if (s_data.CircleData.DataBuffers[imageIndex][s_data.CircleData.DataBufferIndex].CircleCount * 6 >= Renderer2DData::CircleData::MaxCircleIndices)
	{
		s_data.CircleData.ExtendBuffers();
		s_data.CircleData.Reset();
	}

	for (uint64_t i = 0; i < circleVertexCount; i++)
	{
		s_data.CircleData.VertexBufferPtr->WorldPosition = Math::Vec3(transform * Renderer2DData::QuadData::VertexPositions[i]);
		s_data.CircleData.VertexBufferPtr->LocalPosition = Math::Vec3(Renderer2DData::QuadData::VertexPositions[i] * 2.0f); //Range [-1, 1]
		s_data.CircleData.VertexBufferPtr->Color = color;
		s_data.CircleData.VertexBufferPtr->Thickness = thickness;
		s_data.CircleData.VertexBufferPtr->Fade = fade;
		s_data.CircleData.VertexBufferPtr->EntityID = entityID;
		s_data.CircleData.VertexBufferPtr++;
	}

	s_data.CircleData.DataBuffers[imageIndex][s_data.CircleData.DataBufferIndex].CircleCount++;

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

	if (s_data.LineData.DataBuffers[imageIndex][s_data.LineData.DataBufferIndex].LineCount * 2 >= Renderer2DData::LineData::MaxLineVertices)
	{
		s_data.LineData.ExtendBuffers();
		s_data.LineData.Reset();
	}

	s_data.LineData.VertexBufferPtr->Position = p0;
	s_data.LineData.VertexBufferPtr->Color = color;
	s_data.LineData.VertexBufferPtr->EntityID = entityID;
	s_data.LineData.VertexBufferPtr++;

	s_data.LineData.VertexBufferPtr->Position = p1;
	s_data.LineData.VertexBufferPtr->Color = color;
	s_data.LineData.VertexBufferPtr->EntityID = entityID;
	s_data.LineData.VertexBufferPtr++;

	s_data.LineData.DataBuffers[imageIndex][s_data.LineData.DataBufferIndex].LineCount++;

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
		lineVertices[i] = Math::Vec3(transform * Renderer2DData::QuadData::VertexPositions[i]);

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
