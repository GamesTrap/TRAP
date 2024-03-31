#ifndef TRAP_RESOURCELOADER_H
#define TRAP_RESOURCELOADER_H

#include <atomic>
#include <condition_variable>
#include <thread>
#include <shared_mutex>

#include "RendererAPI.h"
#include "ImageLoader/Image.h"
#include "Maths/Math.h"

namespace TRAP::Graphics
{
	class CommandPool;
	class Texture;
}

namespace TRAP::Graphics::API
{
	using SyncToken = u64;

	/// @brief Resource loader.
	/// Does the heavy lifting of sending data between the CPU and GPU.
	class ResourceLoader
	{
	public:
		/// @brief Instantiate the resource loader.
		/// @param desc Optional settings.
		explicit ResourceLoader(const RendererAPI::ResourceLoaderDesc* desc = nullptr);
		/// @brief Destructor.
		~ResourceLoader() = default;

		/// @brief Copy constructor.
		consteval ResourceLoader(const ResourceLoader&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval ResourceLoader& operator=(const ResourceLoader&) noexcept = delete;
		/// @brief Move constructor.
		consteval ResourceLoader(ResourceLoader&&) noexcept = delete;
		/// @brief Move assignment operator.
		consteval ResourceLoader& operator=(ResourceLoader&&) noexcept = delete;

		//Adding and updating resources can be done using a AddResource or BeginUpdateResource/EndUpdateResource
		//pair.

		/// @brief Add a resource to GPU.
		/// If called with a data size larger than the ResourceLoader's staging buffer,
		/// the ResourceLoader will perform multiple copies/flushes rather than failing the copy.
		/// @param desc Description of buffer resource to add.
		/// @param token
		/// If token is nullptr, the resource will be available when AllResourceLoadsCompleted() returns true.
		/// If token is not nullptr, the resource will be available after IsTokenCompleted(token) returns true.
		void AddResource(RendererAPI::BufferLoadDesc& desc, SyncToken* token);
		/// @brief Add a resource to GPU.
		/// @param textureDesc Description of texture resource to add.
		/// @param token
		/// If token is nullptr, the resource will be available when AllResourceLoadsCompleted() returns true.
		/// If token is not nullptr, the resource will be available after IsTokenCompleted(token) returns true.
		void AddResource(RendererAPI::TextureLoadDesc& textureDesc, SyncToken* token);

		/// @brief Start updating a buffer resource.
		/// @param desc Description of buffer resource to update.
		static void BeginUpdateResource(RendererAPI::BufferUpdateDesc& desc);
		/// @brief Start updating a texture resource.
		/// @param desc Description of texture resource to update.
		static void BeginUpdateResource(RendererAPI::TextureUpdateDesc& desc);
		/// @brief End updating a buffer resource.
		/// @param desc Description of updated buffer resource.
		/// @param token
		/// If token is nullptr, the resource will be available when AllResourceLoadsCompleted() returns true.
		/// If token is not nullptr, the resource will be available after IsTokenCompleted(token) returns true.
		void EndUpdateResource(RendererAPI::BufferUpdateDesc& desc, SyncToken* token);
		/// @brief End updating a texture resource.
		/// @param desc Description of updated texture resource.
		/// @param token
		/// If token is nullptr, the resource will be available when AllResourceLoadsCompleted() returns true.
		/// If token is not nullptr, the resource will be available after IsTokenCompleted(token) returns true.
		void EndUpdateResource(RendererAPI::TextureUpdateDesc& desc, SyncToken* token);

		/// @brief Copy data from GPU to the CPU.
		/// For optimal use, the amount of data to transfer should be minimized as much as possible and
		/// applications should provide additional graphics/compute work that the GPU can execute alongside the copy.
		/// @param textureDesc Description of the texture copy.
		/// @param token
		/// If token is nullptr, the resource will be available when AllResourceLoadsCompleted() returns true.
		/// If token is not nullptr, the resource will be available after IsTokenCompleted(token) return true.
		void CopyResource(RendererAPI::TextureCopyDesc& textureDesc, SyncToken* token);

		/// @brief Retrieve whether all submitted resource loads and updates have been completed.
		/// @return True if all loads and updates have finished, false otherwise.
		[[nodiscard]] bool AllResourceLoadsCompleted() const noexcept;

		/// @brief Block the calling thread until AllResourceLoadsCompleted() returns true.
		/// @note If more resource loads or updates are submitted from a different thread
		///       while the calling thread is blocked, those loads or updates are not guaranteed
		///       to have completed when this function returns.
		void WaitForAllResourceLoads();

		//A SyncToken is an array of monotonically increasing integers.

		/// @brief Retrieve the last sync token which has finished loading or updating.
		/// @return Last value for which IsTokenCompleted(token) is guaranteed to return true.
		[[nodiscard]] SyncToken GetLastTokenCompleted() const noexcept;
		/// @brief Retrieve whether a sync token has finished loading or updating.
		/// @param token Token to check.
		/// @return True if the token has finished, false otherwise.
		[[nodiscard]] bool IsTokenCompleted(const SyncToken* token) const;
		/// @brief Block the calling thread until IsTokenCompleted(token) for the provided token returns true.
		/// @param token Token to wait for completion.
		void WaitForToken(const SyncToken* token);

		//Allows clients to synchronize with the submission of copy commands (as opposed to their completion).
		//This can reduce the wait time for client but requires using the Semaphore from GetLastSemaphoreCompleted()
		//in a wait operation in a submit that uses the textures just updated.

		/// @brief Retrieve the last sync token which has been submitted for copying.
		/// @return Last value for which IsTokenSubmitted(token) is guaranteed to return true.
		[[nodiscard]] SyncToken GetLastTokenSubmitted() const noexcept;
		/// @brief Retrieve whether a sync token has been submitted for copying.
		/// @param token Token to check.
		/// @return True if the token has been submitted for copying, false otherwise.
		[[nodiscard]] bool IsTokenSubmitted(const SyncToken* token) const;
		/// @brief Block the calling thread until IsTokenSubmitted(token) for the provided token returns true.
		/// @param token Token to wait for submission.
		void WaitForTokenSubmitted(const SyncToken* token);

		/// @brief Retrieve the semaphore for the last copy operation that has been submitted.
		/// @return Semaphore for the last copy operation that has been submitted.
		/// @note This can be nullptr if no operations have been executed.
		[[nodiscard]] TRAP::Ref<Semaphore> GetLastSemaphoreCompleted();

	private:
		struct TextureUpdateDescInternal
		{
			TRAP::Graphics::Texture* Texture = nullptr;
			RendererAPI::MappedMemoryRange Range{};
			u32 BaseMipLevel = 0;
			u32 MipLevels = 0;
			u32 BaseArrayLayer = 0;
			u32 LayerCount = 0;
			bool MipsAfterSlice = false;
		};
		struct UpdateRequest;

		static RendererAPI::ResourceLoaderDesc DefaultResourceLoaderDesc;
		/// @brief Resource loader thread.
		/// Used to async load and update queued resources.
		/// @param stopToken Token to use for stop request of the thread.
		/// @param loader Pointer to the ResourceLoader instance.
		static void StreamerThreadFunc(const std::stop_token& stopToken, ResourceLoader* loader);
		/// @brief Retrieve the row alignment for textures used by the GPU.
		/// @return Row alignment used by the GPU.
		[[nodiscard]] static u32 UtilGetTextureRowAlignment() noexcept;
		/// @brief Retrieve the subresource alignment for textures used by the GPU.
		/// @param fmt Format of the texture. Default: ImageFormat::Undefined.
		/// @return Subresource alignment used by the GPU.
		[[nodiscard]] static u32 UtilGetTextureSubresourceAlignment(TRAP::Graphics::API::ImageFormat fmt = TRAP::Graphics::API::ImageFormat::Undefined) noexcept;
		/// @brief </summary>
		/// @param fmt Image format of the texture.
		/// @param width Width of the texture.
		/// @param height Height of the texture.
		/// @param depth Depth of the texture.
		/// @param rowStride Row stride of the texture.
		/// @param sliceStride Slice stride of the texture.
		/// @param baseMipLevel Base mip level of the texture.
		/// @param mipLevels Number of mip levels of the texture.
		/// @param baseArrayLayer Base array layer of the texture.
		/// @param arrayLayers Number of array layers of the texture.
		/// @return Total surface size of the texture.
		[[nodiscard]] static constexpr u32 UtilGetSurfaceSize(TRAP::Graphics::API::ImageFormat fmt, u32 width, u32 height,
		                                                           u32 depth, u32 rowStride, u32 sliceStride,
										                           u32 baseMipLevel, u32 mipLevels, u32 baseArrayLayer,
										                           u32 arrayLayers) noexcept;
		/// @brief Retrieve information about a texture.
		/// @param width Width of the texture.
		/// @param height Height of the texture.
		/// @param fmt Image format of the texture.
		/// @param outNumBytes Optional output for the number of bytes in the texture.
		/// @param outRowBytes Optional output for the number of bytes in a row of the texture.
		/// @param outNumRows Optional output for the number of rows in the texture.
		/// @return True on successful retrieval, false otherwise.
		[[nodiscard]] static constexpr bool UtilGetSurfaceInfo(u32 width, u32 height, TRAP::Graphics::API::ImageFormat fmt,
		                                                       u32* outNumBytes, u32* outRowBytes, u32* outNumRows);
		/// @brief Allocate a new staging buffer.
		/// @param memoryRequirement Required memory size.
		/// @param alignment Memory alignment.
		/// @return Staging buffer information.
		[[nodiscard]] static RendererAPI::MappedMemoryRange AllocateUploadMemory(u64 memoryRequirement, u32 alignment);
		/// @brief Queue a buffer barrier.
		/// @param buffer Buffer to queue a barrier on.
		/// @param state State to transition the buffer to.
		/// @param token Optional output sync token.
		void QueueBufferBarrier(const TRAP::Ref<Buffer>& buffer, RendererAPI::ResourceState state, SyncToken* token);
		/// @brief Queue a buffer update.
		/// @param desc Description of buffer update.
		/// @param token Optional output sync token.
		void QueueBufferUpdate(const RendererAPI::BufferUpdateDesc& desc, SyncToken* token);
		/// @brief Queue a texture load.
		/// @param desc Description of texture load.
		/// @param token Optional output sync token.
		void QueueTextureLoad(const RendererAPI::TextureLoadDesc& desc, SyncToken* token);
		/// @brief Queue a texture update.
		/// @param textureUpdate Internal description of texture update.
		/// @param token Optional output sync token.
		void QueueTextureUpdate(const TextureUpdateDescInternal& textureUpdate, SyncToken* token);
		/// @brief Queue a texture copy.
		/// @param desc Description of texture copy.
		/// @param token Optional output sync token.
		void QueueTextureCopy(const RendererAPI::TextureCopyDesc& desc, SyncToken* token);
		/// @brief Queue a texture barrier.
		/// @param texture Texture to queue a barrier on.
		/// @param state State to transition the texture to.
		/// @param token Optional output sync token.
		void QueueTextureBarrier(TRAP::Graphics::Texture* texture,
		                         RendererAPI::ResourceState state, SyncToken* token);

		/// @brief Retrieve memory from pre-allocated staging buffer or create a
		/// temporary buffer if the streamer ran out of memory.
		/// @param memoryRequirement Required memory size.
		/// @param alignment Memory alignment.
		/// @return Memory from pre-allocated staging buffer or temporary buffer.
		[[nodiscard]] RendererAPI::MappedMemoryRange AllocateStagingMemory(u64 memoryRequirement, u32 alignment);
		/// @brief Free all upload memory and temporary buffers.
		void FreeAllUploadMemory() noexcept;

		/// @brief Initialize the copy engine.
		void SetupCopyEngine();
		/// @brief Cleanup the copy engine.
		void CleanupCopyEngine();
		/// @brief Wait for a copy engine resource set to complete its work.
		/// @param activeSet Copy engine resource set to wait for.
		void WaitCopyEngineSet(usize activeSet) const;
		/// @brief Reset a copy engine resource set.
		/// @param activeSet Copy engine resource set to reset.
		void ResetCopyEngineSet(usize activeSet);
		/// @brief Acquire a command buffer for a copy engine resource set.
		/// @param activeSet Copy engine resource set to acquire command buffer for.
		/// @return Acquired command buffer.
		/// @note If not already recording, this functions also starts command buffer recording.
		[[nodiscard]] CommandBuffer& AcquireCmd(usize activeSet);
		/// @brief Resource loader thread.
		/// Flush a copy engine resource set.
		///
		/// 1. Stops command buffer recording.
		/// 2. Submits command buffer to queue.
		/// @param activeSet Active resource set to flush.
		void StreamerFlush(usize activeSet);
		/// @brief Check whether there are tasks pending for the resource loader.
		/// @return True if there are tasks pending, false otherwise.
		[[nodiscard]] constexpr bool AreTasksAvailable() const noexcept;

		/// @brief Determine the resources start state from a buffer description.
		/// @param desc Buffer description.
		/// @return Determined start state of the buffer.
		[[nodiscard]] static constexpr RendererAPI::ResourceState UtilDetermineResourceStartState(const RendererAPI::BufferDesc& desc) noexcept;
		/// @brief Determine the resources start state.
		/// @param uav Is unordered access view.
		/// @return Determined start state of the buffer.
		[[nodiscard]] static constexpr RendererAPI::ResourceState UtilDetermineResourceStartState(bool uav) noexcept;

		/// @brief Generate mip maps for a texture.
		///
		/// Uses vkCmdBlitImage to generate mip maps.
		/// @param texture Texture to generate mip maps for.
		/// @param cmd Command buffer to record the commands on.
		/// @warning This function only works for Vulkan API.
		///          It may be replaced with an API agnostic function in the future.
		static void VulkanGenerateMipMaps(TRAP::Graphics::Texture* texture, CommandBuffer& cmd);

		enum class UploadFunctionResult
		{
			Completed,
			StagingBufferFull,
			InvalidRequest
		};

		/// @brief Update a buffer with the specified resource set.
		/// @param activeSet Resource set to use.
		/// @param bufferUpdateDesc Description of buffer update.
		/// @return Result of upload.
		[[nodiscard]] UploadFunctionResult UpdateBuffer(usize activeSet,
		                                                const RendererAPI::BufferUpdateDesc& bufferUpdateDesc);
		/// @brief Update a texture with the specified resource set.
		/// @param activeSet Resource set to use.
		/// @param textureUpdateDesc Description of texture update.
		/// @param images Images to update.
		/// @return Result of upload.
		[[nodiscard]] UploadFunctionResult UpdateTexture(usize activeSet, const TextureUpdateDescInternal& textureUpdateDesc,
		                                                 const std::array<const TRAP::Image*, 6>* images);
		/// @brief Load a texture with the specified resource set.
		/// @param activeSet Resource set to use.
		/// @param textureUpdate Texture update request.
		/// @return Result of upload.
		[[nodiscard]] UploadFunctionResult LoadTexture(usize activeSet, UpdateRequest& textureUpdate);

		/// @brief Copy a texture to a buffer with the specified resource set.
		/// @param activeSet Resource set to use.
		/// @param textureCopy Texture copy request.
		/// @return Result of copy.
		[[nodiscard]] UploadFunctionResult CopyTexture(usize activeSet, RendererAPI::TextureCopyDesc& textureCopy);

		RendererAPI::ResourceLoaderDesc m_desc;

		TracyLockable(std::mutex, m_queueMutex);
		std::condition_variable_any m_queueCond;
		TracyLockable(std::mutex, m_tokenMutex);
		std::condition_variable_any m_tokenCond;

		enum class UpdateRequestType
		{
			UpdateBuffer,
			UpdateTexture,
			BufferBarrier,
			TextureBarrier,
			LoadTexture,
			CopyTexture,
			Invalid
		};
		struct UpdateRequest
		{
			/// @brief Constructor for buffer update request.
			/// @param buffer Description of buffer update.
			constexpr explicit UpdateRequest(const RendererAPI::BufferUpdateDesc& buffer) noexcept;
			/// @brief Constructor for texture load request.
			/// @param texture Description of texture load.
			constexpr explicit UpdateRequest(const RendererAPI::TextureLoadDesc& texture) noexcept;
			/// @brief Constructor for texture copy request.
			/// @param textureCopy Description of texture copy.
			constexpr explicit UpdateRequest(const RendererAPI::TextureCopyDesc& textureCopy) noexcept;
			/// @brief Constructor for texture update request.
			/// @param texture Description of texture update.
			explicit UpdateRequest(TRAP::Graphics::API::ResourceLoader::TextureUpdateDescInternal texture) noexcept;
			/// @brief Constructor for buffer barrier request.
			/// @param barrier Description of buffer barrier.
			constexpr explicit UpdateRequest(const RendererAPI::BufferBarrier& barrier) noexcept;
			/// @brief Constructor for texture barrier request.
			/// @param barrier Description of texture barrier.
			constexpr explicit UpdateRequest(const RendererAPI::TextureBarrier& barrier) noexcept;

			UpdateRequestType Type = UpdateRequestType::Invalid;
			u64 WaitIndex = 0;
			TRAP::Ref<Buffer> UploadBuffer = nullptr;
			TRAP::Scope<TRAP::Image> Image = nullptr;
			std::variant<RendererAPI::BufferUpdateDesc,
			             TRAP::Graphics::API::ResourceLoader::TextureUpdateDescInternal,
						 RendererAPI::TextureLoadDesc, RendererAPI::TextureCopyDesc,
						 RendererAPI::BufferBarrier, RendererAPI::TextureBarrier> Desc;
		};
		std::vector<UpdateRequest> m_requestQueue{};

		std::atomic<u64> m_tokenCompleted = 0;
		std::atomic<u64> m_tokenSubmitted = 0;
		std::atomic<u64> m_tokenCounter = 0;

		TracySharedLockable(std::shared_mutex, m_semaphoreMutex);

		std::array<SyncToken, TRAP::Graphics::RendererAPI::ImageCount> m_currentTokenState{};

		struct CopyEngine
		{
			TRAP::Ref<TRAP::Graphics::Queue> Queue;
			struct CopyResourceSet
			{
				TRAP::Ref<TRAP::Graphics::Fence> Fence;
				TRAP::Ref<TRAP::Graphics::CommandPool> CommandPool;
				std::reference_wrapper<CommandBuffer> Cmd;
				TRAP::Ref<TRAP::Graphics::Buffer> Buffer;
				u64 AllocatedSpace;

				//Buffers created in case we ran out of space in the original staging buffer
				//Will be cleaned up after the fence for this set is complete
				std::vector<TRAP::Ref<Graphics::Buffer>> TempBuffers;

				TRAP::Ref<TRAP::Graphics::Semaphore> CopyCompletedSemaphore;
			};
			std::vector<CopyResourceSet> ResourceSets{};
			u64 BufferSize{};
			bool IsRecording{};
			TRAP::Ref<TRAP::Graphics::Semaphore> LastCompletedSemaphore;

			//For reading back GPU generated textures, we need to ensure writes have completed before performing the copy.
			std::vector<TRAP::Ref<Semaphore>> WaitSemaphores;
		} m_copyEngine;
		u32 m_nextSet = 0;

		std::jthread m_thread{}; //Stay at the bottom so stop requests can join thread without hitting exceptions or deadlock
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::ResourceState TRAP::Graphics::API::ResourceLoader::UtilDetermineResourceStartState(const RendererAPI::BufferDesc& desc) noexcept
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
		if ((usage & RendererAPI::DescriptorType::RWBuffer) != RendererAPI::DescriptorType::Undefined)
			return RendererAPI::ResourceState::UnorderedAccess;
		if ((((usage & RendererAPI::DescriptorType::VertexBuffer) != RendererAPI::DescriptorType::Undefined) ||
			 ((usage & RendererAPI::DescriptorType::UniformBuffer) != RendererAPI::DescriptorType::Undefined)))
			return RendererAPI::ResourceState::VertexAndConstantBuffer;
		if ((usage & RendererAPI::DescriptorType::IndexBuffer) != RendererAPI::DescriptorType::Undefined)
			return RendererAPI::ResourceState::IndexBuffer;
		if ((usage & RendererAPI::DescriptorType::Buffer) != RendererAPI::DescriptorType::Undefined)
			return RendererAPI::ResourceState::ShaderResource;

		return RendererAPI::ResourceState::Common;
	}

	//Host Cached (Readback Heap)
	return RendererAPI::ResourceState::CopyDestination;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::ResourceState TRAP::Graphics::API::ResourceLoader::UtilDetermineResourceStartState(const bool uav) noexcept
{
	if(uav)
		return RendererAPI::ResourceState::UnorderedAccess;

	return RendererAPI::ResourceState::ShaderResource;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::API::ResourceLoader::UtilGetSurfaceSize(const TRAP::Graphics::API::ImageFormat fmt,
                                                                                         const u32 width, const u32 height,
																                         const u32 depth, const u32 rowStride,
																                         const u32 sliceStride,
																                         const u32 baseMipLevel,
																                         const u32 mipLevels,
																                         const u32 baseArrayLayer,
																                         const u32 arrayLayers) noexcept
{
	u32 requiredSize = 0;

	for(u32 s = baseArrayLayer; s < baseArrayLayer + arrayLayers; ++s)
	{
		u32 w = width;
		u32 h = height;
		u32 d = depth;

		for(u32 m = baseMipLevel; m < (baseMipLevel + mipLevels); ++m)
		{
			u32 rowBytes = 0;
			u32 numRows = 0;

			if(!UtilGetSurfaceInfo(w, h, fmt, nullptr, &rowBytes, &numRows))
				return 0u;

			const u32 temp = ((rowBytes + rowStride - 1) / rowStride) * rowStride;
			requiredSize += (((d * temp * numRows) + sliceStride - 1) / sliceStride) * sliceStride;

			w = w >> 1u;
			h = h >> 1u;
			d = d >> 1u;
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

[[nodiscard]] constexpr bool TRAP::Graphics::API::ResourceLoader::UtilGetSurfaceInfo(const u32 width, const u32 height,
	                                                                                 const TRAP::Graphics::API::ImageFormat fmt,
															                         u32* const outNumBytes, u32* const outRowBytes,
															                         u32* const outNumRows)
{
	u64 numBytes = 0;
	u64 rowBytes = 0;
	u64 numRows = 0;

	const u32 bpp = TRAP::Graphics::API::ImageFormatBitSizeOfBlock(fmt);
	const bool compressed = TRAP::Graphics::API::ImageFormatIsCompressed(fmt);
	const bool planar = TRAP::Graphics::API::ImageFormatIsPlanar(fmt);

	const bool packed = false; //TODO

	if(compressed)
	{
		const u32 blockWidth = TRAP::Graphics::API::ImageFormatWidthOfBlock(fmt);
		const u32 blockHeight = TRAP::Graphics::API::ImageFormatHeightOfBlock(fmt);
		u32 numBlocksWide = 0;
		u32 numBlocksHigh = 0;
		if(width > 0)
			numBlocksWide = Math::Max(1u, (width + (blockWidth - 1)) / blockWidth);
		if(height > 0)
			numBlocksHigh = Math::Max(1u, (height + (blockHeight - 1)) / blockHeight);

		rowBytes = NumericCast<u64>(numBlocksWide) * (bpp >> 3u);
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
		const u32 numOfPlanes = TRAP::Graphics::API::ImageFormatNumOfPlanes(fmt);

		for(u32 i = 0; i < numOfPlanes; ++i)
		{
			numBytes += NumericCast<u64>(TRAP::Graphics::API::ImageFormatPlaneWidth(fmt, i, width)) *
			            TRAP::Graphics::API::ImageFormatPlaneHeight(fmt, i, height) *
						TRAP::Graphics::API::ImageFormatPlaneSizeOfBlock(fmt, i);
		}

		numRows = 1;
		rowBytes = numBytes;
	}
	else
	{
		if(bpp == 0u)
			return false;

		rowBytes = (NumericCast<u64>(width) * bpp + 7u) / 8u; //Round up to nearest byte
		numRows = NumericCast<u64>(height);
		numBytes = rowBytes * height;
	}

	if(numBytes > std::numeric_limits<u32>::max() ||
	   rowBytes > std::numeric_limits<u32>::max() ||
	   numRows > std::numeric_limits<u32>::max())
	{
		return false;
	}

	if(outNumBytes != nullptr)
		*outNumBytes = NumericCast<u32>(numBytes);
	if(outRowBytes != nullptr)
		*outRowBytes = NumericCast<u32>(rowBytes);
	if(outNumRows != nullptr)
		*outNumRows = NumericCast<u32>(numRows);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Graphics::API::ResourceLoader::AreTasksAvailable() const noexcept
{
	return !m_requestQueue.empty();
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(const RendererAPI::BufferUpdateDesc& buffer) noexcept
	: Type(UpdateRequestType::UpdateBuffer), Desc(buffer)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(const RendererAPI::TextureLoadDesc& texture) noexcept
	: Type(UpdateRequestType::LoadTexture), Desc(texture)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(const RendererAPI::TextureCopyDesc& textureCopy) noexcept
	: Type(UpdateRequestType::CopyTexture), Desc(textureCopy)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(const RendererAPI::BufferBarrier& barrier) noexcept
	: Type(UpdateRequestType::BufferBarrier), Desc(barrier)
{}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::API::ResourceLoader::UpdateRequest::UpdateRequest(const RendererAPI::TextureBarrier& barrier) noexcept
	: Type(UpdateRequestType::TextureBarrier), Desc(barrier)
{}

#endif /*TRAP_RESOURCELOADER_H*/