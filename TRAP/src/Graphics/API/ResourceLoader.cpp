#include "TRAPPCH.h"
#include "ResourceLoader.h"

#include "Objects/Fence.h"
#include "Objects/Buffer.h"
#include "Objects/CommandBuffer.h"
#include "Objects/CommandPool.h"
#include "Vulkan/VulkanRenderer.h"
#include "Utils/String/String.h"
#include "VFS/VFS.h"

#include "Vulkan/VulkanCommon.h"
#include "Vulkan/Objects/VulkanDevice.h"
#include "Vulkan/Objects/VulkanPhysicalDevice.h"
#include "Vulkan/Objects/VulkanCommandBuffer.h"

TRAP::Graphics::RendererAPI::ResourceLoaderDesc TRAP::Graphics::API::ResourceLoader::DefaultResourceLoaderDesc{8ull << 20, 2};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::StreamerThreadFunc(ResourceLoader* loader)
{
	TRAP_ASSERT(loader);

	SyncToken maxToken{};

	while(loader->m_run)
	{
		{
			std::unique_lock<std::mutex> lock(loader->m_queueMutex);

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
		loader->WaitCopyEngineSet(loader->m_nextSet, true);
		loader->ResetCopyEngineSet(loader->m_nextSet);

		//Signal pending tokens from previous frames
		{
			std::lock_guard<std::mutex> lock(loader->m_tokenMutex);
			loader->m_tokenCompleted = loader->m_currentTokenState[loader->m_nextSet];
		}
		loader->m_tokenCond.notify_all();

		uint64_t completionMask = 0;

		std::vector<UpdateRequest> activeQueue;
		{
			std::lock_guard<std::mutex> lock(loader->m_queueMutex);

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
				loader->AcquireCmd(loader->m_nextSet)->ResourceBarrier(
					{ std::get<RendererAPI::BufferBarrier>(updateState.Desc) },
					{}, {});
				result = UploadFunctionResult::Completed;
				break;
			}

			case UpdateRequestType::TextureBarrier:
			{
				loader->AcquireCmd(loader->m_nextSet)->ResourceBarrier
					(
						{}, {std::get<RendererAPI::TextureBarrier>(updateState.Desc)}, {}
					);
				result = UploadFunctionResult::Completed;
				break;
			}

			case UpdateRequestType::LoadTexture:
			{
				result = loader->LoadTexture(loader->m_nextSet, updateState);
				break;
			}

			case UpdateRequestType::Invalid:
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
				TRAP_ASSERT(maxToken < updateState.WaitIndex);
				maxToken = updateState.WaitIndex;
			}

			TRAP_ASSERT(result != UploadFunctionResult::StagingBufferFull);
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

uint32_t TRAP::Graphics::API::ResourceLoader::UtilGetTextureSubresourceAlignment(const RendererAPI::ImageFormat fmt)
{
	const uint32_t blockSize = Math::Max(1u, RendererAPI::ImageFormatBitSizeOfBlock(fmt) >> 3);
	const uint32_t alignment = ((RendererAPI::GPUSettings.UploadBufferTextureAlignment + blockSize - 1) /
	                            blockSize) * blockSize;

	const uint32_t rowAlignment = Math::Max(1u, RendererAPI::GPUSettings.UploadBufferTextureRowAlignment);
	return ((alignment + rowAlignment - 1) / rowAlignment) * rowAlignment;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::ResourceLoader::UtilGetSurfaceSize(const RendererAPI::ImageFormat fmt,
                                                                 const uint32_t width, const uint32_t height,
																 const uint32_t depth, const uint32_t rowStride,
																 const uint32_t sliceStride,
																 const uint32_t baseMipLevel,
																 const uint32_t mipLevels,
																 const uint32_t baseArrayLayer,
																 const uint32_t arrayLayers)
{
	uint32_t requiredSize = 0;

	for(uint32_t s = baseArrayLayer; s < baseArrayLayer + arrayLayers; ++s)
	{
		uint32_t w = width;
		uint32_t h = height;
		uint32_t d = depth;

		for(uint32_t m = baseMipLevel; m < baseMipLevel + mipLevels; ++m)
		{
			uint32_t rowBytes = 0;
			uint32_t numRows = 0;

			if(!UtilGetSurfaceInfo(w, h, fmt, nullptr, &rowBytes, &numRows))
				return false;

			const uint32_t temp = ((rowBytes + rowStride - 1) / rowStride) * rowStride;
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

bool TRAP::Graphics::API::ResourceLoader::UtilGetSurfaceInfo(const uint32_t width, const uint32_t height,
	                                                         const RendererAPI::ImageFormat fmt,
															 uint32_t* outNumBytes, uint32_t* outRowBytes,
															 uint32_t* outNumRows)
{
	uint64_t numBytes = 0;
	uint64_t rowBytes;
	uint64_t numRows;

	const uint32_t bpp = RendererAPI::ImageFormatBitSizeOfBlock(fmt);
	const bool compressed = RendererAPI::ImageFormatIsCompressed(fmt);
	const bool planar = RendererAPI::ImageFormatIsPlanar(fmt);

	const bool packed = false;

	if(compressed)
	{
		const uint32_t blockWidth = RendererAPI::ImageFormatWidthOfBlock(fmt);
		const uint32_t blockHeight = RendererAPI::ImageFormatHeightOfBlock(fmt);
		uint32_t numBlocksWide = 0;
		uint32_t numBlocksHigh = 0;
		if(width > 0)
			numBlocksWide = Math::Max(1u, (width + (blockWidth - 1)) / blockWidth);
		if(height > 0)
			numBlocksHigh = Math::Max(1u, (height + (blockHeight - 1)) / blockHeight);

		rowBytes = numBlocksWide * (bpp >> 3);
		numRows = numBlocksHigh;
		numBytes = rowBytes * numBlocksHigh;
	}
	else if(packed)
	{
		TP_ERROR(Log::RendererVulkanTexturePrefix, "Packed not implemented!");
		return false;
	}
	else if(planar)
	{
		const uint32_t numOfPlanes = RendererAPI::ImageFormatNumOfPlanes(fmt);

		for(uint32_t i = 0; i < numOfPlanes; ++i)
			numBytes += RendererAPI::ImageFormatPlaneWidth(fmt, i, width) *
			            RendererAPI::ImageFormatPlaneHeight(fmt, i, height) *
						RendererAPI::ImageFormatPlaneSizeOfBlock(fmt, i);

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
		*outNumBytes = static_cast<uint32_t>(numBytes);
	if(outRowBytes)
		*outRowBytes = static_cast<uint32_t>(rowBytes);
	if(outNumRows)
		*outNumRows = static_cast<uint32_t>(numRows);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::ResourceLoader(const RendererAPI::ResourceLoaderDesc* desc)
	: m_desc(desc ? *desc : DefaultResourceLoaderDesc),
	  m_run(true),
	  m_tokenCompleted(0),
	  m_tokenCounter(0),
	  m_currentTokenState(),
      m_nextSet(),
      m_submittedSets()
{
	SetupCopyEngine();

	//Create dedicated resource loader thread.
	m_thread = std::thread(StreamerThreadFunc, this);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::~ResourceLoader()
{
	m_run = false;

	m_queueCond.notify_one();
	m_thread.join();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::AddResource(RendererAPI::BufferLoadDesc& desc, SyncToken* token)
{
	const uint64_t stagingBufferSize = m_copyEngine.BufferSize;
	const bool update = desc.Data || desc.ForceReset;

	TRAP_ASSERT(stagingBufferSize > 0);
	if(desc.Desc.MemoryUsage == RendererAPI::ResourceMemoryUsage::GPUOnly &&
	   !static_cast<uint32_t>(desc.Desc.StartState) && !update)
	{
		desc.Desc.StartState = UtilDetermineResourceStartState(desc.Desc);
		TP_WARN(Log::RendererVulkanBufferPrefix, "Buffer start state not provided. Determined the start state as (",
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
			const void* data = desc.Data;

			RendererAPI::BufferUpdateDesc updateDesc{};
			updateDesc.Buffer = desc.Buffer;
			for(uint64_t offset = 0; offset < desc.Desc.Size; offset += stagingBufferSize)
			{
				const std::size_t chunkSize = Math::Min<std::size_t>(stagingBufferSize, desc.Desc.Size - offset);
				updateDesc.Size = chunkSize;
				updateDesc.DstOffset = offset;
				BeginUpdateResource(updateDesc);
				if (desc.ForceReset)
					std::memset(updateDesc.MappedData, 0, chunkSize);
				else
					std::memcpy(updateDesc.MappedData, static_cast<const uint8_t*>(data) + offset, chunkSize);
				EndUpdateResource(updateDesc, token);
			}
		}
		else
		{
			RendererAPI::BufferUpdateDesc updateDesc{};
			updateDesc.Buffer = desc.Buffer;
			BeginUpdateResource(updateDesc);
			if (desc.ForceReset)
				std::memset(updateDesc.MappedData, 0, static_cast<std::size_t>(desc.Desc.Size));
			else
				std::memcpy(updateDesc.MappedData, desc.Data, static_cast<std::size_t>(desc.Desc.Size));
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

void TRAP::Graphics::API::ResourceLoader::AddResource(RendererAPI::TextureLoadDesc& textureDesc, SyncToken* token)
{
	TRAP_ASSERT(textureDesc.Texture);

	if(textureDesc.Filepaths[0].empty() && textureDesc.Desc)
	{
		TRAP_ASSERT(static_cast<uint32_t>(textureDesc.Desc->StartState));

		//If texture is supposed to be filled later (UAV / Update later / ...) proceed with the StartState provided
		//by the user in the texture description
		//TODO Replace with Texture abstraction
		TRAP::Graphics::API::VulkanRenderer* vkRenderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>
			(
				RendererAPI::GetRenderer().get()
			);
		*textureDesc.Texture = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(vkRenderer->GetDevice(),
		                                                                         *textureDesc.Desc,
																				 vkRenderer->GetVMA());

		//Transition texture to desired state for Vulkan since all Vulkan resources are created in undefined state
		if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
		{
			RendererAPI::ResourceState startState = textureDesc.Desc->StartState;
			//Check whether this is required (user specified a state other than undefined / common)
			if(startState == RendererAPI::ResourceState::Undefined ||
			   startState == RendererAPI::ResourceState::Common)
				startState = UtilDetermineResourceStartState(static_cast<uint32_t>(textureDesc.Desc->Descriptors) &
				                                             static_cast<uint32_t>(RendererAPI::DescriptorType::RWTexture));
			QueueTextureBarrier(*textureDesc.Texture, startState, token);
		}
	}
	else
	{
		const RendererAPI::TextureLoadDesc loadDesc = textureDesc;
		QueueTextureLoad(loadDesc, token);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::BeginUpdateResource(RendererAPI::BufferUpdateDesc& desc)
{
	const TRAP::Ref<Buffer>& buffer = desc.Buffer;
	TRAP_ASSERT(buffer);

	const uint64_t size = desc.Size > 0 ? desc.Size : (desc.Buffer->GetSize() - desc.DstOffset);
	TRAP_ASSERT(desc.DstOffset + size <= buffer->GetSize());

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
	//TODO Replace with abstraction
	const TRAP::Ref<TRAP::Graphics::API::VulkanTexture> texture = desc.Texture;
	const RendererAPI::ImageFormat fmt = texture->GetImageFormat();
	const uint32_t alignment = UtilGetTextureSubresourceAlignment(fmt);

	const bool success = UtilGetSurfaceInfo(texture->GetWidth(), texture->GetHeight(),
	                                        fmt, &desc.SrcSliceStride, &desc.SrcRowStride, &desc.RowCount);
	TRAP_ASSERT(success);

	const uint32_t rowAlignment = Math::Max(1u, RendererAPI::GPUSettings.UploadBufferTextureRowAlignment);
	desc.DstRowStride = ((desc.SrcRowStride + rowAlignment - 1) / rowAlignment) * rowAlignment;
	desc.DstSliceStride = (((desc.DstRowStride * desc.RowCount) + alignment - 1) / alignment) * alignment;

	const int64_t requiredSize = ((desc.DstRowStride * desc.RowCount + alignment - 1) / alignment) * alignment;

	//We need to use a staging buffer
	desc.Internal.MappedRange = AllocateUploadMemory(requiredSize, alignment);
	desc.Internal.MappedRange.Flags = static_cast<uint32_t>(MappedRangeFlag::TempBuffer);
	desc.MappedData = desc.Internal.MappedRange.Data;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::EndUpdateResource(RendererAPI::BufferUpdateDesc& desc, SyncToken* token)
{
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

void TRAP::Graphics::API::ResourceLoader::EndUpdateResource(RendererAPI::TextureUpdateDesc& desc, SyncToken* token)
{
	TextureUpdateDescInternal internalDesc = {};
	internalDesc.Texture = desc.Texture;
	internalDesc.Range = desc.Internal.MappedRange;
	internalDesc.BaseMipLevel = 0;
	internalDesc.MipLevels = 1;
	internalDesc.BaseArrayLayer = desc.ArrayLayer;
	internalDesc.LayerCount = 1;
	QueueTextureUpdate(internalDesc, token);

	//Restore the state to before the BeginUpdateResource call.
	desc.MappedData = nullptr;
	desc.Internal = {};
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::ResourceLoader::AllResourceLoadsCompleted() const
{
	const SyncToken token = m_tokenCounter;
	return token <= m_tokenCompleted;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::WaitForAllResourceLoads()
{
	SyncToken token = m_tokenCounter;
	WaitForToken(&token);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::SyncToken TRAP::Graphics::API::ResourceLoader::GetLastTokenCompleted() const
{
	return m_tokenCompleted;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::ResourceLoader::IsTokenCompleted(const SyncToken* token) const
{
	return *token <= m_tokenCompleted;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::WaitForToken(const SyncToken* token)
{
	std::unique_lock<std::mutex> lock(m_tokenMutex);
	while (!IsTokenCompleted(token))
		m_tokenCond.wait(lock);
}

//-------------------------------------------------------------------------------------------------------------------//

//Return a new staging buffer
TRAP::Graphics::RendererAPI::MappedMemoryRange TRAP::Graphics::API::ResourceLoader::AllocateUploadMemory(const uint64_t memoryRequirement,
                                                                                                         const uint32_t alignment)
{
	RendererAPI::BufferDesc desc{};
	desc.Size = memoryRequirement;
	desc.Alignment = alignment;
	desc.MemoryUsage = RendererAPI::ResourceMemoryUsage::CPUOnly;
	desc.Flags = RendererAPI::BufferCreationFlags::PersistentMap;
	TRAP::Ref<Buffer> buffer = Buffer::Create(desc);

	return RendererAPI::MappedMemoryRange{static_cast<uint8_t*>(buffer->GetCPUMappedAddress()), std::move(buffer),
	                                      0, memoryRequirement };
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::QueueBufferBarrier(const TRAP::Ref<Buffer>& buffer,
                                                             const RendererAPI::ResourceState state,
															 SyncToken* token)
{
	SyncToken t = 0;
	{
		std::lock_guard<std::mutex> lock(m_queueMutex);

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
                                                            SyncToken* token)
{
	SyncToken t = 0;
	{
		std::lock_guard<std::mutex> lock(m_queueMutex);

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
											               SyncToken* token)
{
	SyncToken t = 0;
	{
		std::lock_guard<std::mutex> lock(m_queueMutex);

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
															 SyncToken* token)
{
	TRAP_ASSERT(textureUpdate.Range.Buffer);

	SyncToken t = 0;
	{
		std::lock_guard<std::mutex> lock(m_queueMutex);

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

void TRAP::Graphics::API::ResourceLoader::QueueTextureBarrier(const TRAP::Ref<TRAP::Graphics::API::VulkanTexture>& texture,
                                                              RendererAPI::ResourceState state, SyncToken* token)
{
	SyncToken t = 0;
	{
		std::lock_guard<std::mutex> lock(m_queueMutex);

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
TRAP::Graphics::RendererAPI::MappedMemoryRange TRAP::Graphics::API::ResourceLoader::AllocateStagingMemory(const uint64_t memoryRequirement,
                                                                                                          const uint32_t alignment)
{
	uint64_t offset = m_copyEngine.ResourceSets[m_nextSet].AllocatedSpace;
	if(alignment != 0)
		offset = ((offset + alignment - 1) / alignment) * alignment;

	CopyEngine::CopyResourceSet& resSet = m_copyEngine.ResourceSets[m_nextSet];
	const uint64_t size = static_cast<uint64_t>(resSet.Buffer->GetSize());
	const bool memoryAvailable = (offset < size) && (memoryRequirement <= size - offset);
	if(memoryAvailable && resSet.Buffer->GetCPUMappedAddress())
	{
		TRAP::Ref<Buffer> buffer = resSet.Buffer;
		TRAP_ASSERT(buffer->GetCPUMappedAddress());
		uint8_t* dstData = static_cast<uint8_t*>(buffer->GetCPUMappedAddress()) + offset;
		m_copyEngine.ResourceSets[m_nextSet].AllocatedSpace = offset + memoryRequirement;
		return { dstData, std::move(buffer), offset, memoryRequirement };
	}

	if(m_copyEngine.BufferSize < memoryRequirement)
	{
		RendererAPI::MappedMemoryRange range = AllocateUploadMemory(memoryRequirement, alignment);
		resSet.TempBuffers.emplace_back(range.Buffer);
		return range;
	}

	RendererAPI::MappedMemoryRange range = AllocateUploadMemory(memoryRequirement, alignment);
	resSet.TempBuffers.emplace_back(range.Buffer);
	return range;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::FreeAllUploadMemory()
{
	for(UpdateRequest& request : m_requestQueue)
	{
		if (request.UploadBuffer)
			request.UploadBuffer.reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::SetupCopyEngine()
{
	//TODO Change after Mipmapping uses compute shaders (and equirectangular images)
	/*const RendererAPI::QueueDesc desc{ RendererAPI::QueueType::Transfer, RendererAPI::QueueFlag::None,
	                                     RendererAPI::QueuePriority::Normal };*/
	const RendererAPI::QueueDesc desc{ RendererAPI::QueueType::Graphics, RendererAPI::QueueFlag::None,
	                                   RendererAPI::QueuePriority::Normal };
	m_copyEngine.Queue = Queue::Create(desc);

	constexpr uint64_t maxBlockSize = 32;
	uint64_t size = m_desc.BufferSize;
	size = Math::Max(size, maxBlockSize);

	m_copyEngine.ResourceSets.reserve(m_desc.BufferCount);
	for(uint32_t i = 0; i < m_desc.BufferCount; ++i)
	{
		CopyEngine::CopyResourceSet cpyResSet{};
		cpyResSet.Fence = Fence::Create();
		RendererAPI::CommandPoolDesc cmdPoolDesc{};
		cmdPoolDesc.Queue = m_copyEngine.Queue;
		cpyResSet.CommandPool = CommandPool::Create(cmdPoolDesc);

		cpyResSet.Cmd = cpyResSet.CommandPool->AllocateCommandBuffer(false);

		cpyResSet.Buffer = AllocateUploadMemory(size, UtilGetTextureSubresourceAlignment()).Buffer;

		m_copyEngine.ResourceSets.push_back(cpyResSet);
	}

	m_copyEngine.BufferSize = size;
	m_copyEngine.BufferCount = m_desc.BufferCount;
	m_copyEngine.IsRecording = false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::CleanupCopyEngine()
{
	for(uint32_t i = 0; i < m_copyEngine.BufferCount; ++i)
	{
		CopyEngine::CopyResourceSet& resourceSet = m_copyEngine.ResourceSets[i];
		resourceSet.Buffer.reset();

		resourceSet.CommandPool->FreeCommandBuffer(resourceSet.Cmd);
		resourceSet.Cmd = nullptr;
		resourceSet.CommandPool.reset();

		resourceSet.Fence.reset();

		if (!resourceSet.TempBuffers.empty())
			TP_INFO(Log::RendererVulkanBufferPrefix, "Was not cleaned up ", i);

		resourceSet.TempBuffers.clear();
	}

	m_copyEngine.ResourceSets.clear();

	m_copyEngine.Queue.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::ResourceLoader::WaitCopyEngineSet(const std::size_t activeSet, const bool wait)
{
	TRAP_ASSERT(!m_copyEngine.IsRecording);
	CopyEngine::CopyResourceSet& resSet = m_copyEngine.ResourceSets[activeSet];
	bool completed = true;

	const RendererAPI::FenceStatus status = resSet.Fence->GetStatus();
	completed = status != RendererAPI::FenceStatus::Incomplete;
	if (wait && !completed)
		resSet.Fence->Wait();

	return completed;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::ResetCopyEngineSet(const std::size_t activeSet)
{
	TRAP_ASSERT(!m_copyEngine.IsRecording);
	m_copyEngine.ResourceSets[activeSet].AllocatedSpace = 0;
	m_copyEngine.IsRecording = false;

	m_copyEngine.ResourceSets[activeSet].TempBuffers.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::CommandBuffer* TRAP::Graphics::API::ResourceLoader::AcquireCmd(const std::size_t activeSet)
{
	CopyEngine::CopyResourceSet& resSet = m_copyEngine.ResourceSets[activeSet];
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
	if(m_copyEngine.IsRecording)
	{
		CopyEngine::CopyResourceSet& resSet = m_copyEngine.ResourceSets[activeSet];
		resSet.Cmd->End();
		RendererAPI::QueueSubmitDesc submitDesc{};
		submitDesc.Cmds.push_back(resSet.Cmd);

		submitDesc.SignalFence = resSet.Fence;

		{
			m_copyEngine.Queue->Submit(submitDesc);
		}
		m_copyEngine.IsRecording = false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::ResourceLoader::AreTasksAvailable() const
{
	if (!m_requestQueue.empty())
		return true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::ResourceState TRAP::Graphics::API::ResourceLoader::UtilDetermineResourceStartState(bool uav)
{
	if(uav)
		return RendererAPI::ResourceState::UnorderedAccess;

	return RendererAPI::ResourceState::ShaderResource;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::ResourceState TRAP::Graphics::API::ResourceLoader::UtilDetermineResourceStartState(const RendererAPI::BufferDesc& desc)
{
	//Host visible (Upload Heap)
	if (desc.MemoryUsage == RendererAPI::ResourceMemoryUsage::CPUOnly ||
	    desc.MemoryUsage == RendererAPI::ResourceMemoryUsage::CPUToGPU)
		return RendererAPI::ResourceState::GenericRead;

	//Device Local (Default Heap)
	if (desc.MemoryUsage == RendererAPI::ResourceMemoryUsage::GPUOnly)
	{
		const RendererAPI::DescriptorType usage = desc.Descriptors;

		//Try to limit number of states used overall to avoid sync complexities
		if (static_cast<uint32_t>(usage & RendererAPI::DescriptorType::RWBuffer))
			return RendererAPI::ResourceState::UnorderedAccess;
		if ((static_cast<uint32_t>(usage & RendererAPI::DescriptorType::VertexBuffer) ||
			static_cast<uint32_t>(usage & RendererAPI::DescriptorType::UniformBuffer)))
			return RendererAPI::ResourceState::VertexAndConstantBuffer;
		if (static_cast<uint32_t>(usage & RendererAPI::DescriptorType::IndexBuffer))
			return RendererAPI::ResourceState::IndexBuffer;
		if (static_cast<uint32_t>(usage & RendererAPI::DescriptorType::Buffer))
			return RendererAPI::ResourceState::ShaderResource;

		return RendererAPI::ResourceState::Common;
	}

	//Host Cached (Readback Heap)
	return RendererAPI::ResourceState::CopyDestination;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::UploadFunctionResult TRAP::Graphics::API::ResourceLoader::UpdateBuffer(const std::size_t activeSet,
																											const RendererAPI::BufferUpdateDesc& bufferUpdateDesc)
{
	const TRAP::Ref<Buffer>& buffer = bufferUpdateDesc.Buffer;
	TRAP_ASSERT(buffer->GetMemoryUsage() == RendererAPI::ResourceMemoryUsage::GPUOnly ||
		        buffer->GetMemoryUsage() == RendererAPI::ResourceMemoryUsage::GPUToCPU);

	CommandBuffer* cmd = AcquireCmd(activeSet);

	const RendererAPI::MappedMemoryRange range = bufferUpdateDesc.Internal.MappedRange;
	cmd->UpdateBuffer(buffer, bufferUpdateDesc.DstOffset, range.Buffer, range.Offset, range.Size);

	return UploadFunctionResult::Completed;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::UploadFunctionResult TRAP::Graphics::API::ResourceLoader::UpdateTexture(const std::size_t activeSet,
																											 const TextureUpdateDescInternal& textureUpdateDesc,
																											 std::array<TRAP::Scope<TRAP::Image>, 6>* const images)
{
	//When this call comes from UpdateResource, staging buffer data is already filled
	//All that is left to do is record and execute the Copy commands
	bool dataAlreadyFilled = textureUpdateDesc.Range.Buffer ? true : false;
	const TRAP::Ref<TRAP::Graphics::API::VulkanTexture>& texture = textureUpdateDesc.Texture; //TODO Replace with abstraction
	const RendererAPI::ImageFormat format = texture->GetImageFormat();
	CommandBuffer* cmd = AcquireCmd(activeSet);

	const uint32_t sliceAlignment = UtilGetTextureSubresourceAlignment(format);
	const uint32_t rowAlignment = Math::Max(1u, RendererAPI::GPUSettings.UploadBufferTextureRowAlignment);
	const uint64_t requiredSize = UtilGetSurfaceSize(format, texture->GetWidth(), texture->GetHeight(),
											         texture->GetDepth(), rowAlignment, sliceAlignment,
													 textureUpdateDesc.BaseMipLevel, textureUpdateDesc.MipLevels,
													 textureUpdateDesc.BaseArrayLayer, textureUpdateDesc.LayerCount);

	if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
	{
		RendererAPI::TextureBarrier barrier = {texture, RendererAPI::ResourceState::Undefined,
		                                       RendererAPI::ResourceState::CopyDestination};
		cmd->ResourceBarrier({}, {barrier}, {});
	}

	RendererAPI::MappedMemoryRange upload = dataAlreadyFilled ? textureUpdateDesc.Range :
	                                        AllocateStagingMemory(requiredSize, sliceAlignment);
	uint64_t offset = 0;

	if(!upload.Data)
		return UploadFunctionResult::StagingBufferFull;

	/* uint32_t firstStart = textureUpdateDesc.MipsAfterSlice ? textureUpdateDesc.BaseMipLevel :
	                         textureUpdateDesc.BaseArrayLayer;*/
	/* uint32_t firstEnd = textureUpdateDesc.MipsAfterSlice ?
	                       (textureUpdateDesc.BaseMipLevel + textureUpdateDesc.MipLevels) :
						   (textureUpdateDesc.BaseArrayLayer + textureUpdateDesc.LayerCount);*/
	/* uint32_t secondStart = textureUpdateDesc.MipsAfterSlice ? textureUpdateDesc.BaseArrayLayer :
	                          textureUpdateDesc.BaseMipLevel;*/
	/* uint32_t secondEnd = textureUpdateDesc.MipsAfterSlice ?
	                        (textureUpdateDesc.BaseArrayLayer + textureUpdateDesc.LayerCount) :
							(textureUpdateDesc.BaseMipLevel + textureUpdateDesc.MipLevels);*/

	// for(uint32_t j = firstStart; j < firstEnd; ++j)
	// {
	// 	for(uint32_t i = secondStart; i < secondEnd; ++i)
	// 	{
			//uint32_t mip = textureUpdateDesc.MipsAfterSlice ? j : i;
			//uint32_t layer = textureUpdateDesc.MipsAfterSlice ? i : j;
	for(uint32_t i = 0; i < textureUpdateDesc.LayerCount; ++i)
	{
		uint32_t layer = i;

		// uint32_t w = Math::Max(1u, (texture->GetWidth() >> mip));
		// uint32_t h = Math::Max(1u, (texture->GetHeight() >> mip));
		// uint32_t d = Math::Max(1u, (texture->GetDepth() >> mip));
		uint32_t w = Math::Max(1u, texture->GetWidth());
		uint32_t h = Math::Max(1u, texture->GetHeight());
		uint32_t d = Math::Max(1u, texture->GetDepth());

		uint32_t numBytes = 0;
		uint32_t rowBytes = 0;
		uint32_t numRows = 0;

		bool ret = UtilGetSurfaceInfo(w, h, format, &numBytes, &rowBytes, &numRows);
		if(!ret)
			return UploadFunctionResult::InvalidRequest;

		uint32_t subRowPitch = ((rowBytes + rowAlignment - 1) / rowAlignment) * rowAlignment;
		uint32_t subSlicePitch = (((subRowPitch * numRows) + sliceAlignment - 1) / sliceAlignment) * sliceAlignment;
		uint32_t subNumRows = numRows;
		uint32_t subDepth = d;
		uint32_t subRowSize = rowBytes;
		uint8_t* data = upload.Data + offset;

		if(!dataAlreadyFilled)
		{
			for(uint32_t z = 0; z < subDepth; ++z)
			{
				uint8_t* dstData = data + subSlicePitch * z;
				const uint8_t* pixelData = static_cast<const uint8_t*>((*images)[layer]->GetPixelData());

				//RGB also needs an alpha value
				if((*images)[layer]->GetColorFormat() == TRAP::Image::ColorFormat::RGB)
				{
					uint8_t alpha1Byte = 255;
					uint16_t alpha2Byte = 65535;
					float alphaHDR = 1.0f;
					uint32_t pixelDataByteSizePerChannel = (*images)[layer]->GetBytesPerPixel() / 3;
					uint64_t pixelDataSizeRGBA = (*images)[layer]->GetWidth() * (*images)[layer]->GetHeight() * 4 *
					                             pixelDataByteSizePerChannel;
					uint64_t pixelDataOffset = 0;
					for(uint64_t j = 0; j < pixelDataSizeRGBA; j += 4 * pixelDataByteSizePerChannel)
					{
						memcpy(dstData + j, pixelData + pixelDataOffset, 3 * pixelDataByteSizePerChannel);
						pixelDataOffset += 3 * pixelDataByteSizePerChannel;

						switch(pixelDataByteSizePerChannel)
						{
						case 1:
							memcpy(dstData + j + 3 * pixelDataByteSizePerChannel, &alpha1Byte,
							       1 * pixelDataByteSizePerChannel);
							break;

						case 2:
							memcpy(dstData + j + 3 * pixelDataByteSizePerChannel, &alpha2Byte,
							       1 * pixelDataByteSizePerChannel);
							break;

						case 4:
							memcpy(dstData + j + 3 * pixelDataByteSizePerChannel, &alphaHDR,
							       1 * pixelDataByteSizePerChannel);
							break;

						default:
							break;
						}
					}
				}
				else
					memcpy(dstData, pixelData, subRowSize * subNumRows);
			}
		}

		RendererAPI::SubresourceDataDesc subresourceDesc = {};
		subresourceDesc.ArrayLayer = layer;
		subresourceDesc.MipLevel = 0;
		subresourceDesc.SrcOffset = upload.Offset + offset;
		if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
		{
			subresourceDesc.RowPitch = subRowPitch;
			subresourceDesc.SlicePitch = subSlicePitch;
		}

		cmd->UpdateSubresource(texture, upload.Buffer, subresourceDesc);
		offset += subDepth * subSlicePitch;
	}
	// 	}
	// }

	//TODO Make API independent (Compute Shader approach)
	if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
	{
		//Mipmapping via vkCmdBlitImage

		//Check if image format supports linear blitting
		VkFormatProperties formatProperties;
		TRAP::Graphics::API::VulkanRenderer* vkRenderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>
			(
				RendererAPI::GetRenderer().get()
			);
		vkGetPhysicalDeviceFormatProperties(vkRenderer->GetDevice()->GetPhysicalDevice()->GetVkPhysicalDevice(),
			                                TRAP::Graphics::API::ImageFormatToVkFormat(texture->GetImageFormat()),
											&formatProperties);

		if(!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		{
			TRAP_ASSERT(false, "Texture image format does not support linear blitting!");
		}

		RendererAPI::TextureBarrier barrier{};
		barrier.Texture = texture;
		barrier.ArrayLayer = 0;
		barrier.SubresourceBarrier = true;

		int32_t mipWidth = texture->GetWidth();
		int32_t mipHeight = texture->GetHeight();

		for(uint32_t i = 1; i < texture->GetMipLevels(); ++i)
		{
			barrier.MipLevel = static_cast<uint8_t>(i) - 1;
			barrier.CurrentState = RendererAPI::ResourceState::CopyDestination;
			barrier.NewState = RendererAPI::ResourceState::CopySource;

			cmd->ResourceBarrier({}, {barrier}, {});

			VkImageBlit blit{};
			blit.srcOffsets[0] = {0, 0, 0};
			blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = {0, 0, 0,};
			blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>(cmd)->GetVkCommandBuffer(),
				texture->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				texture->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.CurrentState = RendererAPI::ResourceState::CopySource;
			barrier.NewState = RendererAPI::ResourceState::CopyDestination;

			cmd->ResourceBarrier({}, {barrier}, {});

			if(mipWidth > 1)
				mipWidth /= 2;
			if(mipHeight > 1)
				mipHeight /= 2;
		}
	}

	if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
	{
		RendererAPI::TextureBarrier barrier = {texture, RendererAPI::ResourceState::CopyDestination,
		                                       RendererAPI::ResourceState::ShaderResource};
		cmd->ResourceBarrier({}, {barrier}, {});
	}

	return UploadFunctionResult::Completed;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::UploadFunctionResult TRAP::Graphics::API::ResourceLoader::LoadTexture(const std::size_t activeSet,
																										   TRAP::Graphics::API::ResourceLoader::UpdateRequest& textureUpdate)
{
	TRAP::Graphics::RendererAPI::TextureLoadDesc& textureLoadDesc = std::get<TRAP::Graphics::RendererAPI::TextureLoadDesc>
		(
			textureUpdate.Desc
		);

	bool validMultiFileCubemap = true;
	for(const std::string& str : textureLoadDesc.Filepaths)
	{
		if(str.empty() || !TRAP::VFS::FileOrFolderExists(str, true) || !TRAP::Image::IsSupportedImageFile(str))
		{
			validMultiFileCubemap = false;
			break;
		}
	}

	bool supported = true;
	if((textureLoadDesc.Filepaths[0].empty() ||
	    !TRAP::VFS::FileOrFolderExists(textureLoadDesc.Filepaths[0], true) ||
	    !TRAP::Image::IsSupportedImageFile(textureLoadDesc.Filepaths[0])))
		supported = false;
	else if(textureLoadDesc.IsCubemap && textureLoadDesc.Type == RendererAPI::TextureCubeType::MultiFile &&
	        !validMultiFileCubemap)
		supported = false;

	TextureUpdateDescInternal updateDesc = {};
	TRAP::Graphics::RendererAPI::TextureDesc textureDesc = {};
	textureDesc.Depth = 1;//images[0]->GetDepth();?!
	textureDesc.ArraySize = 1;
	textureDesc.Descriptors = RendererAPI::DescriptorType::Texture;
	textureDesc.SampleCount = RendererAPI::SampleCount::SampleCount1;
	textureDesc.StartState = TRAP::Graphics::RendererAPI::ResourceState::Common;
	textureDesc.Flags |= textureLoadDesc.CreationFlag;

	if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan &&
	   textureLoadDesc.Desc != nullptr)
			textureDesc.VkSamplerYcbcrConversionInfo = textureLoadDesc.Desc->VkSamplerYcbcrConversionInfo;

	std::array<TRAP::Scope<TRAP::Image>, 6> images{};

	//TODO Mipmapping via Compute Shader
	//https://github.com/GPUOpen-Effects/FidelityFX-SPD/blob/master/sample/src/VK/CSDownsampler.glsl
	if(!textureLoadDesc.Filepaths[0].empty() && supported)
	{
		textureDesc.Name = TRAP::Utils::String::SplitString(textureLoadDesc.Filepaths[0], '/').back();

		if(textureLoadDesc.IsCubemap && textureLoadDesc.Type == RendererAPI::TextureCubeType::MultiFile)
		{
			for(uint32_t i = 0; i < images.size(); ++i)
				images[i] = TRAP::Image::LoadFromFile(textureLoadDesc.Filepaths[i]);

			//Validation checks
			bool valid = true;
			if(images[0]->GetWidth() != images[0]->GetHeight())
			{
				TP_ERROR(Log::TextureCubePrefix, "Images width and height must be the same!");
				valid = false;
			}
			for(uint32_t i = 1; i < images.size(); ++i)
			{
				if(!valid)
					break;

				//Check if every image has the same resolution as image 0
				if (images[0]->GetWidth() != images[i]->GetWidth() ||
					images[0]->GetHeight() != images[i]->GetHeight())
				{
					TP_ERROR(Log::TextureCubePrefix, "Images have mismatching width and/or height!");
					valid = false;
					break;
				}
				else if (images[0]->GetColorFormat() != images[i]->GetColorFormat() ||
					     images[0]->GetBitsPerPixel() != images[i]->GetBitsPerPixel())
				{
					TP_ERROR(Log::TextureCubePrefix, "Images have mismatching color formats and/or BPP!");
					valid = false;
					break;
				}
			}

			if(!valid) //Use FallbackCube
			{
				TP_WARN(Log::TextureCubePrefix, "Texture using FallbackCube Texture");
				textureDesc.Name = "FallbackCube";

				for (auto& image : images)
					image = TRAP::Image::LoadFallback();
			}

			textureDesc.Width = images[0]->GetWidth();
			textureDesc.Height = images[0]->GetHeight();
			textureDesc.Descriptors |= RendererAPI::DescriptorType::TextureCube;
			textureDesc.ArraySize = 6;
			textureDesc.MipLevels = Math::Max(1u, static_cast<uint32_t>
				(
					Math::Floor(Math::Log2(Math::Max(static_cast<float>(textureDesc.Width),
					                                 static_cast<float>(textureDesc.Height))))
				) + 1); //Minimum 1 Mip Level
		}
		/*else if(textureLoadDesc.IsCubemap && textureLoadDesc.Type == RendererAPI::TextureCubeType::Equirectangular)
		{
			//TODO
			//load image
			//TODO what now?
			//set texturedesc
			//arraysize = 6 ?!
			//descriptors texture, cubemap
		}*/
		else if(textureLoadDesc.IsCubemap && textureLoadDesc.Type == RendererAPI::TextureCubeType::Cross)
		{
			TRAP::Scope<TRAP::Image> baseImg = TRAP::Image::LoadFromFile(textureLoadDesc.Filepaths[0]);

			bool valid = true;
			uint32_t faceWidth = 0, faceHeight = 0;
			if(baseImg->GetWidth() > baseImg->GetHeight()) //Horizontal
			{
				if(baseImg->GetWidth() % 4 != 0 || baseImg->GetHeight() % 3 != 0)
				{
					valid = false;
					TP_ERROR(Log::TextureCubePrefix,
					         "Width must be a multiple of 4 & Height must be a multiple of 3!");
				}

				faceWidth = baseImg->GetWidth() / 4;
				faceHeight = baseImg->GetHeight() / 3;
			}
			else //Vertical
			{
				if(baseImg->GetWidth() % 3 != 0 || baseImg->GetHeight() % 4 != 0)
				{
					TP_ERROR(Log::TextureCubePrefix,
					         "Width must be a multiple of 3 & Height must be a multiple of 4!");
					valid = false;
				}

				faceWidth = baseImg->GetWidth() / 3;
				faceHeight = baseImg->GetHeight() / 4;
			}

			if(valid)
			{
				if(baseImg->IsHDR())
					images = SplitImageFromCross<float>(baseImg, faceWidth, faceHeight);
				else if ((baseImg->IsImageGrayScale() && baseImg->GetBitsPerPixel() == 16 && !baseImg->HasAlphaChannel()) ||
						 (baseImg->IsImageGrayScale() && baseImg->GetBitsPerPixel() == 32 &&  baseImg->HasAlphaChannel()) ||
						 (baseImg->IsImageColored()   && baseImg->GetBitsPerPixel() == 48 && !baseImg->HasAlphaChannel()) ||
						 (baseImg->IsImageColored()   && baseImg->GetBitsPerPixel() == 64 &&  baseImg->HasAlphaChannel()))
					images = SplitImageFromCross<uint16_t>(baseImg, faceWidth, faceHeight);
				else
					images = SplitImageFromCross<uint8_t>(baseImg, faceWidth, faceHeight);
			}
			else //Use FallbackCube
			{
				TP_WARN(Log::TextureCubePrefix, "Texture using FallbackCube Texture");
				textureDesc.Name = "FallbackCube";

				for (auto& image : images)
					image = TRAP::Image::LoadFallback();
			}

			textureDesc.Width = images[0]->GetWidth();
			textureDesc.Height = images[0]->GetHeight();
			textureDesc.Descriptors |= RendererAPI::DescriptorType::TextureCube;
			textureDesc.ArraySize = 6;
			textureDesc.MipLevels = Math::Max(1u, static_cast<uint32_t>
				(
					Math::Floor(Math::Log2(Math::Max(static_cast<float>(textureDesc.Width),
					                                 static_cast<float>(textureDesc.Height))))
				) + 1); //Minimum 1 Mip Level
		}
		else //if(!textureLoadDesc.IsCubemap) //Normal Texture
		{
			images[0] = TRAP::Image::LoadFromFile(textureLoadDesc.Filepaths[0]);

			textureDesc.Width = images[0]->GetWidth();
			textureDesc.Height = images[0]->GetHeight();
			textureDesc.MipLevels = Math::Max(1u, static_cast<uint32_t>
				(
					Math::Floor(Math::Log2(Math::Max(static_cast<float>(textureDesc.Width),
					                                 static_cast<float>(textureDesc.Height))))
				) + 1); //Minimum 1 Mip Level
		}

		if (images[0]->IsHDR() && images[0]->GetColorFormat() == TRAP::Image::ColorFormat::RGB) //Always no Alpha Channel
			textureDesc.Format = RendererAPI::ImageFormat::R32G32B32A32_SFLOAT;
		else if(images[0]->IsHDR() && images[0]->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale)
			textureDesc.Format = RendererAPI::ImageFormat::R32_SFLOAT;
		else if (images[0]->IsImageColored() && images[0]->GetBitsPerPixel() == 64 && images[0]->HasAlphaChannel())
			 textureDesc.Format = RendererAPI::ImageFormat::R16G16B16A16_UNORM;
		else if (images[0]->IsImageColored() && images[0]->GetBitsPerPixel() == 48 && !images[0]->HasAlphaChannel())
			 textureDesc.Format = RendererAPI::ImageFormat::R16G16B16A16_UNORM;
		else if (images[0]->IsImageColored() && images[0]->GetBitsPerPixel() == 32 && images[0]->HasAlphaChannel())
			textureDesc.Format = RendererAPI::ImageFormat::R8G8B8A8_UNORM;
		else if (images[0]->IsImageGrayScale() && images[0]->GetBitsPerPixel() == 16 && images[0]->HasAlphaChannel())
			 textureDesc.Format = RendererAPI::ImageFormat::R8G8_UNORM;
		else if (images[0]->IsImageGrayScale() && images[0]->GetBitsPerPixel() == 16 && !images[0]->HasAlphaChannel())
			textureDesc.Format = RendererAPI::ImageFormat::R16_UNORM;
		else if (images[0]->IsImageGrayScale() && images[0]->GetBitsPerPixel() == 8 && !images[0]->HasAlphaChannel())
			textureDesc.Format = RendererAPI::ImageFormat::R8_UNORM;
		else
			textureDesc.Format = RendererAPI::ImageFormat::R8G8B8A8_UNORM;

		//TODO Replace with Texture abstraction
		TRAP::Graphics::API::VulkanRenderer* vkRenderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>
			(
				RendererAPI::GetRenderer().get()
			);
		*textureLoadDesc.Texture = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(vkRenderer->GetDevice(),
		                                                                             textureDesc,
																					 vkRenderer->GetVMA());

		updateDesc.Texture = *textureLoadDesc.Texture;
		updateDesc.BaseMipLevel = 0;
		updateDesc.MipLevels = textureDesc.MipLevels;
		updateDesc.BaseArrayLayer = 0;
		updateDesc.LayerCount = textureDesc.ArraySize;

		return UpdateTexture(activeSet, updateDesc, &images);
	}

	//Fallback to default textures
	if(!textureLoadDesc.IsCubemap)
	{
		textureDesc.Name = "Fallback2D";

		images[0] = TRAP::Image::LoadFallback();

		textureDesc.Width = images[0]->GetWidth();
		textureDesc.Height = images[0]->GetHeight();
		textureDesc.Format = RendererAPI::ImageFormat::R8G8B8A8_SRGB;
		textureDesc.MipLevels = Math::Max(1u, static_cast<uint32_t>
			(
				Math::Floor(Math::Log2(Math::Max(static_cast<float>(textureDesc.Width),
				                                 static_cast<float>(textureDesc.Height))))
			) + 1); //Minimum 1 Mip Level
	}
	else
	{
		textureDesc.Name = "FallbackCube";

		for (auto& image : images)
			image = TRAP::Image::LoadFallback();

		textureDesc.Width = images[0]->GetWidth();
		textureDesc.Height = images[0]->GetHeight();
		textureDesc.Descriptors |= RendererAPI::DescriptorType::TextureCube;
		textureDesc.ArraySize = 6;
		textureDesc.Format = RendererAPI::ImageFormat::R8G8B8A8_SRGB;
		textureDesc.MipLevels = Math::Max(1u, static_cast<uint32_t>
			(
				Math::Floor(Math::Log2(Math::Max(static_cast<float>(textureDesc.Width),
				                                 static_cast<float>(textureDesc.Height))))
			) + 1); //Minimum 1 Mip Level
	}

	//TODO Replace with Texture abstraction
	TRAP::Graphics::API::VulkanRenderer* vkRenderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>
		(
			RendererAPI::GetRenderer().get()
		);
	*textureLoadDesc.Texture = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(vkRenderer->GetDevice(),
	                                                                             textureDesc,
																				 vkRenderer->GetVMA());

	updateDesc.Texture = *textureLoadDesc.Texture;
	updateDesc.BaseMipLevel = 0;
	updateDesc.MipLevels = textureDesc.MipLevels;
	updateDesc.BaseArrayLayer = 0;
	updateDesc.LayerCount = textureDesc.ArraySize;

	return UpdateTexture(activeSet, updateDesc, &images);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
std::array<TRAP::Scope<TRAP::Image>, 6> TRAP::Graphics::API::ResourceLoader::SplitImageFromCross(const TRAP::Scope<TRAP::Image>& image,
																							     const uint32_t faceWidth,
																								 const uint32_t faceHeight)
{
	const bool isHorizontal = image->GetWidth() > image->GetHeight();

	const uint32_t stride = image->GetBytesPerPixel();
	uint32_t face = 0;
	uint32_t cxLimit = 4, cyLimit = 3;
	if(!isHorizontal)
	{
		cxLimit = 3;
		cyLimit = 4;
	}

	std::array<std::vector<T>, 6> cubeTextureData;
	for(auto& i : cubeTextureData)
		i.resize(faceWidth * faceHeight * stride);

	for(uint32_t cy = 0; cy < cyLimit; ++cy)
	{
		for(uint32_t cx = 0; cx < cxLimit; ++cx)
		{
			if((cy == 0 || cy == 2 || cy == 3) && cx != 1)
				continue;

			for(uint32_t y = 0; y < faceHeight; ++y)
			{
				uint32_t offset = y;
				if(!isHorizontal && face == 5)
					offset = faceHeight - (y + 1);
				const uint32_t yp = cy * faceHeight + offset;

				for(uint32_t x = 0; x < faceWidth; ++x)
				{
					offset = x;
					if(!isHorizontal && face == 5)
						offset = faceWidth - (x + 1);
					const uint32_t xp = cx * faceWidth + offset;
					switch(stride)
					{
					case 1:
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 0];
						break;

					case 2:
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 0];
						cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 1];
						break;

					case 3:
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 0];
						cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 1];
						cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 2];
						break;

					case 4:
						cubeTextureData[face][(x + y * faceWidth) * stride + 0] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 0];
						cubeTextureData[face][(x + y * faceWidth) * stride + 1] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 1];
						cubeTextureData[face][(x + y * faceWidth) * stride + 2] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 2];
						cubeTextureData[face][(x + y * faceWidth) * stride + 3] = static_cast<const T*>
							(
								image->GetPixelData()
							)[(xp + yp * image->GetWidth()) * stride + 3];
						break;

					default:
						break;
					}
				}
			}
			++face;
		}
	}

	std::array<TRAP::Scope<TRAP::Image>, 6> images{};

	for(uint32_t i = 0; i < images.size(); ++i)
		images[i] = TRAP::Image::LoadFromMemory(faceWidth, faceHeight, image->GetColorFormat(), cubeTextureData[i]);

	return images;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(const RendererAPI::BufferUpdateDesc& buffer)
	: Type(UpdateRequestType::UpdateBuffer), Desc(buffer)
{}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(const RendererAPI::TextureLoadDesc& texture)
	: Type(UpdateRequestType::LoadTexture), Desc(texture)
{}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(TRAP::Graphics::API::ResourceLoader::TextureUpdateDescInternal texture)
	: Type(UpdateRequestType::UpdateTexture), Desc(texture)
{}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(const RendererAPI::BufferBarrier& barrier)
	: Type(UpdateRequestType::BufferBarrier), Desc(barrier)
{}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(const RendererAPI::TextureBarrier& barrier)
	: Type(UpdateRequestType::TextureBarrier), Desc(barrier)
{}