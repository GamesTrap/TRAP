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
#include "Graphics/Cameras/Editor/EditorCamera.h"
#include "Scene/Components.h"
#include "Graphics/Cameras/Orthographic/OrthographicCamera.h"
#include "Graphics/Textures/Texture.h"
#include "Graphics/Textures/SubTexture2D.h"

namespace TRAP::Graphics
{
	struct Renderer2DData
	{
		Renderer2DData();
		constexpr Renderer2DData(const Renderer2DData&) = delete;
		constexpr Renderer2DData& operator=(const Renderer2DData&) = delete;
		Renderer2DData(Renderer2DData&&) noexcept = default;
		Renderer2DData& operator=(Renderer2DData&&) noexcept = default;
		~Renderer2DData() = default;

		struct QuadData
		{
			static constexpr u32 MaxQuads = 50000;
			static constexpr u32 MaxQuadVertices = MaxQuads * 4;
			static constexpr u32 MaxQuadIndices = MaxQuads * 6;
			static constexpr u32 MaxTextureSlots = 32;

			struct QuadVertex
			{
				Math::Vec3 Position;
				Math::Vec4 Color;
				Math::Vec2 TexCoord;
				f32 TexIndex;

				//Editor only
				i32 EntityID;
			};

			//Per draw call data
			struct QuadBuffers
			{
				Scope<Graphics::VertexBuffer> VertexBuffer = nullptr;
				std::vector<QuadVertex> Vertices = std::vector<QuadVertex>(MaxQuadVertices);
				std::vector<Ref<Texture>> TextureSlots = std::vector<Ref<Texture>>(MaxTextureSlots);
				u32 QuadCount = 0;
			};
			std::array<std::vector<QuadBuffers>, RendererAPI::ImageCount> DataBuffers{};
			u32 DataBufferIndex = 0;
			QuadVertex* VertexBufferPtr = nullptr;
			u32 TextureSlotIndex = 1; //0 = White texture

			inline static Scope<Graphics::IndexBuffer> IndexBuffer = nullptr;

			inline static Ref<Graphics::Shader> Shader = nullptr;
			inline static Ref<Sampler> TextureSampler = nullptr;
			inline static Ref<Texture> WhiteTexture = nullptr;

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

			static void Init();
			void InitBuffers();
			static void Shutdown() noexcept;
			void Reset();
			void ExtendBuffers(); //Extend buffers to allow for an additional draw call
			u32 DrawBuffers(UniformBuffer* camera);

			[[nodiscard]] u32 GetTextureIndex(const Ref<Texture>& texture);
		} QuadData;

		//-------------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//

		struct CircleData
		{
			static constexpr u32 MaxCircles = 50000;
			static constexpr u32 MaxCircleVertices = MaxCircles * 4;
			static constexpr u32 MaxCircleIndices = MaxCircles * 6;

			struct CircleVertex
			{
				Math::Vec3 WorldPosition;
				Math::Vec3 LocalPosition;
				Math::Vec4 Color;
				f32 Thickness;
				f32 Fade;

				//Editor only
				i32 EntityID;
			};

			//Per draw call data
			struct CircleBuffers
			{
				Scope<Graphics::VertexBuffer> VertexBuffer = nullptr;
				std::vector<CircleVertex> Vertices = std::vector<CircleVertex>(MaxCircleVertices);
				u32 CircleCount = 0;
			};
			std::array<std::vector<CircleBuffers>, RendererAPI::ImageCount> DataBuffers{};
			u32 DataBufferIndex = 0;
			CircleVertex* VertexBufferPtr = nullptr;

			inline static Scope<Graphics::IndexBuffer> IndexBuffer = nullptr;

			inline static Ref<Graphics::Shader> Shader = nullptr;

			inline static const TRAP::Graphics::VertexBufferLayout VertexLayout
			{
				{ ShaderDataType::Float3, "WorldPosition" },
				{ ShaderDataType::Float3, "LocalPosition" },
				{ ShaderDataType::Float4, "Color" },
				{ ShaderDataType::Float, "Thickness" },
				{ ShaderDataType::Float, "Fade" },
				{ ShaderDataType::Int, "EntityID" }
			};

			static void Init();
			void InitBuffers();
			static void Shutdown() noexcept;
			void Reset();
			void ExtendBuffers(); //Extend buffers to allow for an additional draw call
			u32 DrawBuffers(UniformBuffer* camera);
		} CircleData;

		//-------------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//

		struct LineData
		{
			static constexpr u32 MaxLines = 50000;
			static constexpr u32 MaxLineVertices = MaxLines * 2;

			struct LineVertex
			{
				Math::Vec3 Position;
				Math::Vec4 Color;

				//Editor only
				i32 EntityID;
			};

			struct LineBuffers
			{
				Scope<Graphics::VertexBuffer> VertexBuffer = nullptr;
				std::vector<LineVertex> Vertices = std::vector<LineVertex>(MaxLineVertices);
				u32 LineCount = 0;
			};
			std::array<std::vector<LineBuffers>, RendererAPI::ImageCount> DataBuffers{};
			u32 DataBufferIndex = 0;
			LineVertex* VertexBufferPtr = nullptr;

			inline static Ref<Graphics::Shader> Shader = nullptr;

			inline static const TRAP::Graphics::VertexBufferLayout VertexLayout
			{
				{ ShaderDataType::Float3, "Position" },
				{ ShaderDataType::Float4, "Color" },
				{ ShaderDataType::Int, "EntityID" }
			};

			static void Init();
			void InitBuffers();
			static void Shutdown() noexcept;
			void Reset();
			void ExtendBuffers(); //Extend buffers to allow for an additional draw call
			u32 DrawBuffers(UniformBuffer* camera);
		} LineData;

		//-------------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------------------------------------------------------------//

		inline constinit static Renderer2D::Statistics Stats{};

		Scope<UniformBuffer> CameraUniformBuffer;
		struct UniformCamera
		{
			Math::Mat4 ProjectionMatrix{ 1.0f };
			Math::Mat4 ViewMatrix{ 1.0f };
		} UniformCamera;
	};
}

//-------------------------------------------------------------------------------------------------------------------//
//Renderer2DData-----------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Renderer2DData::Renderer2DData()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	CameraUniformBuffer = UniformBuffer::Create(&UniformCamera,
	                                            sizeof(Renderer2DData::UniformCamera),
		                                        UpdateFrequency::Dynamic);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Graphics::Renderer2DData> TRAP::Graphics::Renderer2D::s_data{};

//-------------------------------------------------------------------------------------------------------------------//
//QuadData-----------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::QuadData::Init()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Shader = Shader::CreateFromSource("Renderer2D_Quad", std::string(Embed::Renderer2DQuadShader));

	const Scope<Image> whiteImage = Image::LoadFromMemory(2, 2, Image::ColorFormat::RGBA,
														  std::vector<u8>{255, 255, 255, 255,
																			255, 255, 255, 255,
																			255, 255, 255, 255,
																			255, 255, 255, 255 });
	WhiteTexture = Texture::CreateFromImage("Renderer2DWhite", whiteImage.get(), TextureType::Texture2D);
	WhiteTexture->AwaitLoading();

	TextureSampler = Sampler::Create({});

	//Initialize quad indices
	const auto IndicesData = MakeScope<std::array<u32, MaxQuadIndices>>();
	for(u32 offset = 0, i = 0; i < MaxQuadIndices; i += 6)
	{
		(*IndicesData)[i + 0] = offset + 0;
		(*IndicesData)[i + 1] = offset + 1;
		(*IndicesData)[i + 2] = offset + 2;

		(*IndicesData)[i + 3] = offset + 2;
		(*IndicesData)[i + 4] = offset + 3;
		(*IndicesData)[i + 5] = offset + 0;

		offset += 4;
	}

	IndexBuffer = IndexBuffer::Create({IndicesData->data(), MaxQuadIndices}, UpdateFrequency::Dynamic);
	IndexBuffer->AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::QuadData::InitBuffers()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	//Create and fill quad index and vertex buffers
	for(auto& buffers : DataBuffers)
	{
		buffers.emplace_back();

		buffers[DataBufferIndex].VertexBuffer = VertexBuffer::Create(MaxQuadVertices * sizeof(QuadVertex),
																	 UpdateFrequency::Dynamic);
		buffers[DataBufferIndex].VertexBuffer->SetLayout(VertexLayout);

		std::ranges::fill(buffers[DataBufferIndex].TextureSlots, WhiteTexture);
	}

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/
	VertexBufferPtr = DataBuffers[imageIndex][DataBufferIndex].Vertices.data();
	TextureSlotIndex = 1;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::QuadData::Shutdown() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Renderer2DData::QuadData::IndexBuffer.reset();
	Renderer2DData::QuadData::Shader.reset();
	Renderer2DData::QuadData::TextureSampler.reset();
	Renderer2DData::QuadData::WhiteTexture.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::QuadData::Reset()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

	if(!DataBuffers[imageIndex].empty())
	{
		VertexBufferPtr = DataBuffers[imageIndex][DataBufferIndex].Vertices.data();

		std::ranges::fill(DataBuffers[imageIndex][DataBufferIndex].TextureSlots, WhiteTexture);
	}

	TextureSlotIndex = 1;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::QuadData::ExtendBuffers()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	DataBufferIndex++;

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

	if(DataBufferIndex < DataBuffers[imageIndex].size()) //Already allocated
		return;

	DataBuffers[imageIndex].emplace_back();

	auto& buffers = DataBuffers[imageIndex][DataBufferIndex];

	buffers.VertexBuffer = TRAP::Graphics::VertexBuffer::Create(MaxQuadVertices * sizeof(QuadVertex),
	                                                            TRAP::Graphics::UpdateFrequency::Dynamic);
	buffers.VertexBuffer->SetLayout(VertexLayout);

	std::ranges::fill(DataBuffers[imageIndex][DataBufferIndex].TextureSlots, WhiteTexture);
}

//-------------------------------------------------------------------------------------------------------------------//

u32 TRAP::Graphics::Renderer2DData::QuadData::DrawBuffers(UniformBuffer* camera)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(camera, "Renderer2DData::QuadData::DrawBuffers(): Invalid UniformBuffer!");

	u32 drawcalls = 0;

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

	for(auto& buffers : DataBuffers[imageIndex])
	{
		if(buffers.QuadCount == 0)
			continue;

		const u32 verticesSize = buffers.QuadCount * sizeof(TRAP::Graphics::Renderer2DData::QuadData::QuadVertex);
		const u32 indicesCount = buffers.QuadCount * 6;

		//Update Vertices
		buffers.VertexBuffer->SetData({reinterpret_cast<const f32*>(buffers.Vertices.data()), verticesSize});

		//Use dynamic shader resources
		Shader->UseTextures(1, 1, buffers.TextureSlots);
		Shader->UseUBO(1, 0, camera);
		Shader->UseSampler(0, 1, TextureSampler.get());

		//Use Vertex & Index Buffer
		buffers.VertexBuffer->Use();
		IndexBuffer->Use();

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

[[nodiscard]] u32 TRAP::Graphics::Renderer2DData::QuadData::GetTextureIndex(const Ref<Texture>& texture)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(texture, "Renderer2DData::QuadData::GetTextureIndex(): Texture is nullptr!");
	TRAP_ASSERT(texture->GetType() == TextureType::Texture2D, "Renderer2DData::QuadData::GetTextureIndex(): Texture is not a Texture2D!");

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

	const auto res = std::ranges::find(DataBuffers[imageIndex][DataBufferIndex].TextureSlots, texture);

	if(res == DataBuffers[imageIndex][DataBufferIndex].TextureSlots.end())
	{
		DataBuffers[imageIndex][DataBufferIndex].TextureSlots[TextureSlotIndex++] = texture;
		return TextureSlotIndex - 1;
	}

	return NumericCast<u32>(std::distance(DataBuffers[imageIndex][DataBufferIndex].TextureSlots.begin(), res));
}

//-------------------------------------------------------------------------------------------------------------------//
//CircleData---------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::CircleData::Init()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Shader = Shader::CreateFromSource("Renderer2D_Circle", std::string(Embed::Renderer2DCircleShader));

	//Initialize circle indices (actually quad indices)
	const auto IndicesData = MakeScope<std::array<u32, MaxCircleIndices>>();
	for(u32 offset = 0, i = 0; i < MaxCircleIndices; i += 6)
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
	IndexBuffer = IndexBuffer::Create({IndicesData->data(), MaxCircleIndices}, UpdateFrequency::Dynamic);
	IndexBuffer->AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::CircleData::InitBuffers()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	//Create and fill circle index and vertex buffers
	for(auto& buffers : DataBuffers)
	{
		buffers.emplace_back();

		buffers[DataBufferIndex].VertexBuffer = VertexBuffer::Create(MaxCircleVertices * sizeof(CircleVertex),
																	 UpdateFrequency::Dynamic);
		buffers[DataBufferIndex].VertexBuffer->SetLayout(VertexLayout);
	}

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/
	VertexBufferPtr = DataBuffers[imageIndex][DataBufferIndex].Vertices.data();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::CircleData::Shutdown() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Renderer2DData::CircleData::IndexBuffer.reset();
	Renderer2DData::CircleData::Shader.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::CircleData::Reset()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

	if(!DataBuffers[imageIndex].empty())
		VertexBufferPtr = DataBuffers[imageIndex][DataBufferIndex].Vertices.data();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::CircleData::ExtendBuffers()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	DataBufferIndex++;

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

	if(DataBufferIndex < DataBuffers[imageIndex].size()) //Already allocated
		return;

	DataBuffers[imageIndex].emplace_back();

	auto& buffers = DataBuffers[imageIndex][DataBufferIndex];

	buffers.VertexBuffer = TRAP::Graphics::VertexBuffer::Create(MaxCircleVertices * sizeof(CircleVertex),
	                                                            TRAP::Graphics::UpdateFrequency::Dynamic);
	buffers.VertexBuffer->SetLayout(VertexLayout);
}

//-------------------------------------------------------------------------------------------------------------------//

u32 TRAP::Graphics::Renderer2DData::CircleData::DrawBuffers(UniformBuffer* camera)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(camera, "Renderer2DData::CircleData::DrawBuffers(): Invalid UniformBuffer!");

	u32 drawcalls = 0;

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

	for(auto& buffers : DataBuffers[imageIndex])
	{
		if(buffers.CircleCount == 0)
			continue;

		const u32 verticesSize = buffers.CircleCount * sizeof(TRAP::Graphics::Renderer2DData::CircleData::CircleVertex);
		const u32 indicesCount = buffers.CircleCount * 6;

		//Update Vertices
		buffers.VertexBuffer->SetData({reinterpret_cast<const f32*>(buffers.Vertices.data()), verticesSize});

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Shader = Shader::CreateFromSource("Renderer2D_Line", std::string(Embed::Renderer2DLineShader));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::LineData::InitBuffers()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	//Create and fill line vertex buffer
	for(auto& buffers : DataBuffers)
	{
		buffers.emplace_back();

		buffers[DataBufferIndex].VertexBuffer = VertexBuffer::Create(MaxLineVertices * sizeof(LineVertex),
																	 UpdateFrequency::Dynamic);
		buffers[DataBufferIndex].VertexBuffer->SetLayout(VertexLayout);
	}

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/
	VertexBufferPtr = DataBuffers[imageIndex][DataBufferIndex].Vertices.data();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::LineData::Shutdown() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Renderer2DData::LineData::Shader.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::LineData::Reset()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

	if(!DataBuffers[imageIndex].empty())
		VertexBufferPtr = DataBuffers[imageIndex][DataBufferIndex].Vertices.data();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2DData::LineData::ExtendBuffers()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	DataBufferIndex++;

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

	if(DataBufferIndex < DataBuffers[imageIndex].size()) //Already allocated
		return;

	DataBuffers[imageIndex].emplace_back();

	auto& buffers = DataBuffers[imageIndex][DataBufferIndex];

	buffers.VertexBuffer = TRAP::Graphics::VertexBuffer::Create(MaxLineVertices * sizeof(LineVertex),
	                                                            TRAP::Graphics::UpdateFrequency::Dynamic);
	buffers.VertexBuffer->SetLayout(VertexLayout);
}

//-------------------------------------------------------------------------------------------------------------------//

u32 TRAP::Graphics::Renderer2DData::LineData::DrawBuffers(UniformBuffer* camera)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(camera, "Renderer2DData::LineData::DrawBuffers(): Invalid UniformBuffer!");

	u32 drawcalls = 0;

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

	for(auto& buffers : DataBuffers[imageIndex])
	{
		if(buffers.LineCount == 0)
			continue;

		const u32 verticesSize = buffers.LineCount * sizeof(TRAP::Graphics::Renderer2DData::LineData::LineVertex);

		TRAP::Graphics::RenderCommand::SetPrimitiveTopology(TRAP::Graphics::PrimitiveTopology::LineList);

		//Update Vertices
		buffers.VertexBuffer->SetData({reinterpret_cast<const f32*>(buffers.Vertices.data()), verticesSize});

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::Renderer2DPrefix, "Initializing");
#endif /*ENABLE_GRAPHICS_DEBUG*/

	Renderer2DData::QuadData::Init();
	Renderer2DData::CircleData::Init();
	Renderer2DData::LineData::Init();

	s_data.emplace_back();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::Renderer2DPrefix, "Shutting down");
#endif /*ENABLE_GRAPHICS_DEBUG*/

	s_data.clear();

	Renderer2DData::LineData::Shutdown();
	Renderer2DData::CircleData::Shutdown();
	Renderer2DData::QuadData::Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::Reset() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	s_dataIndex = 0;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::BeginScene(const Camera& camera, const Math::Mat4& transform)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	//Create new Renderer2DData if needed
	if(s_dataIndex >= s_data.size())
		s_data.emplace_back();

	auto& currData = s_data[s_dataIndex];

	//Update Camera
	currData.UniformCamera.ProjectionMatrix = camera.GetProjectionMatrix();
	currData.UniformCamera.ViewMatrix = Math::Inverse(transform);
	currData.CameraUniformBuffer->SetData(&currData.UniformCamera, sizeof(Renderer2DData::UniformCamera));
	currData.CameraUniformBuffer->AwaitLoading();


	//Reset Vertices & Indices
	currData.LineData.Reset();
	currData.CircleData.Reset();
	currData.QuadData.Reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::BeginScene(const OrthographicCamera& camera)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	//Create new Renderer2DData if needed
	if(s_dataIndex >= s_data.size())
		s_data.emplace_back();

	auto& currData = s_data[s_dataIndex];

	//Update Camera
	currData.UniformCamera.ProjectionMatrix = camera.GetProjectionMatrix();
	currData.UniformCamera.ViewMatrix = camera.GetViewMatrix();
	currData.CameraUniformBuffer->SetData(&currData.UniformCamera, sizeof(Renderer2DData::UniformCamera));
	currData.CameraUniformBuffer->AwaitLoading();


	//Reset Vertices & Indices
	currData.LineData.Reset();
	currData.CircleData.Reset();
	currData.QuadData.Reset();
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::Renderer2D::BeginScene(const EditorCamera& camera)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	//Create new Renderer2DData if needed
	if(s_dataIndex >= s_data.size())
		s_data.emplace_back();

	auto& currData = s_data[s_dataIndex];

	//Update Camera
	currData.UniformCamera.ProjectionMatrix = camera.GetProjectionMatrix();
	currData.UniformCamera.ViewMatrix = camera.GetViewMatrix();
	currData.CameraUniformBuffer->SetData(&currData.UniformCamera, sizeof(Renderer2DData::UniformCamera));
	currData.CameraUniformBuffer->AwaitLoading();


	//Reset Vertices & Indices
	currData.LineData.Reset();
	currData.CircleData.Reset();
	currData.QuadData.Reset();
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::EndScene()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	auto& currData = s_data[s_dataIndex];

	Renderer2DData::Stats.DrawCalls += currData.QuadData.DrawBuffers(currData.CameraUniformBuffer.get());
	Renderer2DData::Stats.DrawCalls += currData.CircleData.DrawBuffers(currData.CameraUniformBuffer.get());
	Renderer2DData::Stats.DrawCalls += currData.LineData.DrawBuffers(currData.CameraUniformBuffer.get());

	s_dataIndex++;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::SetCustomSampler(const TRAP::Ref<TRAP::Graphics::Sampler>& sampler)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(sampler, "Renderer2D::SetCustomSampler(): Sampler is nullptr!");
	if(!sampler)
		return;

	Renderer2DData::QuadData::TextureSampler = sampler;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const Math::Vec4& color)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	DrawQuad(transform, color, Renderer2DData::QuadData::WhiteTexture);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const Ref<Texture>& texture)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(texture->GetType() != TextureType::Texture2D)
		return;

	DrawQuad(transform, Math::Vec4(1.0f), texture);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const Math::Vec4& color,
                                          const Ref<Texture>& texture)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(texture->GetType() != TextureType::Texture2D)
		return;

	Math::Mat4 transformation;
	if (transform.Rotation.x() != 0.0f || transform.Rotation.y() != 0.0f || transform.Rotation.z() != 0.0f)
		transformation = Math::Translate(transform.Position) *
		                 Mat4Cast(Math::Quat(Radians(transform.Rotation))) * Math::Scale(transform.Scale);
	else
		transformation = Math::Translate(transform.Position) * Math::Scale(transform.Scale);

	DrawQuad(transformation, color, texture, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const TRAP::Ref<SubTexture2D>& texture)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(!texture || texture->GetTexture()->GetType() != TextureType::Texture2D)
		return;

	DrawQuad(transform, Math::Vec4(1.0f), texture);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Transform& transform, const Math::Vec4& color,
                                          const TRAP::Ref<SubTexture2D>& texture)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(!texture || texture->GetTexture()->GetType() != TextureType::Texture2D)
		return;

	Math::Mat4 transformation;
	if (transform.Rotation.x() != 0.0f || transform.Rotation.y() != 0.0f || transform.Rotation.z() != 0.0f)
		transformation = Math::Translate(transform.Position) *
		                 Mat4Cast(Math::Quat(Radians(transform.Rotation))) * Math::Scale(transform.Scale);
	else
		transformation = Math::Translate(transform.Position) * Math::Scale(transform.Scale);

	DrawQuad(transformation, color, texture->GetTexture(), &texture->GetTexCoords());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawQuad(const Math::Mat4& transform, const Math::Vec4& color,
                                          const Ref<Texture>& texture, const std::array<Math::Vec2, 4>* const texCoords,
										  const i32 entityID)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	static constexpr u64 quadVertexCount = 4;
	static constexpr std::array<Math::Vec2, 4> textureCoords = { {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}} };

	if(texture && texture->GetType() != TextureType::Texture2D)
		return;

	auto& currData = s_data[s_dataIndex];

	if(std::get<0>(currData.QuadData.DataBuffers).empty())
		currData.QuadData.InitBuffers();

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

	if (currData.QuadData.DataBuffers[imageIndex][currData.QuadData.DataBufferIndex].QuadCount * 6 >= Renderer2DData::QuadData::MaxQuadIndices ||
	    currData.QuadData.TextureSlotIndex >= Renderer2DData::QuadData::MaxTextureSlots)
	{
		currData.QuadData.ExtendBuffers();
		currData.QuadData.Reset();
	}

	Ref<Texture> tex = texture ? texture : Renderer2DData::QuadData::WhiteTexture;
	const u32 textureIndex = currData.QuadData.GetTextureIndex(tex);

	for (u64 i = 0; i < quadVertexCount; i++)
	{
		currData.QuadData.VertexBufferPtr->Position = Math::Vec3(transform * Renderer2DData::QuadData::VertexPositions[i]);
		currData.QuadData.VertexBufferPtr->Color = color;
		currData.QuadData.VertexBufferPtr->TexCoord = texCoords != nullptr ? (*texCoords)[i] : textureCoords[i];
		currData.QuadData.VertexBufferPtr->TexIndex = NumericCast<f32>(textureIndex);
		currData.QuadData.VertexBufferPtr->EntityID = entityID;
		currData.QuadData.VertexBufferPtr++;
	}

	currData.QuadData.DataBuffers[imageIndex][currData.QuadData.DataBufferIndex].QuadCount++;

	Renderer2DData::Stats.QuadCount++;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawCircle(const Math::Mat4& transform, const Math::Vec4& color,
                                            const f32 thickness, const f32 fade, const i32 entityID)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	static constexpr u64 circleVertexCount = 4;

	auto& currData = s_data[s_dataIndex];

	if(std::get<0>(currData.CircleData.DataBuffers).empty())
		currData.CircleData.InitBuffers();

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

	if (currData.CircleData.DataBuffers[imageIndex][currData.CircleData.DataBufferIndex].CircleCount * 6 >= Renderer2DData::CircleData::MaxCircleIndices)
	{
		currData.CircleData.ExtendBuffers();
		currData.CircleData.Reset();
	}

	for (u64 i = 0; i < circleVertexCount; i++)
	{
		currData.CircleData.VertexBufferPtr->WorldPosition = Math::Vec3(transform * Renderer2DData::QuadData::VertexPositions[i]);
		currData.CircleData.VertexBufferPtr->LocalPosition = Math::Vec3(Renderer2DData::QuadData::VertexPositions[i] * 2.0f); //Range [-1, 1]
		currData.CircleData.VertexBufferPtr->Color = color;
		currData.CircleData.VertexBufferPtr->Thickness = thickness;
		currData.CircleData.VertexBufferPtr->Fade = fade;
		currData.CircleData.VertexBufferPtr->EntityID = entityID;
		currData.CircleData.VertexBufferPtr++;
	}

	currData.CircleData.DataBuffers[imageIndex][currData.CircleData.DataBufferIndex].CircleCount++;

	Renderer2DData::Stats.CircleCount++;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawSprite(const TRAP::Math::Mat4& transform,
                                            const TRAP::SpriteRendererComponent& sprite, const i32 entityID)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	DrawQuad(transform, sprite.Color, nullptr, nullptr, entityID);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawLine(const TRAP::Math::Vec3& p0, const TRAP::Math::Vec3& p1,
                                          const TRAP::Math::Vec4& color, const i32 entityID)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	auto& currData = s_data[s_dataIndex];

	if(std::get<0>(currData.LineData.DataBuffers).empty())
		currData.LineData.InitBuffers();

#ifndef TRAP_HEADLESS_MODE
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow());
#else
	const u32 imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

	if (currData.LineData.DataBuffers[imageIndex][currData.LineData.DataBufferIndex].LineCount * 2 >= Renderer2DData::LineData::MaxLineVertices)
	{
		currData.LineData.ExtendBuffers();
		currData.LineData.Reset();
	}

	currData.LineData.VertexBufferPtr->Position = p0;
	currData.LineData.VertexBufferPtr->Color = color;
	currData.LineData.VertexBufferPtr->EntityID = entityID;
	currData.LineData.VertexBufferPtr++;

	currData.LineData.VertexBufferPtr->Position = p1;
	currData.LineData.VertexBufferPtr->Color = color;
	currData.LineData.VertexBufferPtr->EntityID = entityID;
	currData.LineData.VertexBufferPtr++;

	currData.LineData.DataBuffers[imageIndex][currData.LineData.DataBufferIndex].LineCount++;

	Renderer2DData::Stats.LineCount++;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawRect(const TRAP::Math::Vec3& position, const TRAP::Math::Vec2& size,
                                          const TRAP::Math::Vec4& color, const i32 entityID)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const TRAP::Math::Vec3 p0 = position + TRAP::Math::Vec3(-size.x() / 2.0f, -size.y() / 2.0f, 0.0f);
	const TRAP::Math::Vec3 p1 = position + TRAP::Math::Vec3( size.x() / 2.0f, -size.y() / 2.0f, 0.0f);
	const TRAP::Math::Vec3 p2 = position + TRAP::Math::Vec3( size.x() / 2.0f,  size.y() / 2.0f, 0.0f);
	const TRAP::Math::Vec3 p3 = position + TRAP::Math::Vec3(-size.x() / 2.0f,  size.y() / 2.0f, 0.0f);

	DrawLine(p0, p1, color, entityID);
	DrawLine(p1, p2, color, entityID);
	DrawLine(p2, p3, color, entityID);
	DrawLine(p3, p0, color, entityID);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::DrawRect(const TRAP::Math::Mat4& transform, const TRAP::Math::Vec4& color,
                                          const i32 entityID)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	std::array<TRAP::Math::Vec3, 4> lineVertices{};
	for(usize i = 0; i < lineVertices.size(); ++i)
		lineVertices[i] = Math::Vec3(transform * Renderer2DData::QuadData::VertexPositions[i]);

	DrawLine(std::get<0>(lineVertices), std::get<1>(lineVertices), color, entityID);
	DrawLine(std::get<1>(lineVertices), std::get<2>(lineVertices), color, entityID);
	DrawLine(std::get<2>(lineVertices), std::get<3>(lineVertices), color, entityID);
	DrawLine(std::get<3>(lineVertices), std::get<0>(lineVertices), color, entityID);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::Renderer2D::Statistics TRAP::Graphics::Renderer2D::GetStats() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return Renderer2DData::Stats;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer2D::ResetStats() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	Renderer2DData::Stats = {};
}
