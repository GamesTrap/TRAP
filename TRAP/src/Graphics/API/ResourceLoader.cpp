#include "TRAPPCH.h"
#include "ResourceLoader.h"

#include "Objects/Fence.h"
#include "Objects/Buffer.h"
#include "Objects/CommandBuffer.h"
#include "Objects/CommandPool.h"
#include "Objects/Semaphore.h"
#include "Graphics/Textures/Texture.h"
#include "Vulkan/VulkanRenderer.h"
#include "Utils/String/String.h"
#include "FileSystem/FileSystem.h"

#include "Vulkan/VulkanCommon.h"
#include "Vulkan/Objects/VulkanDevice.h"
#include "Vulkan/Objects/VulkanPhysicalDevice.h"
#include "Vulkan/Objects/VulkanCommandBuffer.h"
#include "Vulkan/Objects/VulkanTexture.h"

TRAP::Graphics::RendererAPI::ResourceLoaderDesc TRAP::Graphics::API::ResourceLoader::DefaultResourceLoaderDesc{8ull << 20u, 2u};

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 MIP_REDUCE(const u32 size, const u32 mip) noexcept
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
#endif /*TRACY_ENABLE*/

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

		loader->m_nextSet = (loader->m_nextSet + 1u) % NumericCast<u32>(loader->m_copyEngine.ResourceSets.size());
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

		bool completionMask = false;

		std::vector<UpdateRequest> activeQueue;
		{
			std::lock_guard lock(loader->m_queueMutex);
			auto& mutex = loader->m_queueMutex;
			LockMark(mutex);

			std::vector<UpdateRequest>& requestQueue = loader->m_requestQueue;

			if(!requestQueue.empty())
				std::swap(requestQueue, activeQueue);
		}

		const usize requestCount = activeQueue.size();

		for(usize j = 0; j < requestCount; ++j)
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

			completionMask |= completed;

			if((updateState.WaitIndex != 0u) && completed)
			{
				TRAP_ASSERT(maxToken < updateState.WaitIndex, "ResourceLoader::StreamerThreadFunc(): Max sync token is smaller than current sync token!");
				maxToken = updateState.WaitIndex;
			}

			TRAP_ASSERT(result != UploadFunctionResult::StagingBufferFull, "ResourceLoader::StreamerThreadFunc() Staging buffer is full!");
		}

		if(completionMask)
		{
			loader->StreamerFlush(loader->m_nextSet);
			{
				std::lock_guard lock(loader->m_semaphoreMutex);
				auto& mutex = loader->m_semaphoreMutex;
				LockMark(mutex);

				loader->m_copyEngine.LastCompletedSemaphore = loader->m_copyEngine.ResourceSets[loader->m_nextSet].CopyCompletedSemaphore;
			}
		}

		const SyncToken nextToken = Math::Max<SyncToken>(maxToken, loader->GetLastTokenCompleted());
		loader->m_currentTokenState[loader->m_nextSet] = nextToken;

		//Signal submitted tokens
		{
			std::lock_guard lock(loader->m_tokenMutex);
			auto& mutex = loader->m_tokenMutex;
			LockMark(mutex);

			loader->m_tokenSubmitted = loader->m_currentTokenState[loader->m_nextSet];
			loader->m_tokenCond.notify_all();
		}
	}

	loader->StreamerFlush(loader->m_nextSet);
	loader->m_copyEngine.Queue->WaitQueueIdle();
	loader->CleanupCopyEngine();

	loader->FreeAllUploadMemory();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u32 TRAP::Graphics::API::ResourceLoader::UtilGetTextureRowAlignment() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return TRAP::Math::Max(1u, RendererAPI::GPUSettings.UploadBufferTextureRowAlignment);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u32 TRAP::Graphics::API::ResourceLoader::UtilGetTextureSubresourceAlignment(const TRAP::Graphics::API::ImageFormat fmt) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const u32 blockSize = Math::Max(1u, TRAP::Graphics::API::ImageFormatBitSizeOfBlock(fmt) >> 3u);
	const u32 alignment = ((RendererAPI::GPUSettings.UploadBufferTextureAlignment + blockSize - 1) /
	                            blockSize) * blockSize;

	const u32 rowAlignment = UtilGetTextureRowAlignment();
	return ((alignment + rowAlignment - 1) / rowAlignment) * rowAlignment;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::ResourceLoader(const RendererAPI::ResourceLoaderDesc* const desc)
	: m_desc(desc != nullptr ? *desc : DefaultResourceLoaderDesc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	SetupCopyEngine();

	//Create dedicated resource loader thread.
	m_thread = std::jthread(StreamerThreadFunc, this);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::~ResourceLoader()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	m_run = false;

	m_queueCond.notify_one();
	if(m_thread.joinable())
		m_thread.join();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::AddResource(RendererAPI::BufferLoadDesc& desc, SyncToken* const token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const u64 stagingBufferSize = m_copyEngine.BufferSize;
	const bool update = (desc.Data != nullptr) || desc.ForceReset;

	TRAP_ASSERT(stagingBufferSize > 0, "ResourceLoader::AddResource(): Staging buffer size is 0!");
	if(desc.Desc.MemoryUsage == RendererAPI::ResourceMemoryUsage::GPUOnly &&
	   (desc.Desc.StartState == RendererAPI::ResourceState::Undefined) && !update)
	{
		desc.Desc.StartState = UtilDetermineResourceStartState(desc.Desc);
		TP_WARN(Log::RendererBufferPrefix, "Buffer start state not provided. Determined the start state as ",
				Utils::String::ConvertToString(desc.Desc.StartState), "(", std::to_underlying(desc.Desc.StartState),
				") based on the provided BufferDesc");
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
			for(u64 offset = 0; offset < desc.Desc.Size; offset += stagingBufferSize)
			{
				const usize chunkSize = Math::Min<usize>(stagingBufferSize, desc.Desc.Size - offset);
				updateDesc.Size = chunkSize;
				updateDesc.DstOffset = offset;
				BeginUpdateResource(updateDesc);
				if (desc.ForceReset)
					std::fill_n(static_cast<u8*>(updateDesc.MappedData), chunkSize, u8(0u));
				else
				{
					TRAP_ASSERT(data, "ResourceLoader::AddResource(): Data is nullptr!");
					std::copy_n(static_cast<const u8*>(data) + offset, chunkSize, static_cast<u8*>(updateDesc.MappedData));
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
				std::fill_n(static_cast<u8*>(updateDesc.MappedData), desc.Desc.Size, u8(0u));
			else
			{
				TRAP_ASSERT(!desc.Desc.Size || desc.Data, "ResourceLoader::AddResource(): Data is nullptr!");
				if(desc.Data != nullptr)
					std::copy_n(static_cast<const u8*>(desc.Data), desc.Desc.Size, static_cast<u8*>(updateDesc.MappedData));
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

	if(std::get<0>(textureDesc.Filepaths).empty() && (std::get<0>(textureDesc.Images) == nullptr) && (textureDesc.Desc != nullptr))
	{
		TRAP_ASSERT(textureDesc.Desc->StartState != RendererAPI::ResourceState::Undefined, "ResourceLoader::AddResource(): Texture start state is undefined!");
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
				startState = UtilDetermineResourceStartState((textureDesc.Desc->Descriptors &
				                                              RendererAPI::DescriptorType::RWTexture) !=
															 RendererAPI::DescriptorType::Undefined);
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

	const u64 size = desc.Size > 0 ? desc.Size : (desc.Buffer->GetSize() - desc.DstOffset);
	TRAP_ASSERT(desc.DstOffset + size <= buffer->GetSize(), "ResourceLoader::BeginUpdateResource(): Update size is larger than buffer size!");

	const RendererAPI::ResourceMemoryUsage memoryUsage = desc.Buffer->GetMemoryUsage();
	if(memoryUsage != RendererAPI::ResourceMemoryUsage::GPUOnly)
	{
		const bool map = buffer->GetCPUMappedAddress() == nullptr;
		if (map)
			buffer->MapBuffer(nullptr);

		desc.Internal.MappedRange = { static_cast<u8*>(buffer->GetCPUMappedAddress()) + desc.DstOffset,
		                              buffer };
		desc.MappedData = desc.Internal.MappedRange.Data;
		desc.Internal.MappedRange.Flags = map ? std::to_underlying(MappedRangeFlag::UnMapBuffer) : 0;
	}
	else
	{
		//We need to use a staging buffer.
		const RendererAPI::MappedMemoryRange range = AllocateUploadMemory(size, 4U);
		desc.MappedData = range.Data;

		desc.Internal.MappedRange = range;
		desc.Internal.MappedRange.Flags = std::to_underlying(MappedRangeFlag::TempBuffer);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::BeginUpdateResource(RendererAPI::TextureUpdateDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const TRAP::Graphics::Texture* texture = desc.Texture;
	const TRAP::Graphics::API::ImageFormat fmt = texture->GetImageFormat();
	const u32 alignment = UtilGetTextureSubresourceAlignment(fmt);

	const bool success = UtilGetSurfaceInfo(MIP_REDUCE(texture->GetWidth(), desc.MipLevel),
	                                        MIP_REDUCE(texture->GetHeight(), desc.MipLevel),
	                                        fmt, &desc.SrcSliceStride, &desc.SrcRowStride, &desc.RowCount);
	TRAP_ASSERT(success, "ResourceLoader::BeginUpdateResource(): Failed to retrieve texture surface info!");

	const u32 rowAlignment = UtilGetTextureRowAlignment();
	desc.DstRowStride = ((desc.SrcRowStride + rowAlignment - 1) / rowAlignment) * rowAlignment;
	desc.DstSliceStride = (((desc.DstRowStride * desc.RowCount) + alignment - 1) / alignment) * alignment;

	const u64 requiredSize = (NumericCast<u64>((MIP_REDUCE(texture->GetDepth(), desc.MipLevel)) *
	                               desc.DstSliceStride + alignment - 1) / alignment) * alignment;

	//We need to use a staging buffer
	desc.Internal.MappedRange = AllocateUploadMemory(requiredSize, alignment);
	desc.Internal.MappedRange.Flags = std::to_underlying(MappedRangeFlag::TempBuffer);
	desc.MappedData = desc.Internal.MappedRange.Data;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::EndUpdateResource(RendererAPI::BufferUpdateDesc& desc, SyncToken* const token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if ((desc.Internal.MappedRange.Flags & std::to_underlying(MappedRangeFlag::UnMapBuffer)) != 0u)
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

[[nodiscard]] TRAP::Graphics::API::SyncToken TRAP::Graphics::API::ResourceLoader::GetLastTokenSubmitted() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_tokenSubmitted;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::API::ResourceLoader::IsTokenSubmitted(const SyncToken* token) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return *token <= m_tokenSubmitted;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::WaitForTokenSubmitted(const SyncToken* token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	std::unique_lock lock(m_tokenMutex);
	LockMark(m_tokenMutex);
	while (!IsTokenSubmitted(token))
		m_tokenCond.wait(lock);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Semaphore> TRAP::Graphics::API::ResourceLoader::GetLastSemaphoreCompleted()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	std::unique_lock lock(m_semaphoreMutex);
	LockMark(m_semaphoreMutex);

	return m_copyEngine.LastCompletedSemaphore;
}

//-------------------------------------------------------------------------------------------------------------------//

//Return a new staging buffer
[[nodiscard]] TRAP::Graphics::RendererAPI::MappedMemoryRange TRAP::Graphics::API::ResourceLoader::AllocateUploadMemory(const u64 memoryRequirement,
                                                                                                                       const u32 alignment)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	RendererAPI::BufferDesc desc{};
	desc.Size = memoryRequirement;
	desc.Alignment = alignment;
	desc.MemoryUsage = RendererAPI::ResourceMemoryUsage::CPUOnly;
	desc.Flags = RendererAPI::BufferCreationFlags::PersistentMap;
	const TRAP::Ref<Buffer> buffer = Buffer::Create(desc);

	return RendererAPI::MappedMemoryRange{static_cast<u8*>(buffer->GetCPUMappedAddress()), buffer,
	                                      0, memoryRequirement };
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::QueueBufferBarrier(const TRAP::Ref<Buffer>& buffer,
                                                             const RendererAPI::ResourceState state,
															 SyncToken* const token)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	SyncToken t{};
	{
		std::lock_guard lock(m_queueMutex);
		LockMark(m_queueMutex);

		t = m_tokenCounter.fetch_add(1) + 1;

		m_requestQueue.emplace_back(RendererAPI::BufferBarrier{buffer, RendererAPI::ResourceState::Undefined, state} );
		UpdateRequest& lastRequest = m_requestQueue.back();
		lastRequest.WaitIndex = t;
	}
	m_queueCond.notify_one();

	if (token != nullptr)
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

		t = m_tokenCounter.fetch_add(1) + 1;

		m_requestQueue.emplace_back(desc);
		UpdateRequest& lastRequest = m_requestQueue.back();
		lastRequest.WaitIndex = t;
		lastRequest.UploadBuffer = (desc.Internal.MappedRange.Flags & std::to_underlying(MappedRangeFlag::TempBuffer)) != 0u ?
									desc.Internal.MappedRange.Buffer : nullptr;
	}
	m_queueCond.notify_one();

	if (token != nullptr)
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

		t = m_tokenCounter.fetch_add(1) + 1;

		m_requestQueue.emplace_back(desc);
		UpdateRequest& lastRequest = m_requestQueue.back();
		lastRequest.WaitIndex = t;
	}
	m_queueCond.notify_one();

	if(token != nullptr)
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

		t = m_tokenCounter.fetch_add(1) + 1;

		m_requestQueue.emplace_back(textureUpdate);
		UpdateRequest& lastRequest = m_requestQueue.back();
		lastRequest.WaitIndex = t;
		lastRequest.UploadBuffer = (textureUpdate.Range.Flags & std::to_underlying(MappedRangeFlag::TempBuffer)) != 0u ?
								   textureUpdate.Range.Buffer : nullptr;
	}
	m_queueCond.notify_one();

	if(token != nullptr)
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

		t = m_tokenCounter.fetch_add(1) + 1;

		m_requestQueue.emplace_back(desc);
		UpdateRequest& lastRequest = m_requestQueue.back();
		lastRequest.WaitIndex = t;
	}
	m_queueCond.notify_one();

	if(token != nullptr)
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

		t = m_tokenCounter.fetch_add(1) + 1;

		m_requestQueue.emplace_back( RendererAPI::TextureBarrier{texture, RendererAPI::ResourceState::Undefined, state} );
		UpdateRequest& lastRequest = m_requestQueue.back();
		lastRequest.WaitIndex = t;
	}
	m_queueCond.notify_one();

	if (token != nullptr)
		*token = Math::Max(t, *token);
}

//-------------------------------------------------------------------------------------------------------------------//

//Return memory from pre-allocated staging buffer or create a temporary buffer if the streamer ran out of memory
[[nodiscard]] TRAP::Graphics::RendererAPI::MappedMemoryRange TRAP::Graphics::API::ResourceLoader::AllocateStagingMemory(const u64 memoryRequirement,
                                                                                                                        const u32 alignment)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	u64 offset = m_copyEngine.ResourceSets[m_nextSet].AllocatedSpace;
	if(alignment != 0)
		offset = ((offset + alignment - 1) / alignment) * alignment;

	CopyEngine::CopyResourceSet& resSet = m_copyEngine.ResourceSets[m_nextSet];
	const u64 size = resSet.Buffer->GetSize();
	const bool memoryAvailable = (offset < size) && (memoryRequirement <= size - offset);
	if(memoryAvailable && (resSet.Buffer->GetCPUMappedAddress() != nullptr))
	{
		const TRAP::Ref<Buffer> buffer = resSet.Buffer;
		TRAP_ASSERT(buffer->GetCPUMappedAddress(), "ResourceLoader::AllocateStagingMemory(): CPU mapped address of buffer is nullptr!");
		u8* const dstData = static_cast<u8*>(buffer->GetCPUMappedAddress()) + offset;
		m_copyEngine.ResourceSets[m_nextSet].AllocatedSpace = offset + memoryRequirement;
		return { dstData, buffer, offset, memoryRequirement };
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

	static constexpr u64 maxBlockSize = 32;
	const u64 size = Math::Max(m_desc.BufferSize, maxBlockSize);

	m_copyEngine.ResourceSets.resize(m_desc.BufferCount);
	for(auto& cpyResSet : m_copyEngine.ResourceSets)
	{
		cpyResSet.Fence = Fence::Create();
		RendererAPI::CommandPoolDesc cmdPoolDesc{};
		cmdPoolDesc.Queue = m_copyEngine.Queue;
		cpyResSet.CommandPool = CommandPool::Create(cmdPoolDesc);

		cpyResSet.Cmd = cpyResSet.CommandPool->AllocateCommandBuffer(false);

		cpyResSet.CopyCompletedSemaphore = Semaphore::Create();

		cpyResSet.Buffer = AllocateUploadMemory(size, UtilGetTextureSubresourceAlignment()).Buffer;
	}

	m_copyEngine.BufferSize = size;
	m_copyEngine.IsRecording = false;
	m_copyEngine.LastCompletedSemaphore = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::CleanupCopyEngine()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	for(u32 i = 0; i < m_copyEngine.ResourceSets.size(); ++i)
	{
		CopyEngine::CopyResourceSet& resourceSet = m_copyEngine.ResourceSets[i];

		resourceSet.CommandPool->FreeCommandBuffer(resourceSet.Cmd);

		if (!resourceSet.TempBuffers.empty())
			TP_WARN(Log::RendererBufferPrefix, "A temporary buffer was not cleaned up ", i);
	}

	m_copyEngine = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::WaitCopyEngineSet(const usize activeSet) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(!m_copyEngine.IsRecording, "ResourceLoader::WaitCopyEngineSet(): Copy Engine is still recording!");
	const CopyEngine::CopyResourceSet& resSet = m_copyEngine.ResourceSets[activeSet];

	if (resSet.Fence->GetStatus() == RendererAPI::FenceStatus::Incomplete)
		resSet.Fence->Wait();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ResourceLoader::ResetCopyEngineSet(const usize activeSet)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(!m_copyEngine.IsRecording, "ResourceLoader::ResetCopyEngineSet(): Copy Engine is still recording!");

	m_copyEngine.ResourceSets[activeSet].AllocatedSpace = 0;
	m_copyEngine.IsRecording = false;

	m_copyEngine.ResourceSets[activeSet].TempBuffers.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::CommandBuffer* TRAP::Graphics::API::ResourceLoader::AcquireCmd(const usize activeSet)
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

void TRAP::Graphics::API::ResourceLoader::StreamerFlush(const usize activeSet)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(!m_copyEngine.IsRecording)
		return;

	const CopyEngine::CopyResourceSet& resSet = m_copyEngine.ResourceSets[activeSet];
	resSet.Cmd->End();
	RendererAPI::QueueSubmitDesc submitDesc{};
	submitDesc.Cmds.push_back(resSet.Cmd);

	submitDesc.SignalSemaphores.push_back(resSet.CopyCompletedSemaphore);
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

	if((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) == 0u)
	{
		TP_ERROR(Log::RendererVulkanTexturePrefix, "Texture image format does not support linear blitting! "
		         "Skipping mip map generation");
		return;
	}

	RendererAPI::TextureBarrier barrier{};
	barrier.Texture = texture;
	barrier.ArrayLayer = 0;
	barrier.SubresourceBarrier = true;

	i32 mipWidth = NumericCast<i32>(texture->GetWidth());
	i32 mipHeight = NumericCast<i32>(texture->GetHeight());

	for(u32 i = 1; i < texture->GetMipLevels(); ++i)
	{
		barrier.MipLevel = NumericCast<u8>(i) - 1;
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

[[nodiscard]] TRAP::Graphics::API::ResourceLoader::UploadFunctionResult TRAP::Graphics::API::ResourceLoader::UpdateBuffer(const usize activeSet,
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

[[nodiscard]] TRAP::Graphics::API::ResourceLoader::UploadFunctionResult TRAP::Graphics::API::ResourceLoader::UpdateTexture(const usize activeSet,
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

	const u32 sliceAlignment = UtilGetTextureSubresourceAlignment(format);
	const u32 rowAlignment = UtilGetTextureRowAlignment();
	const u32 requiredSize = UtilGetSurfaceSize(format, texture->GetWidth(), texture->GetHeight(),
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
		                                              AllocateStagingMemory(requiredSize, sliceAlignment);
	u64 offset = 0;

	if(upload.Data == nullptr)
		return UploadFunctionResult::StagingBufferFull;

	const u32 firstStart = textureUpdateDesc.MipsAfterSlice ? textureUpdateDesc.BaseMipLevel :
		                            textureUpdateDesc.BaseArrayLayer;
	const u32 firstEnd = textureUpdateDesc.MipsAfterSlice ?
		                          (textureUpdateDesc.BaseMipLevel + textureUpdateDesc.MipLevels) :
		                          (textureUpdateDesc.BaseArrayLayer + textureUpdateDesc.LayerCount);
	const u32 secondStart = textureUpdateDesc.MipsAfterSlice ? textureUpdateDesc.BaseArrayLayer :
		                             textureUpdateDesc.BaseMipLevel;
	const u32 secondEnd = textureUpdateDesc.MipsAfterSlice ?
		                           (textureUpdateDesc.BaseArrayLayer + textureUpdateDesc.LayerCount) :
		                           (textureUpdateDesc.BaseMipLevel + textureUpdateDesc.MipLevels);

	for(u32 j = firstStart; j < firstEnd; ++j)
	{
		for(u32 i = secondStart; i < secondEnd; ++i)
		{
			const u32 mip = textureUpdateDesc.MipsAfterSlice ? j : i; //TODO For now only upload mip level 0 and use the Vulkan way to blit
			                                                         //them to their mips till I know that Compute Shaders work
			const u32 layer = textureUpdateDesc.MipsAfterSlice ? i : j;

			const u32 w = MIP_REDUCE(texture->GetWidth(), mip);
			const u32 h = MIP_REDUCE(texture->GetHeight(), mip);
			const u32 d = MIP_REDUCE(texture->GetDepth(), mip);

			u32 rowBytes = 0;
			u32 numRows = 0;

			const bool ret = UtilGetSurfaceInfo(w, h, format, nullptr, &rowBytes, &numRows);
			if(!ret)
				return UploadFunctionResult::InvalidRequest;

			const u32 subRowPitch = ((rowBytes + rowAlignment - 1) / rowAlignment) * rowAlignment;
			const u32 subSlicePitch = (((subRowPitch * numRows) + sliceAlignment - 1) / sliceAlignment) * sliceAlignment;
			const u32 subNumRows = numRows;
			const u32 subDepth = d;
			const u32 subRowSize = rowBytes;
			u8* const data = upload.Data + offset;

			if(!dataAlreadyFilled)
			{
				for(u32 z = 0; z < subDepth; ++z)
				{
					//Convert to RGBA if necessary
					const u8* pixelData = nullptr;
					TRAP::Scope<TRAP::Image> RGBAImage = nullptr;
					if((*images)[layer]->GetColorFormat() == TRAP::Image::ColorFormat::RGB) //Convert RGB to RGBA
					{
						RGBAImage = TRAP::Image::ConvertRGBToRGBA((*images)[layer]);
						pixelData = static_cast<const u8*>(RGBAImage->GetPixelData());
					}
					else
						pixelData = static_cast<const u8*>((*images)[layer]->GetPixelData());

					u8* const dstData = data + subSlicePitch * z;
					for(u64 r = 0; r < subNumRows; ++r)
						std::copy_n(pixelData + r * subRowSize, subRowSize, dstData + r * subRowPitch);
				}
			}

			RendererAPI::SubresourceDataDesc subresourceDesc = {};
			subresourceDesc.ArrayLayer = layer;
			subresourceDesc.MipLevel = mip;
			subresourceDesc.SrcOffset = upload.Offset + offset;
			if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
			{
				subresourceDesc.RowPitch = subRowPitch;
				subresourceDesc.SlicePitch = subSlicePitch;
			}

			cmd->UpdateSubresource(texture, upload.Buffer, subresourceDesc);
			offset += NumericCast<u64>(subDepth) * subSlicePitch;
		}
	}

	if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan && texture->GetMipLevels() > 1)
		VulkanGenerateMipMaps(texture, cmd); //Mipmapping via vkCmdBlitImage
	//D3D12/DirectX 12 would need a compute shader to generate mip maps (as there is no vkCmdBlitImage equivalent)
	//https://github.com/GPUOpen-Effects/FidelityFX-SPD/blob/master/sample/src/VK/CSDownsampler.glsl

	if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
	{
		const RendererAPI::ResourceState finalLayout = UtilDetermineResourceStartState((texture->GetDescriptorTypes() & RendererAPI::DescriptorType::RWTexture) != RendererAPI::DescriptorType::Undefined);
		const RendererAPI::TextureBarrier barrier{texture, RendererAPI::ResourceState::CopyDestination,
		                                          finalLayout};
		cmd->ResourceBarrier(nullptr, &barrier, nullptr);
	}

	return UploadFunctionResult::Completed;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::ResourceLoader::UploadFunctionResult TRAP::Graphics::API::ResourceLoader::LoadTexture(const usize activeSet,
																										                 TRAP::Graphics::API::ResourceLoader::UpdateRequest& textureUpdate)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const TRAP::Graphics::RendererAPI::TextureLoadDesc& textureLoadDesc = std::get<TRAP::Graphics::RendererAPI::TextureLoadDesc>
		(
			textureUpdate.Desc
		);

	bool validMultiFileCubemap = true;
	if(std::get<0>(textureLoadDesc.Images) == nullptr)
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
			if(img == nullptr)
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

	if((std::get<0>(textureLoadDesc.Images) == nullptr) && supported)
	{
		if((std::get<0>(textureLoadDesc.Filepaths).empty() ||
			!TRAP::FileSystem::Exists(std::get<0>(textureLoadDesc.Filepaths)) ||
			!TRAP::Image::IsSupportedImageFile(std::get<0>(textureLoadDesc.Filepaths))))
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
	if((textureDesc.Flags & RendererAPI::TextureCreationFlags::Storage) != RendererAPI::TextureCreationFlags::None)
		textureDesc.Descriptors |= RendererAPI::DescriptorType::RWTexture;

	if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan &&
	   textureLoadDesc.Desc != nullptr)
			textureDesc.VkSamplerYcbcrConversionInfo = textureLoadDesc.Desc->VkSamplerYcbcrConversionInfo;

	std::array<TRAP::Scope<TRAP::Image>, 6> ownedImages{}; //Storage for images that are getting loaded from here
	std::array<const TRAP::Image*, 6> ptrImages{}; //Pointers to the images to upload

	if((!std::get<0>(textureLoadDesc.Filepaths).empty() || (std::get<0>(textureLoadDesc.Images) != nullptr)) && supported)
	{
		//Get a name for the texture
		textureDesc.Name = "Unknown";
		if(std::get<0>(textureLoadDesc.Images) == nullptr)
		{
			//Use file path
			const auto fileName = FileSystem::GetFileNameWithEnding(std::get<0>(textureLoadDesc.Filepaths));
			if(fileName)
				textureDesc.Name = *fileName;
		}
		else if(!std::get<0>(textureLoadDesc.Images)->GetFilePath().empty())
		{
			//Use already in memory image
			const auto fileName = FileSystem::GetFileNameWithEnding(std::get<0>(textureLoadDesc.Images)->GetFilePath());
			if(fileName)
				textureDesc.Name = *fileName;
		}

		//Handle the different types of textures
		if(textureLoadDesc.IsCubemap && textureLoadDesc.Type == RendererAPI::TextureCubeType::MultiFile)
		{
			if(std::get<0>(textureLoadDesc.Images) == nullptr)
			{
				//Use file paths
				for(usize i = 0; i < ownedImages.size(); ++i)
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
			if(std::get<0>(ptrImages)->GetWidth() != std::get<0>(ptrImages)->GetHeight())
			{
				TP_ERROR(Log::TexturePrefix, "Images width and height must be the same!");
				valid = false;
			}
			for(usize i = 1; i < ptrImages.size(); ++i)
			{
				if(!valid)
					break;

				//Check if every image has the same resolution as image 0
				if (std::get<0>(ptrImages)->GetWidth() != ptrImages[i]->GetWidth() ||
					std::get<0>(ptrImages)->GetHeight() != ptrImages[i]->GetHeight())
				{
					TP_ERROR(Log::TexturePrefix, "Images have mismatching width and/or height!");
					valid = false;
					break;
				}
				if (std::get<0>(ptrImages)->GetColorFormat() != ptrImages[i]->GetColorFormat() ||
					std::get<0>(ptrImages)->GetBitsPerPixel() != ptrImages[i]->GetBitsPerPixel())
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

				for(usize i = 0; i < ownedImages.size(); ++i)
				{
					ownedImages[i] = TRAP::Image::LoadFallback();
					ptrImages[i] = ownedImages[i].get();
				}
			}

			textureDesc.Width = std::get<0>(ptrImages)->GetWidth();
			textureDesc.Height = std::get<0>(ptrImages)->GetHeight();
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
			if(std::get<0>(textureLoadDesc.Images) == nullptr)
				baseImg = TRAP::Image::LoadFromFile(std::get<0>(textureLoadDesc.Filepaths));
			const TRAP::Image* const baseImgPtr = std::get<0>(textureLoadDesc.Images) != nullptr ? std::get<0>(textureLoadDesc.Images) : baseImg.get();

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
				    ownedImages = TRAP::Graphics::Texture::SplitImageFromCross<f32>(baseImgPtr);
				else if (baseImgPtr->IsLDR() && baseImgPtr->GetBytesPerChannel() == 2)
					ownedImages = TRAP::Graphics::Texture::SplitImageFromCross<u16>(baseImgPtr);
				else /*if (baseImgPtr->IsLDR() && baseImgPtr->GetBytesPerChannel() == 1)*/
					ownedImages = TRAP::Graphics::Texture::SplitImageFromCross<u8>(baseImgPtr);
			}
			else //Use FallbackCube
			{
				TP_WARN(Log::TexturePrefix, "Texture using FallbackCube texture");
				textureDesc.Name = "FallbackCube";

				for(auto & ownedImage : ownedImages)
					ownedImage = TRAP::Image::LoadFallback();
			}

			for(usize i = 0; i < ownedImages.size(); ++i)
				ptrImages[i] = ownedImages[i].get();

			textureDesc.Width = std::get<0>(ptrImages)->GetWidth();
			textureDesc.Height = std::get<0>(ptrImages)->GetHeight();
			textureDesc.Descriptors |= RendererAPI::DescriptorType::TextureCube;
			textureDesc.ArraySize = 6;
			textureDesc.MipLevels = TRAP::Graphics::Texture::CalculateMipLevels(textureDesc.Width,
			                                                                    textureDesc.Height);
		}
		else //if(!textureLoadDesc.IsCubemap) //Normal Texture
		{
			if(std::get<0>(textureLoadDesc.Images) == nullptr)
				std::get<0>(ownedImages) = TRAP::Image::LoadFromFile(std::get<0>(textureLoadDesc.Filepaths));
			std::get<0>(ptrImages) = std::get<0>(textureLoadDesc.Images) != nullptr ? std::get<0>(textureLoadDesc.Images) : std::get<0>(ownedImages).get();

			textureDesc.Width = std::get<0>(ptrImages)->GetWidth();
			textureDesc.Height = std::get<0>(ptrImages)->GetHeight();
			textureDesc.MipLevels = TRAP::Graphics::Texture::CalculateMipLevels(textureDesc.Width,
			                                                                    textureDesc.Height);
		}

		textureDesc.Format = TRAP::Graphics::API::ImageFormat::R8G8B8A8_UNORM;

		if (std::get<0>(ptrImages)->IsHDR() && std::get<0>(ptrImages)->GetColorFormat() == TRAP::Image::ColorFormat::RGB) //RGB HDR (32 bpc) | Will be converted to RGBA before upload
			textureDesc.Format = TRAP::Graphics::API::ImageFormat::R32G32B32A32_SFLOAT;
		else if (std::get<0>(ptrImages)->GetBitsPerChannel() == 16 && (std::get<0>(ptrImages)->GetColorFormat() == TRAP::Image::ColorFormat::RGBA || //RGB(A) 16 bpc | Will be converted to RGBA before upload
				 std::get<0>(ptrImages)->GetColorFormat() == TRAP::Image::ColorFormat::RGB))
			 textureDesc.Format = TRAP::Graphics::API::ImageFormat::R16G16B16A16_UNORM;
		else if (std::get<0>(ptrImages)->GetBitsPerChannel() == 8 && std::get<0>(ptrImages)->GetColorFormat() == TRAP::Image::ColorFormat::RGBA) //RGBA 8 bpc
			textureDesc.Format = TRAP::Graphics::API::ImageFormat::R8G8B8A8_UNORM;
		else if (std::get<0>(ptrImages)->GetBitsPerChannel() == 32 && std::get<0>(ptrImages)->GetColorFormat() == TRAP::Image::ColorFormat::GrayScaleAlpha) //GrayScale Alpha HDR (32 bpc)
			textureDesc.Format = TRAP::Graphics::API::ImageFormat::R32G32_SFLOAT;
		else if (std::get<0>(ptrImages)->GetBitsPerChannel() == 16 && std::get<0>(ptrImages)->GetColorFormat() == TRAP::Image::ColorFormat::GrayScaleAlpha) //GrayScale Alpha 16 bpc
			textureDesc.Format = TRAP::Graphics::API::ImageFormat::R16G16_UNORM;
		else if (std::get<0>(ptrImages)->GetBitsPerChannel() == 8 && std::get<0>(ptrImages)->GetColorFormat() == TRAP::Image::ColorFormat::GrayScaleAlpha) //GrayScale Alpha 8 bpc
			 textureDesc.Format = TRAP::Graphics::API::ImageFormat::R8G8_UNORM;
		else if(std::get<0>(ptrImages)->IsHDR() && std::get<0>(ptrImages)->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale) //GrayScale HDR (32 bpc)
			textureDesc.Format = TRAP::Graphics::API::ImageFormat::R32_SFLOAT;
		else if (std::get<0>(ptrImages)->GetBitsPerChannel() == 16 && std::get<0>(ptrImages)->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale) //GrayScale 16 bpc
			textureDesc.Format = TRAP::Graphics::API::ImageFormat::R16_UNORM;
		else if (std::get<0>(ptrImages)->GetBitsPerChannel() == 8 && std::get<0>(ptrImages)->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale) //GrayScale 8 bpc
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

		std::get<0>(ownedImages) = TRAP::Image::LoadFallback();
		std::get<0>(ptrImages) = std::get<0>(ownedImages).get();

		textureDesc.Width = std::get<0>(ptrImages)->GetWidth();
		textureDesc.Height = std::get<0>(ptrImages)->GetHeight();
		textureDesc.Format = TRAP::Graphics::API::ImageFormat::R8G8B8A8_UNORM;
		textureDesc.MipLevels = TRAP::Graphics::Texture::CalculateMipLevels(textureDesc.Width,
			                                                                textureDesc.Height);
	}
	else
	{
		textureDesc.Name = "FallbackCube";

		for(usize i = 0; i < ownedImages.size(); ++i)
		{
			ownedImages[i] = TRAP::Image::LoadFallback();
			ptrImages[i] = ownedImages[i].get();
		}

		textureDesc.Width = std::get<0>(ptrImages)->GetWidth();
		textureDesc.Height = std::get<0>(ptrImages)->GetHeight();
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

[[nodiscard]] TRAP::Graphics::API::ResourceLoader::UploadFunctionResult TRAP::Graphics::API::ResourceLoader::CopyTexture(usize activeSet,
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

	u32 rowBytes = 0;
	u32 numRows = 0;

	const bool ret = UtilGetSurfaceInfo(texture->GetWidth(), texture->GetHeight(), format, nullptr, &rowBytes, &numRows);
	if(!ret)
		return UploadFunctionResult::InvalidRequest;

	RendererAPI::SubresourceDataDesc subresourceDesc{};
	subresourceDesc.ArrayLayer = textureCopy.ArrayLayer;
	subresourceDesc.MipLevel = textureCopy.MipLevel;
	subresourceDesc.SrcOffset = textureCopy.BufferOffset;

	if(RendererAPI::GetRenderAPI() == RenderAPI::Vulkan)
	{
		const u32 sliceAlignment = UtilGetTextureSubresourceAlignment(format);
		const u32 rowAlignment = UtilGetTextureRowAlignment();
		const u32 subRowPitch = ((rowBytes + rowAlignment - 1) / rowAlignment) * rowAlignment;
		const u32 subSlicePitch = (((subRowPitch * numRows) + sliceAlignment - 1) / sliceAlignment) * sliceAlignment;
		subresourceDesc.RowPitch = subRowPitch;
		subresourceDesc.SlicePitch = subSlicePitch;
	}

	cmd->CopySubresource(textureCopy.Buffer.get(), textureCopy.Texture.get(), subresourceDesc);

	return UploadFunctionResult::Completed;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(TRAP::Graphics::API::ResourceLoader::TextureUpdateDescInternal texture) noexcept
	: Type(UpdateRequestType::UpdateTexture), Desc(texture)
{}
