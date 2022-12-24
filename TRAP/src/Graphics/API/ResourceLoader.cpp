#include "TRAPPCH.h"
#include "ResourceLoader.h"

#include "Objects/Fence.h"
#include "Objects/Buffer.h"
#include "Objects/CommandBuffer.h"
#include "Objects/CommandPool.h"
#include "Graphics/Textures/Texture.h"
#include "Vulkan/VulkanRenderer.h"
#include "Utils/String/String.h"
#include "FileSystem/FileSystem.h"

#include "Vulkan/VulkanCommon.h"
#include "Vulkan/Objects/VulkanDevice.h"
#include "Vulkan/Objects/VulkanPhysicalDevice.h"
#include "Vulkan/Objects/VulkanCommandBuffer.h"
#include "Vulkan/Objects/VulkanTexture.h"
#include <memory>

TRAP::Graphics::RendererAPI::ResourceLoaderDesc TRAP::Graphics::API::ResourceLoader::DefaultResourceLoaderDesc{8ull << 20, 2};

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t MIP_REDUCE(const uint32_t size, const uint32_t mip) noexcept
{
	return TRAP::Math::Max(1u, size >> mip);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::StreamerThreadFunc(ResourceLoader* const loader)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(loader, "ResourceLoader::StreamerThreadFunc(): ResourceLoader is nullptr!");

#ifdef TRACY_ENABLE
	//Set Thread name for profiler
	tracy::SetThreadName("ResourceLoader");
#endif

	SyncToken maxToken{};

	while(loader->m_run)
	{
		{
			std::unique_lock lock(loader->m_queueMutex);
			auto& mutex = loader->m_queueMutex;
			LockMark(mutex);

			//Check for pending tokens
			//Safe to use m_tokenCounter as we are inside critical section
			const bool allTokensSignaled = (loader->m_tokenCompleted == loader->m_tokenCounter);

			while(!loader->AreTasksAvailable() && allTokensSignaled && loader->m_run)
			{
				//Sleep until someone adds an update request to the queue
				loader->m_queueCond.wait(lock);
			}
		}

		loader->m_nextSet = (loader->m_nextSet + 1) % loader->m_desc.BufferCount;
		loader->WaitCopyEngineSet(loader->m_nextSet);
		loader->ResetCopyEngineSet(loader->m_nextSet);

		//Signal pending tokens from previous frames
		{
			std::lock_guard lock(loader->m_tokenMutex);
			auto& mutex = loader->m_tokenMutex;
			LockMark(mutex);
			loader->m_tokenCompleted = loader->m_currentTokenState[loader->m_nextSet];
		}
		loader->m_tokenCond.notify_all();

		uint64_t completionMask = 0;

		std::vector<UpdateRequest> activeQueue;
		{
			std::lock_guard lock(loader->m_queueMutex);
			auto& mutex = loader->m_queueMutex;
			LockMark(mutex);

			std::vector<UpdateRequest>& requestQueue = loader->m_requestQueue;

			if(!requestQueue.empty())
				std::swap(requestQueue, activeQueue);
		}

		const std::size_t requestCount = activeQueue.size();

		for(std::size_t j = 0; j < requestCount; ++j)
		{
			UpdateRequest& updateState = activeQueue[j];

			UploadFunctionResult result = UploadFunctionResult::Completed;
			switch(updateState.Type)
			{
			case UpdateRequestType::UpdateBuffer:
			{
				result = loader->UpdateBuffer(loader->m_nextSet,
				                              std::get<RendererAPI::BufferUpdateDesc>(updateState.Desc));
				break;
			}

			case UpdateRequestType::UpdateTexture:
			{
				result = loader->UpdateTexture(loader->m_nextSet,
				                               std::get<TextureUpdateDescInternal>(updateState.Desc), nullptr);
				break;
			}

			case UpdateRequestType::BufferBarrier:
			{
				const RendererAPI::BufferBarrier barrier = std::get<RendererAPI::BufferBarrier>(updateState.Desc);
				loader->AcquireCmd(loader->m_nextSet)->ResourceBarrier(&barrier, nullptr, nullptr);
				result = UploadFunctionResult::Completed;
				break;
			}

			case UpdateRequestType::TextureBarrier:
			{
				const RendererAPI::TextureBarrier barrier = std::get<RendererAPI::TextureBarrier>(updateState.Desc);
				loader->AcquireCmd(loader->m_nextSet)->ResourceBarrier(nullptr, &barrier, nullptr);
				result = UploadFunctionResult::Completed;
				break;
			}

			case UpdateRequestType::LoadTexture:
			{
				result = loader->LoadTexture(loader->m_nextSet, updateState);
				break;
			}

			case UpdateRequestType::CopyTexture:
			{
				result = loader->CopyTexture(loader->m_nextSet, std::get<RendererAPI::TextureCopyDesc>(updateState.Desc));
				break;
			}

			case UpdateRequestType::Invalid:
            	[[fallthrough]];
			default:
				break;
			}

			if(updateState.UploadBuffer)
			{
				CopyEngine::CopyResourceSet& resSet = loader->m_copyEngine.ResourceSets[loader->m_nextSet];
				resSet.TempBuffers.push_back(updateState.UploadBuffer);
			}

			const bool completed = result == UploadFunctionResult::Completed ||
			                       result == UploadFunctionResult::InvalidRequest;

			completionMask |= static_cast<uint64_t>(completed);

			if(updateState.WaitIndex && completed)
			{
				TRAP_ASSERT(maxToken < updateState.WaitIndex, "ResourceLoader::StreamerThreadFunc(): Max sync token is smaller than current sync token!");
				maxToken = updateState.WaitIndex;
			}

			TRAP_ASSERT(result != UploadFunctionResult::StagingBufferFull, "ResourceLoader::StreamerThreadFunc() Staging buffer is full!");
		}

		if(completionMask != 0)
			loader->StreamerFlush(loader->m_nextSet);

		const SyncToken nextToken = Math::Max<SyncToken>(maxToken, loader->GetLastTokenCompleted());
		loader->m_currentTokenState[loader->m_nextSet] = nextToken;
	}

	loader->StreamerFlush(loader->m_nextSet);
	loader->m_copyEngine.Queue->WaitQueueIdle();
	loader->CleanupCopyEngine();

	loader->FreeAllUploadMemory();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint64_t TRAP::Graphics::API::ResourceLoader::UtilGetTextureRowAlignment() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return TRAP::Math::Max(static_cast<uint64_t>(1), RendererAPI::GPUSettings.UploadBufferTextureRowAlignment);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint64_t TRAP::Graphics::API::ResourceLoader::UtilGetTextureSubresourceAlignment(const TRAP::Graphics::API::ImageFormat fmt) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const uint64_t blockSize = Math::Max(static_cast<uint64_t>(1), static_cast<uint64_t>(TRAP::Graphics::API::ImageFormatBitSizeOfBlock(fmt) >> 3));
	const uint64_t alignment = ((RendererAPI::GPUSettings.UploadBufferTextureAlignment + blockSize - 1) /
	                            blockSize) * blockSize;

	const uint64_t rowAlignment = UtilGetTextureRowAlignment();
	return ((alignment + rowAlignment - 1) / rowAlignment) * rowAlignment;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint64_t TRAP::Graphics::API::ResourceLoader::UtilGetSurfaceSize(const TRAP::Graphics::API::ImageFormat fmt,
                                                                               const uint32_t width, const uint32_t height,
																               const uint32_t depth, const uint64_t rowStride,
																               const uint64_t sliceStride,
																               const uint32_t baseMipLevel,
																               const uint32_t mipLevels,
																               const uint32_t baseArrayLayer,
																               const uint32_t arrayLayers) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	uint64_t requiredSize = 0;

	for(uint32_t s = baseArrayLayer; s < baseArrayLayer + arrayLayers; ++s)
	{
		uint32_t w = width;
		uint32_t h = height;
		uint32_t d = depth;

		for(uint32_t m = baseMipLevel; m < baseMipLevel + mipLevels; ++m)
		{
			uint64_t rowBytes = 0;
			uint64_t numRows = 0;

			if(!UtilGetSurfaceInfo(w, h, fmt, nullptr, &rowBytes, &numRows))
				return false;

			const uint64_t temp = ((rowBytes + rowStride - 1) / rowStride) * rowStride;
			requiredSize += (((d * temp * numRows) + sliceStride - 1) / sliceStride) * sliceStride;

			w = w >> 1;
			h = h >> 1;
			d = d >> 1;
			if(w == 0)
				w = 1;
			if(h == 0)
				h = 1;
			if(d == 0)
				d = 1;
		}
	}

	return requiredSize;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::API::ResourceLoader::UtilGetSurfaceInfo(const uint32_t width, const uint32_t height,
	                                                                       const TRAP::Graphics::API::ImageFormat fmt,
															               uint64_t* const outNumBytes, uint64_t* const outRowBytes,
															               uint64_t* const outNumRows)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	uint64_t numBytes = 0;
	uint64_t rowBytes = 0;
	uint64_t numRows = 0;

	const uint32_t bpp = TRAP::Graphics::API::ImageFormatBitSizeOfBlock(fmt);
	const bool compressed = TRAP::Graphics::API::ImageFormatIsCompressed(fmt);
	const bool planar = TRAP::Graphics::API::ImageFormatIsPlanar(fmt);

	const bool packed = false; //TODO

	if(compressed)
	{
		const uint32_t blockWidth = TRAP::Graphics::API::ImageFormatWidthOfBlock(fmt);
		const uint32_t blockHeight = TRAP::Graphics::API::ImageFormatHeightOfBlock(fmt);
		uint32_t numBlocksWide = 0;
		uint32_t numBlocksHigh = 0;
		if(width > 0)
			numBlocksWide = Math::Max(1u, (width + (blockWidth - 1)) / blockWidth);
		if(height > 0)
			numBlocksHigh = Math::Max(1u, (height + (blockHeight - 1)) / blockHeight);

		rowBytes = static_cast<uint64_t>(numBlocksWide) * (bpp >> 3u);
		numRows = numBlocksHigh;
		numBytes = rowBytes * numBlocksHigh;
	}
	else if(packed)
	{
		TP_ERROR(Log::TexturePrefix, "Packed not implemented!");
		return false;
	}
	else if(planar)
	{
		const uint32_t numOfPlanes = TRAP::Graphics::API::ImageFormatNumOfPlanes(fmt);

		for(uint32_t i = 0; i < numOfPlanes; ++i)
			numBytes += static_cast<uint64_t>(TRAP::Graphics::API::ImageFormatPlaneWidth(fmt, i, width)) *
			            TRAP::Graphics::API::ImageFormatPlaneHeight(fmt, i, height) *
						TRAP::Graphics::API::ImageFormatPlaneSizeOfBlock(fmt, i);

		numRows = 1;
		rowBytes = numBytes;
	}
	else
	{
		if(!bpp)
			return false;

		rowBytes = (static_cast<uint64_t>(width) * bpp + 7u) / 8u; //Round up to nearest byte
		numRows = static_cast<uint64_t>(height);
		numBytes = rowBytes * height;
	}

	if(numBytes > std::numeric_limits<uint32_t>::max() ||
	   rowBytes > std::numeric_limits<uint32_t>::max() ||
	   numRows > std::numeric_limits<uint32_t>::max())
		return false;

	if(outNumBytes)
		*outNumBytes = numBytes;
	if(outRowBytes)
		*outRowBytes = rowBytes;
	if(outNumRows)
		*outNumRows = numRows;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::ResourceLoader(const RendererAPI::ResourceLoaderDesc* const desc)
	: m_desc(desc ? *desc : DefaultResourceLoaderDesc),
	  m_run(true),
	  m_tokenCompleted(0),
	  m_tokenCounter(0),
	  m_currentTokenState(),
      m_nextSet()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	SetupCopyEngine();

	//Create dedicated resource loader thread.
	m_thread = std::thread(StreamerThreadFunc, this);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::~ResourceLoader()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	m_run = false;

	m_queueCond.notify_one();
	m_thread.join();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::AddResource(RendererAPI::BufferLoadDesc& desc, SyncToken* const token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const uint64_t stagingBufferSize = m_copyEngine.BufferSize;
	const bool update = desc.Data || desc.ForceReset;

	TRAP_ASSERT(stagingBufferSize > 0, "ResourceLoader::AddResource(): Staging buffer size is 0!");
	if(desc.Desc.MemoryUsage == RendererAPI::ResourceMemoryUsage::GPUOnly &&
	   !static_cast<uint32_t>(desc.Desc.StartState) && !update)
	{
		desc.Desc.StartState = UtilDetermineResourceStartState(desc.Desc);
		TP_WARN(Log::RendererBufferPrefix, "Buffer start state not provided. Determined the start state as (",
		        static_cast<uint32_t>(desc.Desc.StartState), ") based on the provided BufferDesc");
	}

	if (desc.Desc.MemoryUsage == RendererAPI::ResourceMemoryUsage::GPUOnly && update)
		desc.Desc.StartState = RendererAPI::ResourceState::Common;

	desc.Buffer = TRAP::Graphics::Buffer::Create(desc.Desc);

	if(update)
	{
		if (desc.Desc.Size > stagingBufferSize &&
			desc.Desc.MemoryUsage == RendererAPI::ResourceMemoryUsage::GPUOnly)
		{
			//The data is too large for a single staging buffer copy, so perform it in stages.

			//Save the data parameter so we can restore it later.
			const void* const data = desc.Data;

			RendererAPI::BufferUpdateDesc updateDesc{};
			updateDesc.Buffer = desc.Buffer;
			for(uint64_t offset = 0; offset < desc.Desc.Size; offset += stagingBufferSize)
			{
				const std::size_t chunkSize = Math::Min<std::size_t>(stagingBufferSize, desc.Desc.Size - offset);
				updateDesc.Size = chunkSize;
				updateDesc.DstOffset = offset;
				BeginUpdateResource(updateDesc);
				if (desc.ForceReset)
					std::fill_n(static_cast<uint8_t*>(updateDesc.MappedData), chunkSize, static_cast<uint8_t>(0u));
				else
				{
					TRAP_ASSERT(data, "ResourceLoader::AddResource(): Data is nullptr!");
					std::copy_n(static_cast<const uint8_t*>(data) + offset, chunkSize, static_cast<uint8_t*>(updateDesc.MappedData));
				}
				EndUpdateResource(updateDesc, token);
			}
		}
		else
		{
			RendererAPI::BufferUpdateDesc updateDesc{};
			updateDesc.Buffer = desc.Buffer;
			BeginUpdateResource(updateDesc);
			if (desc.ForceReset)
				std::fill_n(static_cast<uint8_t*>(updateDesc.MappedData), desc.Desc.Size, static_cast<uint8_t>(0u));
			else
			{
				TRAP_ASSERT(!desc.Desc.Size || desc.Data, "ResourceLoader::AddResource(): Data is nullptr!");
				if(desc.Data)
					std::copy_n(static_cast<const uint8_t*>(desc.Data), desc.Desc.Size, static_cast<uint8_t*>(updateDesc.MappedData));
			}
			EndUpdateResource(updateDesc, token);
		}
	}
	else
	{
		//Transition GPU buffer to desired state for Vulkan since all Vulkan resources are create in undefined state
		if (RendererAPI::GetRenderAPI() == RenderAPI::Vulkan &&
			desc.Desc.MemoryUsage == RendererAPI::ResourceMemoryUsage::GPUOnly &&
			//Check whether this is required (user specified a state other than undefined / common)
			(desc.Desc.StartState != RendererAPI::ResourceState::Undefined &&
			 desc.Desc.StartState != RendererAPI::ResourceState::Common))
			QueueBufferBarrier(desc.Buffer, desc.Desc.StartState, token);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::AddResource(RendererAPI::TextureLoadDesc& textureDesc, SyncToken* const token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(textureDesc.Texture, "ResourceLoader::AddResource(): Texture is nullptr!");

	if(textureDesc.Filepaths[0].empty() && !textureDesc.Images[0] && textureDesc.Desc)
	{
		TRAP_ASSERT(static_cast<uint32_t>(textureDesc.Desc->StartState), "ResourceLoader::AddResource(): Texture start state is undefined!");
		TRAP_ASSERT(textureDesc.Texture != nullptr, "ResourceLoader::AddResource(): Texture must be constructed before loading");

		//If texture is supposed to be filled later (UAV / Update later / ...) proceed with the StartState provided
		//by the user in the texture description
		textureDesc.Texture->Init(*textureDesc.Desc);

		//Transition texture to desired state for Vulkan since all Vulkan resources are created in undefined state
		if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
		{
			RendererAPI::ResourceState startState = textureDesc.Desc->StartState;
			//Check whether this is required (user specified a state other than undefined / common)
			if(startState == RendererAPI::ResourceState::Undefined ||
			   startState == RendererAPI::ResourceState::Common)
				startState = UtilDetermineResourceStartState(static_cast<uint32_t>(textureDesc.Desc->Descriptors) &
				                                             static_cast<uint32_t>(RendererAPI::DescriptorType::RWTexture));
			QueueTextureBarrier(textureDesc.Texture, startState, token);
		}
	}
	else
		QueueTextureLoad(textureDesc, token);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::BeginUpdateResource(RendererAPI::BufferUpdateDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const TRAP::Ref<Buffer>& buffer = desc.Buffer;
	TRAP_ASSERT(buffer, "ResourceLoader::BeginUpdateResource(): Buffer is nullptr!");

	const uint64_t size = desc.Size > 0 ? desc.Size : (desc.Buffer->GetSize() - desc.DstOffset);
	TRAP_ASSERT(desc.DstOffset + size <= buffer->GetSize(), "ResourceLoader::BeginUpdateResource(): Update size is larger than buffer size!");

	const RendererAPI::ResourceMemoryUsage memoryUsage = desc.Buffer->GetMemoryUsage();
	if(memoryUsage != RendererAPI::ResourceMemoryUsage::GPUOnly)
	{
		const bool map = !buffer->GetCPUMappedAddress();
		if (map)
			buffer->MapBuffer(nullptr);

		desc.Internal.MappedRange = { static_cast<uint8_t*>(buffer->GetCPUMappedAddress()) + desc.DstOffset,
		                              buffer };
		desc.MappedData = desc.Internal.MappedRange.Data;
		desc.Internal.MappedRange.Flags = map ? static_cast<uint32_t>(MappedRangeFlag::UnMapBuffer) : 0;
	}
	else
	{
		//We need to use a staging buffer.
		const RendererAPI::MappedMemoryRange range = AllocateUploadMemory(size, 4U);
		desc.MappedData = range.Data;

		desc.Internal.MappedRange = range;
		desc.Internal.MappedRange.Flags = static_cast<uint32_t>(MappedRangeFlag::TempBuffer);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::BeginUpdateResource(RendererAPI::TextureUpdateDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const TRAP::Graphics::Texture* texture = desc.Texture;
	const TRAP::Graphics::API::ImageFormat fmt = texture->GetImageFormat();
	const uint64_t alignment = UtilGetTextureSubresourceAlignment(fmt);

	const bool success = UtilGetSurfaceInfo(MIP_REDUCE(texture->GetWidth(), desc.MipLevel),
	                                        MIP_REDUCE(texture->GetHeight(), desc.MipLevel),
	                                        fmt, &desc.SrcSliceStride, &desc.SrcRowStride, &desc.RowCount);
	TRAP_ASSERT(success, "ResourceLoader::BeginUpdateResource(): Failed to retrieve texture surface info!");

	const uint64_t rowAlignment = UtilGetTextureRowAlignment();
	desc.DstRowStride = ((desc.SrcRowStride + rowAlignment - 1) / rowAlignment) * rowAlignment;
	desc.DstSliceStride = (((desc.DstRowStride * desc.RowCount) + alignment - 1) / alignment) * alignment;

	const uint64_t requiredSize = ((MIP_REDUCE(texture->GetDepth(), desc.MipLevel) *
	                               desc.DstSliceStride + alignment - 1) / alignment) * alignment;

	//We need to use a staging buffer
	desc.Internal.MappedRange = AllocateUploadMemory(requiredSize, static_cast<uint32_t>(alignment));
	desc.Internal.MappedRange.Flags = static_cast<uint32_t>(MappedRangeFlag::TempBuffer);
	desc.MappedData = desc.Internal.MappedRange.Data;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::EndUpdateResource(RendererAPI::BufferUpdateDesc& desc, SyncToken* const token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if (desc.Internal.MappedRange.Flags & static_cast<uint32_t>(MappedRangeFlag::UnMapBuffer))
		desc.Buffer->UnMapBuffer();

	const RendererAPI::ResourceMemoryUsage memoryUsage = desc.Buffer->GetMemoryUsage();
	if (memoryUsage == RendererAPI::ResourceMemoryUsage::GPUOnly)
		QueueBufferUpdate(desc, token);

	//Restore the state to before the BeginUpdateResource call.
	desc.MappedData = nullptr;
	desc.Internal = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::EndUpdateResource(RendererAPI::TextureUpdateDesc& desc, SyncToken* const token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TextureUpdateDescInternal internalDesc = {};
	internalDesc.Texture = desc.Texture;
	internalDesc.Range = desc.Internal.MappedRange;
	internalDesc.BaseMipLevel = desc.MipLevel;
	internalDesc.MipLevels = 1;
	internalDesc.BaseArrayLayer = desc.ArrayLayer;
	internalDesc.LayerCount = 1;
	QueueTextureUpdate(internalDesc, token);

	//Restore the state to before the BeginUpdateResource call.
	desc.MappedData = nullptr;
	desc.Internal = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::CopyResource(RendererAPI::TextureCopyDesc &textureDesc, SyncToken *token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	QueueTextureCopy(textureDesc, token);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::ResourceLoader::AllResourceLoadsCompleted() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const SyncToken token = m_tokenCounter;
	return token <= m_tokenCompleted;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::WaitForAllResourceLoads()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const SyncToken token = m_tokenCounter;
	WaitForToken(&token);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::SyncToken TRAP::Graphics::API::ResourceLoader::GetLastTokenCompleted() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_tokenCompleted;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::API::ResourceLoader::IsTokenCompleted(const SyncToken* const token) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return *token <= m_tokenCompleted;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::WaitForToken(const SyncToken* const token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	std::unique_lock lock(m_tokenMutex);
	LockMark(m_tokenMutex);
	while (!IsTokenCompleted(token))
		m_tokenCond.wait(lock);
}

//-------------------------------------------------------------------------------------------------------------------//

//Return a new staging buffer
[[nodiscard]] TRAP::Graphics::RendererAPI::MappedMemoryRange TRAP::Graphics::API::ResourceLoader::AllocateUploadMemory(const uint64_t memoryRequirement,
                                                                                                                       const uint32_t alignment)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	RendererAPI::BufferDesc desc{};
	desc.Size = memoryRequirement;
	desc.Alignment = alignment;
	desc.MemoryUsage = RendererAPI::ResourceMemoryUsage::CPUOnly;
	desc.Flags = RendererAPI::BufferCreationFlags::PersistentMap;
	const TRAP::Ref<Buffer> buffer = Buffer::Create(desc);

	return RendererAPI::MappedMemoryRange{static_cast<uint8_t*>(buffer->GetCPUMappedAddress()), buffer,
	                                      0, memoryRequirement };
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::QueueBufferBarrier(const TRAP::Ref<Buffer>& buffer,
                                                             const RendererAPI::ResourceState state,
															 SyncToken* const token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	SyncToken t = 0;
	{
		std::lock_guard lock(m_queueMutex);
		LockMark(m_queueMutex);

		t = m_tokenCounter + 1;
		++m_tokenCounter;

		m_requestQueue.emplace_back(UpdateRequest{ RendererAPI::BufferBarrier{buffer,
		                                                                      RendererAPI::ResourceState::Undefined,
																			  state} });
		m_requestQueue.back().WaitIndex = t;
	}
	m_queueCond.notify_one();

	if (token)
		*token = Math::Max(t, *token);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::QueueBufferUpdate(const RendererAPI::BufferUpdateDesc& desc,
                                                            SyncToken* const token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	SyncToken t{};
	{
		std::lock_guard lock(m_queueMutex);
		LockMark(m_queueMutex);

		t = m_tokenCounter + 1;
		++m_tokenCounter;

		m_requestQueue.emplace_back(UpdateRequest(desc));
		m_requestQueue.back().WaitIndex = t;
		m_requestQueue.back().UploadBuffer = (desc.Internal.MappedRange.Flags &
		                                      static_cast<uint32_t>(MappedRangeFlag::TempBuffer)) ?
											 desc.Internal.MappedRange.Buffer : nullptr;
	}
	m_queueCond.notify_one();

	if (token)
		*token = Math::Max(t, *token);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::QueueTextureLoad(const RendererAPI::TextureLoadDesc& desc,
											               SyncToken* const token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	SyncToken t{};
	{
		std::lock_guard lock(m_queueMutex);
		LockMark(m_queueMutex);

		t = m_tokenCounter + 1;
		++m_tokenCounter;

		m_requestQueue.emplace_back(UpdateRequest(desc));
		m_requestQueue.back().WaitIndex = t;
	}
	m_queueCond.notify_one();

	if(token)
		*token = Math::Max(t, *token);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::QueueTextureUpdate(const TextureUpdateDescInternal& textureUpdate,
															 SyncToken* const token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(textureUpdate.Range.Buffer, "ResourceLoader::QueueTextureUpdate(): Texture update buffer is nullptr!");

	SyncToken t{};
	{
		std::lock_guard lock(m_queueMutex);
		LockMark(m_queueMutex);

		t = m_tokenCounter + 1;
		++m_tokenCounter;

		m_requestQueue.emplace_back(UpdateRequest(textureUpdate));
		m_requestQueue.back().WaitIndex = t;
		m_requestQueue.back().UploadBuffer = (textureUpdate.Range.Flags &
		                                      static_cast<uint32_t>(MappedRangeFlag::TempBuffer) ?
											 textureUpdate.Range.Buffer : nullptr);
	}
	m_queueCond.notify_one();

	if(token)
		*token = Math::Max(t, *token);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::QueueTextureCopy(const RendererAPI::TextureCopyDesc& desc, SyncToken* const token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	SyncToken t{};
	{
		std::lock_guard lock(m_queueMutex);
		LockMark(m_queueMutex);

		t = m_tokenCounter + 1;
		++m_tokenCounter;

		m_requestQueue.emplace_back(UpdateRequest(desc));
		m_requestQueue.back().WaitIndex = t;
	}
	m_queueCond.notify_one();

	if(token)
		*token = Math::Max(t, *token);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::QueueTextureBarrier(TRAP::Graphics::Texture* const texture,
                                                              const RendererAPI::ResourceState state, SyncToken* const token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	SyncToken t{};
	{
		std::lock_guard lock(m_queueMutex);
		LockMark(m_queueMutex);

		t = m_tokenCounter + 1;
		++m_tokenCounter;

		m_requestQueue.emplace_back(UpdateRequest{ RendererAPI::TextureBarrier{texture,
		                                                                       RendererAPI::ResourceState::Undefined,
																			   state} });
		m_requestQueue.back().WaitIndex = t;
	}
	m_queueCond.notify_one();

	if (token)
		*token = Math::Max(t, *token);
}

//-------------------------------------------------------------------------------------------------------------------//

//Return memory from pre-allocated staging buffer or create a temporary buffer if the streamer ran out of memory
[[nodiscard]] TRAP::Graphics::RendererAPI::MappedMemoryRange TRAP::Graphics::API::ResourceLoader::AllocateStagingMemory(const uint64_t memoryRequirement,
                                                                                                                        const uint32_t alignment)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	uint64_t offset = m_copyEngine.ResourceSets[m_nextSet].AllocatedSpace;
	if(alignment != 0)
		offset = ((offset + static_cast<uint64_t>(alignment) - 1) / static_cast<uint64_t>(alignment)) *
		         static_cast<uint64_t>(alignment);

	CopyEngine::CopyResourceSet& resSet = m_copyEngine.ResourceSets[m_nextSet];
	const uint64_t size = static_cast<uint64_t>(resSet.Buffer->GetSize());
	const bool memoryAvailable = (offset < size) && (memoryRequirement <= size - offset);
	if(memoryAvailable && resSet.Buffer->GetCPUMappedAddress())
	{
		const TRAP::Ref<Buffer> buffer = resSet.Buffer;
		TRAP_ASSERT(buffer->GetCPUMappedAddress(), "ResourceLoader::AllocateStagingMemory(): CPU mapped address of buffer is nullptr!");
		uint8_t* const dstData = static_cast<uint8_t*>(buffer->GetCPUMappedAddress()) + offset;
		m_copyEngine.ResourceSets[m_nextSet].AllocatedSpace = offset + memoryRequirement;
		return { dstData, std::move(buffer), offset, memoryRequirement };
	}

	if(m_copyEngine.BufferSize < memoryRequirement)
	{
		const RendererAPI::MappedMemoryRange range = AllocateUploadMemory(memoryRequirement, alignment);
		resSet.TempBuffers.emplace_back(range.Buffer);
		return range;
	}

	const RendererAPI::MappedMemoryRange range = AllocateUploadMemory(memoryRequirement, alignment);
	resSet.TempBuffers.emplace_back(range.Buffer);
	return range;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::FreeAllUploadMemory() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	for(UpdateRequest& request : m_requestQueue)
	{
		if (request.UploadBuffer)
			request.UploadBuffer.reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::SetupCopyEngine()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	//TODO Use Transfer Queue if texture already has mip maps inside it (they don't need to be generated)
	//     Graphics Queue is required in order to use vkCmdBlitImage
	/*const RendererAPI::QueueDesc desc{ RendererAPI::QueueType::Transfer, RendererAPI::QueueFlag::None,
	                                     RendererAPI::QueuePriority::Normal };*/
	const RendererAPI::QueueDesc desc{ RendererAPI::QueueType::Graphics, RendererAPI::QueueFlag::None,
	                                   RendererAPI::QueuePriority::Normal };
	m_copyEngine.Queue = Queue::Create(desc);

	static constexpr uint64_t maxBlockSize = 32;
	const uint64_t size = Math::Max(m_desc.BufferSize, maxBlockSize);

	m_copyEngine.ResourceSets.resize(m_desc.BufferCount);
	for(uint32_t i = 0; i < m_desc.BufferCount; ++i)
	{
		auto& cpyResSet = m_copyEngine.ResourceSets[i];

		cpyResSet.Fence = Fence::Create();
		RendererAPI::CommandPoolDesc cmdPoolDesc{};
		cmdPoolDesc.Queue = m_copyEngine.Queue;
		cpyResSet.CommandPool = CommandPool::Create(cmdPoolDesc);

		cpyResSet.Cmd = cpyResSet.CommandPool->AllocateCommandBuffer(false);

		cpyResSet.Buffer = AllocateUploadMemory(size, static_cast<uint32_t>(UtilGetTextureSubresourceAlignment())).Buffer;
	}

	m_copyEngine.BufferSize = size;
	m_copyEngine.BufferCount = m_desc.BufferCount;
	m_copyEngine.IsRecording = false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::CleanupCopyEngine()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	for(uint32_t i = 0+1; i < m_copyEngine.BufferCount; ++i)
	{
		CopyEngine::CopyResourceSet& resourceSet = m_copyEngine.ResourceSets[i];
		resourceSet.Buffer.reset();

		resourceSet.CommandPool->FreeCommandBuffer(resourceSet.Cmd);
		resourceSet.Cmd = nullptr;
		resourceSet.CommandPool.reset();

		resourceSet.Fence.reset();

		if (!resourceSet.TempBuffers.empty())
			TP_WARN(Log::RendererBufferPrefix, "A temporary buffer was not cleaned up ", i);

		resourceSet.TempBuffers.clear();
	}

	m_copyEngine.ResourceSets.clear();

	m_copyEngine.Queue.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::WaitCopyEngineSet(const std::size_t activeSet) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(!m_copyEngine.IsRecording, "ResourceLoader::WaitCopyEngineSet(): Copy Engine is still recording!");
	const CopyEngine::CopyResourceSet& resSet = m_copyEngine.ResourceSets[activeSet];

	if (resSet.Fence->GetStatus() == RendererAPI::FenceStatus::Incomplete)
		resSet.Fence->Wait();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::ResetCopyEngineSet(const std::size_t activeSet)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(!m_copyEngine.IsRecording, "ResourceLoader::ResetCopyEngineSet(): Copy Engine is still recording!");

	m_copyEngine.ResourceSets[activeSet].AllocatedSpace = 0;
	m_copyEngine.IsRecording = false;

	m_copyEngine.ResourceSets[activeSet].TempBuffers.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::CommandBuffer* TRAP::Graphics::API::ResourceLoader::AcquireCmd(const std::size_t activeSet)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const CopyEngine::CopyResourceSet& resSet = m_copyEngine.ResourceSets[activeSet];
	if(!m_copyEngine.IsRecording)
	{
		resSet.CommandPool->Reset();
		resSet.Cmd->Begin();
		m_copyEngine.IsRecording = true;
	}

	return resSet.Cmd;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::StreamerFlush(const std::size_t activeSet)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(!m_copyEngine.IsRecording)
		return;

	const CopyEngine::CopyResourceSet& resSet = m_copyEngine.ResourceSets[activeSet];
	resSet.Cmd->End();
	RendererAPI::QueueSubmitDesc submitDesc{};
	submitDesc.Cmds.push_back(resSet.Cmd);

	submitDesc.SignalFence = resSet.Fence;

	if(!m_copyEngine.WaitSemaphores.empty())
	{
		submitDesc.WaitSemaphores = m_copyEngine.WaitSemaphores;
		m_copyEngine.WaitSemaphores.clear();
	}

	{
		m_copyEngine.Queue->Submit(submitDesc);
	}
	m_copyEngine.IsRecording = false;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::API::ResourceLoader::AreTasksAvailable() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return !m_requestQueue.empty();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::VulkanGenerateMipMaps(TRAP::Graphics::Texture* const texture,
                                                                CommandBuffer* const cmd)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	//Check if image format supports linear blitting
	const TRAP::Graphics::API::VulkanRenderer* const vkRenderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>
		(
			RendererAPI::GetRenderer()
		);
	const VkFormatProperties formatProperties = vkRenderer->GetDevice()->GetPhysicalDevice()->GetVkPhysicalDeviceFormatProperties(TRAP::Graphics::API::ImageFormatToVkFormat(texture->GetImageFormat()));

	if(!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		TP_ERROR(Log::RendererVulkanTexturePrefix, "Texture image format does not support linear blitting! "
		         "Skipping mip map generation");
		return;
	}

	RendererAPI::TextureBarrier barrier{};
	barrier.Texture = texture;
	barrier.ArrayLayer = 0;
	barrier.SubresourceBarrier = true;

	int32_t mipWidth = static_cast<int32_t>(texture->GetWidth());
	int32_t mipHeight = static_cast<int32_t>(texture->GetHeight());

	for(uint32_t i = 1; i < texture->GetMipLevels(); ++i)
	{
		barrier.MipLevel = static_cast<uint8_t>(i) - 1;
		barrier.CurrentState = RendererAPI::ResourceState::CopyDestination;
		barrier.NewState = RendererAPI::ResourceState::CopySource;

		cmd->ResourceBarrier(nullptr, &barrier, nullptr);

		VkImageBlit blit{};
		blit.srcOffsets[0] = {0, 0, 0};
		blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = {0, 0, 0};
		blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		const auto* const vkTexture = dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(texture);
		const auto* const vkCmd = dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>(cmd);

		vkCmdBlitImage(vkCmd->GetVkCommandBuffer(),
			           vkTexture->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			           vkTexture->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			           1, &blit,
			           VK_FILTER_LINEAR);

		barrier.CurrentState = RendererAPI::ResourceState::CopySource;
		barrier.NewState = RendererAPI::ResourceState::CopyDestination;

		cmd->ResourceBarrier(nullptr, &barrier, nullptr);

		if(mipWidth > 1)
			mipWidth /= 2;
		if(mipHeight > 1)
			mipHeight /= 2;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::ResourceLoader::UploadFunctionResult TRAP::Graphics::API::ResourceLoader::UpdateBuffer(const std::size_t activeSet,
																											              const RendererAPI::BufferUpdateDesc& bufferUpdateDesc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const TRAP::Ref<Buffer>& buffer = bufferUpdateDesc.Buffer;
	TRAP_ASSERT(buffer->GetMemoryUsage() == RendererAPI::ResourceMemoryUsage::GPUOnly ||
		        buffer->GetMemoryUsage() == RendererAPI::ResourceMemoryUsage::GPUToCPU, "ResourceLoader::UpdateBuffer(): Buffer must have memory usage GPUOnly or GPUToCPU!");

	const CommandBuffer* const cmd = AcquireCmd(activeSet);

	const RendererAPI::MappedMemoryRange range = bufferUpdateDesc.Internal.MappedRange;
	cmd->UpdateBuffer(buffer, bufferUpdateDesc.DstOffset, range.Buffer, range.Offset, range.Size);

	return UploadFunctionResult::Completed;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::ResourceLoader::UploadFunctionResult TRAP::Graphics::API::ResourceLoader::UpdateTexture(const std::size_t activeSet,
																											               const TextureUpdateDescInternal& textureUpdateDesc,
																											               const std::array<const TRAP::Image*, 6>* const images)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	//When this call comes from UpdateResource, staging buffer data is already filled
	//All that is left to do is record and execute the Copy commands
	const bool dataAlreadyFilled = textureUpdateDesc.Range.Buffer != nullptr;
	TRAP::Graphics::Texture* const texture = textureUpdateDesc.Texture;
	const TRAP::Graphics::API::ImageFormat format = texture->GetImageFormat();
	CommandBuffer* const cmd = AcquireCmd(activeSet);

	const uint64_t sliceAlignment = UtilGetTextureSubresourceAlignment(format);
	const uint64_t rowAlignment = UtilGetTextureRowAlignment();
	const uint64_t requiredSize = UtilGetSurfaceSize(format, texture->GetWidth(), texture->GetHeight(),
											         texture->GetDepth(), rowAlignment, sliceAlignment,
													 textureUpdateDesc.BaseMipLevel, textureUpdateDesc.MipLevels,
													 textureUpdateDesc.BaseArrayLayer, textureUpdateDesc.LayerCount);

	if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
	{
		const RendererAPI::TextureBarrier barrier{texture, RendererAPI::ResourceState::Undefined,
		                                    RendererAPI::ResourceState::CopyDestination};
		cmd->ResourceBarrier(nullptr, &barrier, nullptr);
	}

	const RendererAPI::MappedMemoryRange upload = dataAlreadyFilled ? textureUpdateDesc.Range :
		                                              AllocateStagingMemory(requiredSize, static_cast<uint32_t>(sliceAlignment));
	uint64_t offset = 0;

	if(!upload.Data)
		return UploadFunctionResult::StagingBufferFull;

	const uint32_t firstStart = textureUpdateDesc.MipsAfterSlice ? textureUpdateDesc.BaseMipLevel :
		                            textureUpdateDesc.BaseArrayLayer;
	const uint32_t firstEnd = textureUpdateDesc.MipsAfterSlice ?
		                          (textureUpdateDesc.BaseMipLevel + textureUpdateDesc.MipLevels) :
		                          (textureUpdateDesc.BaseArrayLayer + textureUpdateDesc.LayerCount);
	const uint32_t secondStart = textureUpdateDesc.MipsAfterSlice ? textureUpdateDesc.BaseArrayLayer :
		                             textureUpdateDesc.BaseMipLevel;
	const uint32_t secondEnd = textureUpdateDesc.MipsAfterSlice ?
		                           (textureUpdateDesc.BaseArrayLayer + textureUpdateDesc.LayerCount) :
		                           (textureUpdateDesc.BaseMipLevel + textureUpdateDesc.MipLevels);

	for(uint32_t j = firstStart; j < firstEnd; ++j)
	{
		for(uint32_t i = secondStart; i < secondEnd; ++i)
		{
			const uint32_t mip = textureUpdateDesc.MipsAfterSlice ? j : i; //TODO For now only upload mip level 0 and use the Vulkan way to blit
			                                                         //them to their mips till I know that Compute Shaders work
			const uint32_t layer = textureUpdateDesc.MipsAfterSlice ? i : j;

			const uint32_t w = MIP_REDUCE(texture->GetWidth(), mip);
			const uint32_t h = MIP_REDUCE(texture->GetHeight(), mip);
			const uint32_t d = MIP_REDUCE(texture->GetDepth(), mip);

			uint64_t numBytes = 0;
			uint64_t rowBytes = 0;
			uint64_t numRows = 0;

			const bool ret = UtilGetSurfaceInfo(w, h, format, &numBytes, &rowBytes, &numRows);
			if(!ret)
				return UploadFunctionResult::InvalidRequest;

			const uint64_t subRowPitch = ((rowBytes + rowAlignment - 1) / rowAlignment) * rowAlignment;
			const uint64_t subSlicePitch = (((subRowPitch * numRows) + sliceAlignment - 1) / sliceAlignment) * sliceAlignment;
			const uint64_t subNumRows = numRows;
			const uint32_t subDepth = d;
			const uint64_t subRowSize = rowBytes;
			uint8_t* const data = upload.Data + offset;

			if(!dataAlreadyFilled)
			{
				for(uint32_t z = 0; z < subDepth; ++z)
				{
					//Convert to RGBA if necessary
					const uint8_t* pixelData = nullptr;
					TRAP::Scope<TRAP::Image> RGBAImage = nullptr;
					if((*images)[layer]->GetColorFormat() == TRAP::Image::ColorFormat::RGB) //Convert RGB to RGBA
					{
						RGBAImage = TRAP::Image::ConvertRGBToRGBA((*images)[layer]);
						pixelData = static_cast<const uint8_t*>(RGBAImage->GetPixelData());
					}
					else
						pixelData = static_cast<const uint8_t*>((*images)[layer]->GetPixelData());

					uint8_t* const dstData = data + subSlicePitch * z;
					for(uint64_t r = 0; r < subNumRows; ++r)
						std::copy_n(pixelData + r * subRowSize, subRowSize, dstData + r * subRowPitch);
				}
			}

			RendererAPI::SubresourceDataDesc subresourceDesc = {};
			subresourceDesc.ArrayLayer = layer;
			subresourceDesc.MipLevel = mip;
			subresourceDesc.SrcOffset = upload.Offset + offset;
			if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
			{
				subresourceDesc.RowPitch = static_cast<uint32_t>(subRowPitch);
				subresourceDesc.SlicePitch = static_cast<uint32_t>(subSlicePitch);
			}

			cmd->UpdateSubresource(texture, upload.Buffer, subresourceDesc);
			offset += subDepth * subSlicePitch;
		}
	}

	if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan && texture->GetMipLevels() > 1)
		VulkanGenerateMipMaps(texture, cmd); //Mipmapping via vkCmdBlitImage
	//D3D12/DirectX 12 would need a compute shader to generate mip maps (as there is no vkCmdBlitImage equivalent)
	//https://github.com/GPUOpen-Effects/FidelityFX-SPD/blob/master/sample/src/VK/CSDownsampler.glsl

	if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
	{
		const RendererAPI::ResourceState finalLayout = UtilDetermineResourceStartState(static_cast<bool>(texture->GetDescriptorTypes() & RendererAPI::DescriptorType::RWTexture));
		const RendererAPI::TextureBarrier barrier{texture, RendererAPI::ResourceState::CopyDestination,
		                                          finalLayout};
		cmd->ResourceBarrier(nullptr, &barrier, nullptr);
	}

	return UploadFunctionResult::Completed;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::ResourceLoader::UploadFunctionResult TRAP::Graphics::API::ResourceLoader::LoadTexture(const std::size_t activeSet,
																										                 TRAP::Graphics::API::ResourceLoader::UpdateRequest& textureUpdate)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const TRAP::Graphics::RendererAPI::TextureLoadDesc& textureLoadDesc = std::get<TRAP::Graphics::RendererAPI::TextureLoadDesc>
		(
			textureUpdate.Desc
		);

	bool validMultiFileCubemap = true;
	if(!textureLoadDesc.Images[0])
	{
		//Use normal file loading
		for(const std::filesystem::path& str : textureLoadDesc.Filepaths)
		{
			if(str.empty() || !TRAP::FileSystem::Exists(str) || !TRAP::Image::IsSupportedImageFile(str))
			{
				validMultiFileCubemap = false;
				break;
			}
		}
	}
	else
	{
		//Use already in memory images
		for(const auto* const img : textureLoadDesc.Images)
		{
			if(!img)
			{
				validMultiFileCubemap = false;
				break;
			}
		}
	}

	bool supported = true;
	if(textureLoadDesc.IsCubemap && textureLoadDesc.Type == RendererAPI::TextureCubeType::MultiFile &&
	   !validMultiFileCubemap)
	{
		supported = false;
	}

	if(!textureLoadDesc.Images[0] && supported)
	{
		if((textureLoadDesc.Filepaths[0].empty() ||
			!TRAP::FileSystem::Exists(textureLoadDesc.Filepaths[0]) ||
			!TRAP::Image::IsSupportedImageFile(textureLoadDesc.Filepaths[0])))
		{
			supported = false;
		}
	}

	TextureUpdateDescInternal updateDesc = {};
	updateDesc.MipsAfterSlice = false; //TODO Change when we support a texture format which holds mips after a slice like KTX
	TRAP::Graphics::RendererAPI::TextureDesc textureDesc = {};
	textureDesc.Depth = 1; //TODO Change when we support a texture format that can hold 3D textures like KTX
	textureDesc.ArraySize = 1; //TODO Change when we support a texture format than can contain multiple images in one file like KTX
	textureDesc.Descriptors = RendererAPI::DescriptorType::Texture;
	textureDesc.SampleCount = RendererAPI::SampleCount::One;
	textureDesc.StartState = TRAP::Graphics::RendererAPI::ResourceState::Common;
	textureDesc.Flags |= textureLoadDesc.CreationFlag;
	if(static_cast<bool>(textureDesc.Flags & RendererAPI::TextureCreationFlags::Storage))
		textureDesc.Descriptors |= RendererAPI::DescriptorType::RWTexture;

	if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan &&
	   textureLoadDesc.Desc != nullptr)
			textureDesc.VkSamplerYcbcrConversionInfo = textureLoadDesc.Desc->VkSamplerYcbcrConversionInfo;

	std::array<TRAP::Scope<TRAP::Image>, 6> ownedImages{}; //Storage for images that are getting loaded from here
	std::array<const TRAP::Image*, 6> ptrImages{}; //Pointers to the images to upload

	if((!textureLoadDesc.Filepaths[0].empty() || textureLoadDesc.Images[0]) && supported)
	{
		//Get a name for the texture
		textureDesc.Name = "Unknown";
		if(!textureLoadDesc.Images[0])
		{
			//Use file path
			const auto fileName = FileSystem::GetFileNameWithEnding(textureLoadDesc.Filepaths[0]);
			if(fileName)
				textureDesc.Name = *fileName;
		}
		else if(!textureLoadDesc.Images[0]->GetFilePath().empty())
		{
			//Use already in memory image
			const auto fileName = FileSystem::GetFileNameWithEnding(textureLoadDesc.Images[0]->GetFilePath());
			if(fileName)
				textureDesc.Name = *fileName;
		}

		//Handle the different types of textures
		if(textureLoadDesc.IsCubemap && textureLoadDesc.Type == RendererAPI::TextureCubeType::MultiFile)
		{
			if(!textureLoadDesc.Images[0])
			{
				//Use file paths
				for(std::size_t i = 0; i < ownedImages.size(); ++i)
				{
					ownedImages[i] = TRAP::Image::LoadFromFile(textureLoadDesc.Filepaths[i]);
					if(ownedImages[i])
						ptrImages[i] = ownedImages[i].get();
				}
			}
			else
			{
				//Use already in memory images
				ptrImages = textureLoadDesc.Images;
			}

			//Validation checks
			bool valid = true;
			if(ptrImages[0]->GetWidth() != ptrImages[0]->GetHeight())
			{
				TP_ERROR(Log::TexturePrefix, "Images width and height must be the same!");
				valid = false;
			}
			for(std::size_t i = 1; i < ptrImages.size(); ++i)
			{
				if(!valid)
					break;

				//Check if every image has the same resolution as image 0
				if (ptrImages[0]->GetWidth() != ptrImages[i]->GetWidth() ||
					ptrImages[0]->GetHeight() != ptrImages[i]->GetHeight())
				{
					TP_ERROR(Log::TexturePrefix, "Images have mismatching width and/or height!");
					valid = false;
					break;
				}
				if (ptrImages[0]->GetColorFormat() != ptrImages[i]->GetColorFormat() ||
					ptrImages[0]->GetBitsPerPixel() != ptrImages[i]->GetBitsPerPixel())
				{
					TP_ERROR(Log::TexturePrefix, "Images have mismatching color formats and/or bits per pixel!");
					valid = false;
					break;
				}
			}

			if(!valid) //Use FallbackCube
			{
				TP_WARN(Log::TexturePrefix, "Texture using FallbackCube texture");
				textureDesc.Name = "FallbackCube";

				for(std::size_t i = 0; i < ownedImages.size(); ++i)
				{
					ownedImages[i] = TRAP::Image::LoadFallback();
					ptrImages[i] = ownedImages[i].get();
				}
			}

			textureDesc.Width = ptrImages[0]->GetWidth();
			textureDesc.Height = ptrImages[0]->GetHeight();
			textureDesc.Descriptors |= RendererAPI::DescriptorType::TextureCube;
			textureDesc.ArraySize = 6;
			textureDesc.MipLevels = TRAP::Graphics::Texture::CalculateMipLevels(textureDesc.Width, textureDesc.Height);
		}
		/*else if(textureLoadDesc.IsCubemap && textureLoadDesc.Type == RendererAPI::TextureCubeType::Equirectangular)
		{
			//TODO
			//load image
			//what now?
			//set texturedesc
			//arraysize = 6 ?!
			//descriptors texture, cubemap
		}*/
		else if(textureLoadDesc.IsCubemap && textureLoadDesc.Type == RendererAPI::TextureCubeType::Cross)
		{
			TRAP::Scope<TRAP::Image> baseImg = nullptr;
			if(!textureLoadDesc.Images[0])
				baseImg = TRAP::Image::LoadFromFile(textureLoadDesc.Filepaths[0]);
			const TRAP::Image* const baseImgPtr = textureLoadDesc.Images[0] ? textureLoadDesc.Images[0] : baseImg.get();

			bool valid = true;
			if(baseImgPtr->GetWidth() > baseImgPtr->GetHeight()) //Horizontal
			{
				if(baseImgPtr->GetWidth() % 4 != 0 || baseImgPtr->GetHeight() % 3 != 0)
				{
					valid = false;
					TP_ERROR(Log::TexturePrefix,
					         "Width must be a multiple of 4 & height must be a multiple of 3!");
				}
			}
			else //Vertical
			{
				if(baseImgPtr->GetWidth() % 3 != 0 || baseImgPtr->GetHeight() % 4 != 0)
				{
					TP_ERROR(Log::TexturePrefix,
					         "Width must be a multiple of 3 & height must be a multiple of 4!");
					valid = false;
				}
			}

			if(valid)
			{
				if(baseImgPtr->IsHDR() && baseImgPtr->GetBytesPerChannel() == 4)
				    ownedImages = TRAP::Graphics::Texture::SplitImageFromCross<float>(baseImgPtr);
				else if (baseImgPtr->IsLDR() && baseImgPtr->GetBytesPerChannel() == 2)
					ownedImages = TRAP::Graphics::Texture::SplitImageFromCross<uint16_t>(baseImgPtr);
				else /*if (baseImgPtr->IsLDR() && baseImgPtr->GetBytesPerChannel() == 1)*/
					ownedImages = TRAP::Graphics::Texture::SplitImageFromCross<uint8_t>(baseImgPtr);
			}
			else //Use FallbackCube
			{
				TP_WARN(Log::TexturePrefix, "Texture using FallbackCube texture");
				textureDesc.Name = "FallbackCube";

				for(std::size_t i = 0; i < ownedImages.size(); ++i)
					ownedImages[i] = TRAP::Image::LoadFallback();
			}

			for(std::size_t i = 0; i < ownedImages.size(); ++i)
				ptrImages[i] = ownedImages[i].get();

			textureDesc.Width = ptrImages[0]->GetWidth();
			textureDesc.Height = ptrImages[0]->GetHeight();
			textureDesc.Descriptors |= RendererAPI::DescriptorType::TextureCube;
			textureDesc.ArraySize = 6;
			textureDesc.MipLevels = TRAP::Graphics::Texture::CalculateMipLevels(textureDesc.Width,
			                                                                    textureDesc.Height);
		}
		else //if(!textureLoadDesc.IsCubemap) //Normal Texture
		{
			if(!textureLoadDesc.Images[0])
				ownedImages[0] = TRAP::Image::LoadFromFile(textureLoadDesc.Filepaths[0]);
			ptrImages[0] = textureLoadDesc.Images[0] ? textureLoadDesc.Images[0] : ownedImages[0].get();

			textureDesc.Width = ptrImages[0]->GetWidth();
			textureDesc.Height = ptrImages[0]->GetHeight();
			textureDesc.MipLevels = TRAP::Graphics::Texture::CalculateMipLevels(textureDesc.Width,
			                                                                    textureDesc.Height);
		}

		textureDesc.Format = TRAP::Graphics::API::ImageFormat::R8G8B8A8_UNORM;

		if (ptrImages[0]->IsHDR() && ptrImages[0]->GetColorFormat() == TRAP::Image::ColorFormat::RGB) //RGB HDR (32 bpc) | Will be converted to RGBA before upload
			textureDesc.Format = TRAP::Graphics::API::ImageFormat::R32G32B32A32_SFLOAT;
		else if (ptrImages[0]->GetBitsPerChannel() == 16 && (ptrImages[0]->GetColorFormat() == TRAP::Image::ColorFormat::RGBA || //RGB(A) 16 bpc | Will be converted to RGBA before upload
				 ptrImages[0]->GetColorFormat() == TRAP::Image::ColorFormat::RGB))
			 textureDesc.Format = TRAP::Graphics::API::ImageFormat::R16G16B16A16_UNORM;
		else if (ptrImages[0]->GetBitsPerChannel() == 8 && ptrImages[0]->GetColorFormat() == TRAP::Image::ColorFormat::RGBA) //RGBA 8 bpc
			textureDesc.Format = TRAP::Graphics::API::ImageFormat::R8G8B8A8_UNORM;
		else if (ptrImages[0]->GetBitsPerChannel() == 32 && ptrImages[0]->GetColorFormat() == TRAP::Image::ColorFormat::GrayScaleAlpha) //GrayScale Alpha HDR (32 bpc)
			textureDesc.Format = TRAP::Graphics::API::ImageFormat::R32G32_SFLOAT;
		else if (ptrImages[0]->GetBitsPerChannel() == 16 && ptrImages[0]->GetColorFormat() == TRAP::Image::ColorFormat::GrayScaleAlpha) //GrayScale Alpha 16 bpc
			textureDesc.Format = TRAP::Graphics::API::ImageFormat::R16G16_UNORM;
		else if (ptrImages[0]->GetBitsPerChannel() == 8 && ptrImages[0]->GetColorFormat() == TRAP::Image::ColorFormat::GrayScaleAlpha) //GrayScale Alpha 8 bpc
			 textureDesc.Format = TRAP::Graphics::API::ImageFormat::R8G8_UNORM;
		else if(ptrImages[0]->IsHDR() && ptrImages[0]->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale) //GrayScale HDR (32 bpc)
			textureDesc.Format = TRAP::Graphics::API::ImageFormat::R32_SFLOAT;
		else if (ptrImages[0]->GetBitsPerChannel() == 16 && ptrImages[0]->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale) //GrayScale 16 bpc
			textureDesc.Format = TRAP::Graphics::API::ImageFormat::R16_UNORM;
		else if (ptrImages[0]->GetBitsPerChannel() == 8 && ptrImages[0]->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale) //GrayScale 8 bpc
			textureDesc.Format = TRAP::Graphics::API::ImageFormat::R8_UNORM;

		textureLoadDesc.Texture->Init(textureDesc);

		updateDesc.Texture = textureLoadDesc.Texture;
		updateDesc.BaseMipLevel = 0;
		//updateDesc.MipLevels = textureDesc.MipLevels; //TODO For now every supported format does not support mip levels in file
		updateDesc.MipLevels = 1;
		updateDesc.BaseArrayLayer = 0;
		updateDesc.LayerCount = textureDesc.ArraySize;

		return UpdateTexture(activeSet, updateDesc, &ptrImages);
	}

	//Fallback to default textures
	if(!textureLoadDesc.IsCubemap)
	{
		textureDesc.Name = "Fallback2D";

		ownedImages[0] = TRAP::Image::LoadFallback();
		ptrImages[0] = ownedImages[0].get();

		textureDesc.Width = ptrImages[0]->GetWidth();
		textureDesc.Height = ptrImages[0]->GetHeight();
		textureDesc.Format = TRAP::Graphics::API::ImageFormat::R8G8B8A8_UNORM;
		textureDesc.MipLevels = TRAP::Graphics::Texture::CalculateMipLevels(textureDesc.Width,
			                                                                textureDesc.Height);
	}
	else
	{
		textureDesc.Name = "FallbackCube";

		for(std::size_t i = 0; i < ownedImages.size(); ++i)
		{
			ownedImages[i] = TRAP::Image::LoadFallback();
			ptrImages[i] = ownedImages[i].get();
		}

		textureDesc.Width = ptrImages[0]->GetWidth();
		textureDesc.Height = ptrImages[0]->GetHeight();
		textureDesc.Descriptors |= RendererAPI::DescriptorType::TextureCube;
		textureDesc.ArraySize = 6;
		textureDesc.Format = TRAP::Graphics::API::ImageFormat::R8G8B8A8_UNORM;
		textureDesc.MipLevels = TRAP::Graphics::Texture::CalculateMipLevels(textureDesc.Width,
			                                                                textureDesc.Height);
	}

	textureLoadDesc.Texture->Init(textureDesc);

	updateDesc.Texture = textureLoadDesc.Texture;
	updateDesc.BaseMipLevel = 0;
	//updateDesc.MipLevels = textureDesc.MipLevels; //TODO For now every supported format does not support mip levels in file
	updateDesc.MipLevels = 1;
	updateDesc.BaseArrayLayer = 0;
	updateDesc.LayerCount = textureDesc.ArraySize;

	return UpdateTexture(activeSet, updateDesc, &ptrImages);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::ResourceLoader::UploadFunctionResult TRAP::Graphics::API::ResourceLoader::CopyTexture(std::size_t activeSet,
            																					                         RendererAPI::TextureCopyDesc& textureCopy)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP::Ref<Texture> texture = textureCopy.Texture;
	const ImageFormat format = texture->GetImageFormat();

	const CommandBuffer* const cmd = AcquireCmd(activeSet);

	if(textureCopy.WaitSemaphore)
		m_copyEngine.WaitSemaphores.push_back(textureCopy.WaitSemaphore);

	if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
	{
		RendererAPI::TextureBarrier barrier{};
		barrier.Texture = texture.get();
		barrier.CurrentState = textureCopy.TextureState;
		barrier.NewState = RendererAPI::ResourceState::CopySource;
		cmd->ResourceBarrier(nullptr, &barrier, nullptr);
	}

	uint64_t numBytes = 0;
	uint64_t rowBytes = 0;
	uint64_t numRows = 0;

	const bool ret = UtilGetSurfaceInfo(texture->GetWidth(), texture->GetHeight(), format, &numBytes, &rowBytes, &numRows);
	if(!ret)
		return UploadFunctionResult::InvalidRequest;

	RendererAPI::SubresourceDataDesc subresourceDesc{};
	subresourceDesc.ArrayLayer = textureCopy.ArrayLayer;
	subresourceDesc.MipLevel = textureCopy.MipLevel;
	subresourceDesc.SrcOffset = textureCopy.BufferOffset;

	if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
	{
		const uint64_t sliceAlignment = UtilGetTextureSubresourceAlignment(format);
		const uint64_t rowAlignment = UtilGetTextureRowAlignment();
		const uint64_t subRowPitch = ((rowBytes + rowAlignment - 1) / rowAlignment) * rowAlignment;
		const uint64_t subSlicePitch = (((subRowPitch * numRows) + sliceAlignment - 1) / sliceAlignment) * sliceAlignment;
		subresourceDesc.RowPitch = static_cast<uint32_t>(subRowPitch);
		subresourceDesc.SlicePitch = static_cast<uint32_t>(subSlicePitch);
	}

	cmd->CopySubresource(textureCopy.Buffer.get(), textureCopy.Texture.get(), subresourceDesc);

	return UploadFunctionResult::Completed;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(const RendererAPI::BufferUpdateDesc& buffer) noexcept
	: Type(UpdateRequestType::UpdateBuffer), Desc(buffer)
{}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(const RendererAPI::TextureLoadDesc& texture) noexcept
	: Type(UpdateRequestType::LoadTexture), Desc(texture)
{}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(const RendererAPI::TextureCopyDesc& textureCopy) noexcept
	: Type(UpdateRequestType::CopyTexture), Desc(textureCopy)
{}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(TRAP::Graphics::API::ResourceLoader::TextureUpdateDescInternal texture) noexcept
	: Type(UpdateRequestType::UpdateTexture), Desc(texture)
{}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(const RendererAPI::BufferBarrier& barrier) noexcept
	: Type(UpdateRequestType::BufferBarrier), Desc(barrier)
{}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(const RendererAPI::TextureBarrier& barrier) noexcept
	: Type(UpdateRequestType::TextureBarrier), Desc(barrier)
{}