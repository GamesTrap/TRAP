#ifndef TRAP_RESOURCELOADER_H
#define TRAP_RESOURCELOADER_H

#include <atomic>
#include <condition_variable>
#include <thread>

#include "RendererAPI.h"
#include "ImageLoader/Image.h"

namespace TRAP::Graphics
{
	class CommandPool;
	class Texture;
}

namespace TRAP::Graphics::API
{
	using SyncToken = uint64_t;

	/// <summary>
	/// Resource loader.
	/// Does the heavy lifting of sending data between the CPU and GPU.
	/// </summary>
	class ResourceLoader
	{
	public:
		/// <summary>
		/// Instantiate the resource loader.
		/// </summary>
		/// <param name="desc">Optional settings.</param>
		explicit ResourceLoader(const RendererAPI::ResourceLoaderDesc* desc = nullptr);
		/// <summary>
		/// Destructor.
		/// </summary>
		~ResourceLoader();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr ResourceLoader(const ResourceLoader&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr ResourceLoader& operator=(const ResourceLoader&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr ResourceLoader(ResourceLoader&&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr ResourceLoader& operator=(ResourceLoader&&) = delete;

		//Adding and updating resources can be done using a AddResource or BeginUpdateResource/EndUpdateResource
		//pair.

		/// <summary>
		/// Add a resource to GPU.
		/// If called with a data size larger than the ResourceLoader's staging buffer,
		/// the ResourceLoader will perform multiple copies/flushes rather than failing the copy.
		/// </summary>
		/// <param name="desc">Description of buffer resource to add.</param>
		/// <param name="token">
		/// If token is nullptr, the resource will be available when AllResourceLoadsCompleted() returns true.
		/// If token is not nullptr, the resource will be available after IsTokenCompleted(token) returns true.
		/// </param>
		void AddResource(RendererAPI::BufferLoadDesc& desc, SyncToken* token);
		/// <summary>
		/// Add a resource to GPU.
		/// </summary>
		/// <param name="textureDesc">Description of texture resource to add.</param>
		/// <param name="token">
		/// If token is nullptr, the resource will be available when AllResourceLoadsCompleted() returns true.
		/// If token is not nullptr, the resource will be available after IsTokenCompleted(token) returns true.
		/// </param>
		void AddResource(RendererAPI::TextureLoadDesc& textureDesc, SyncToken* token);

		/// <summary>
		/// Start updating a buffer resource.
		/// </summary>
		/// <param name="desc">Description of buffer resource to update.</param>
		static void BeginUpdateResource(RendererAPI::BufferUpdateDesc& desc);
		/// <summary>
		/// Start updating a texture resource.
		/// </summary>
		/// <param name="desc">Description of texture resource to update.</param>
		static void BeginUpdateResource(RendererAPI::TextureUpdateDesc& desc);
		/// <summary>
		/// End updating a buffer resource.
		/// </summary>
		/// <param name="desc">Description of updated buffer resource.</param>
		/// <param name="token">
		/// If token is nullptr, the resource will be available when AllResourceLoadsCompleted() returns true.
		/// If token is not nullptr, the resource will be available after IsTokenCompleted(token) returns true.
		/// </param>
		void EndUpdateResource(RendererAPI::BufferUpdateDesc& desc, SyncToken* token);
		/// <summary>
		/// End updating a texture resource.
		/// </summary>
		/// <param name="desc">Description of updated texture resource.</param>
		/// <param name="token">
		/// If token is nullptr, the resource will be available when AllResourceLoadsCompleted() returns true.
		/// If token is not nullptr, the resource will be available after IsTokenCompleted(token) returns true.
		/// </param>
		void EndUpdateResource(RendererAPI::TextureUpdateDesc& desc, SyncToken* token);

		/// <summary>
		/// Copy data from GPU to the CPU.
		/// For optimal use, the amount of data to transfer should be minimized as much as possible and
		/// applications should provide additional graphics/compute work that the GPU can execute alongside the copy.
		/// </summary>
		/// <param name="textureDesc">Description of the texture copy.</param>
		/// <param name="token">
		/// If token is nullptr, the resource will be available when AllResourceLoadsCompleted() returns true.
		/// If token is not nullptr, the resource will be available after IsTokenCompleted(token) return true.
		/// </param>
		void CopyResource(RendererAPI::TextureCopyDesc& textureDesc, SyncToken* token);

		/// <summary>
		/// Retrieve whether all submitted resource loads and updates have been completed.
		/// </summary>
		/// <returns>True if all loads and updates have finished, false otherwise.</returns>
		[[nodiscard]] bool AllResourceLoadsCompleted() const noexcept;

		//Blocks the calling thread until AllResourceLoadsCompleted() returns true.
		//Note that if more resource loads or updates are submitted from a different thread
		//while the calling thread is blocked, those loads or updates are not guaranteed to have
		//completed when this function returns.

		/// <summary>
		/// Block the calling thread until AllResourceLoadsCompleted() returns true.
		///
		/// Note: If more resource loads or updates are submitted from a different thread
		///       while the calling thread is blocked, those loads or updates are not guaranteed
		///       to have completed when this function returns.
		/// </summary>
		/// <returns>True if all loads and updates have finished, false otherwise.</returns>
		void WaitForAllResourceLoads();

		//A SyncToken is an array of monotonically increasing integers.

		/// <summary>
		/// Retrieve the last sync token which has finished loading or updating.
		/// </summary>
		/// <returns>Last value for which IsTokenCompleted(token) is guaranteed to return true.</returns>
		[[nodiscard]] SyncToken GetLastTokenCompleted() const noexcept;
		/// <summary>
		/// Retrieve whether a sync token has finished loading or updating.
		/// </summary>
		/// <param name="token">Token to check.</param>
		/// <returns>True if the token has finished, false otherwise.</returns>
		[[nodiscard]] bool IsTokenCompleted(const SyncToken* token) const;
		/// <summary>
		/// Block the calling thread until IsTokenCompleted(token) for the provided token returns true.
		/// </summary>
		/// <param name="token">Token to wait for completion.</param>
		void WaitForToken(const SyncToken* token);

		//Allows clients to synchronize with the submission of copy commands (as opposed to their completion).
		//This can reduce the wait time for client but requires using the Semaphore from GetLastSemaphoreCompleted()
		//in a wait operation in a submit that uses the textures just updated.

		/// <summary>
		/// Retrieve the last sync token which has been submitted for copying.
		/// </summary>
		/// <returns>Last value for which IsTokenSubmitted(token) is guaranteed to return true.</returns>
		[[nodiscard]] SyncToken GetLastTokenSubmitted() const noexcept;
		/// <summary>
		/// Retrieve whether a sync token has been submitted for copying.
		/// </summary>
		/// <param name="token">Token to check.</param>
		/// <returns>True if the token has been submitted for copying, false otherwise.</returns>
		[[nodiscard]] bool IsTokenSubmitted(const SyncToken* token) const;
		/// <summary>
		/// Block the calling thread until IsTokenSubmitted(token) for the provided token returns true.
		/// </summary>
		/// <param name="token">Token to wait for submission.</param>
		void WaitForTokenSubmitted(const SyncToken* token);

		/// <summary>
		/// Retrieve the semaphore for the last copy operation that has been submitted.
		/// Note: This can be nullptr if no operations have been executed.
		/// </summary>
		/// <returns>Semaphore for the last copy operation that has been submitted.</returns>
		[[nodiscard]] TRAP::Ref<Semaphore> GetLastSemaphoreCompleted();

	private:
		struct TextureUpdateDescInternal
		{
			TRAP::Graphics::Texture* Texture = nullptr;
			RendererAPI::MappedMemoryRange Range{};
			uint32_t BaseMipLevel = 0;
			uint32_t MipLevels = 0;
			uint32_t BaseArrayLayer = 0;
			uint32_t LayerCount = 0;
			bool MipsAfterSlice = false;
		};
		struct UpdateRequest;

		static RendererAPI::ResourceLoaderDesc DefaultResourceLoaderDesc;
		/// <summary>
		/// Resource loader thread.
		/// Used to async load and update queued resources.
		/// </summary>
		/// <param name="loader">Pointer to the ResourceLoader instance.</param>
		static void StreamerThreadFunc(ResourceLoader* loader);
		/// <summary>
		/// Retrieve the row alignment for textures used by the GPU.
		/// </summary>
		/// <returns>Row alignment used by the GPU.</returns>
		[[nodiscard]] static uint32_t UtilGetTextureRowAlignment() noexcept;
		/// <summary>
		/// Retrieve the subresource alignment for textures used by the GPU.
		/// </summary>
		/// <param name="fmt">Format of the texture. Default: ImageFormat::Undefined.</param>
		/// <returns>Subresource alignment used by the GPU.</returns>
		[[nodiscard]] static uint32_t UtilGetTextureSubresourceAlignment(TRAP::Graphics::API::ImageFormat fmt = TRAP::Graphics::API::ImageFormat::Undefined) noexcept;
		/// <summary>
		///	Retrieve the total surface size of a texture.
		/// </summary>
		/// <param name="fmt">Image format of the texture.</param>
		/// <param name="width">Width of the texture.</param>
		/// <param name="height">Height of the texture.</param>
		/// <param name="depth">Depth of the texture.</param>
		/// <param name="rowStride">Row stride of the texture.</param>
		/// <param name="sliceStride">Slice stride of the texture.</param>
		/// <param name="baseMipLevel">Base mip level of the texture.</param>
		/// <param name="mipLevels">Number of mip levels of the texture.</param>
		/// <param name="baseArrayLayer">Base array layer of the texture.</param>
		/// <param name="arrayLayers">Number of array layers of the texture.</param>
		/// <returns>Total surface size of the texture.</returns>
		[[nodiscard]] static constexpr uint32_t UtilGetSurfaceSize(TRAP::Graphics::API::ImageFormat fmt, uint32_t width, uint32_t height,
		                                                           uint32_t depth, uint32_t rowStride, uint32_t sliceStride,
										                           uint32_t baseMipLevel, uint32_t mipLevels, uint32_t baseArrayLayer,
										                           uint32_t arrayLayers) noexcept;
		/// <summary>
		/// Retrieve information about a texture.
		/// </summary>
		/// <param name="width">Width of the texture.</param>
		/// <param name="height">Height of the texture.</param>
		/// <param name="fmt">Image format of the texture.</param>
		/// <param name="outNumBytes">Optional output for the number of bytes in the texture.</param>
		/// <param name="outRowBytes">Optional output for the number of bytes in a row of the texture.</param>
		/// <param name="outNumRows">Optional output for the number of rows in the texture.</param>
		/// <returns>True on successful retrieval, false otherwise.</returns>
		[[nodiscard]] static constexpr bool UtilGetSurfaceInfo(uint32_t width, uint32_t height, TRAP::Graphics::API::ImageFormat fmt,
		                                                       uint32_t* outNumBytes, uint32_t* outRowBytes, uint32_t* outNumRows);
		/// <summary>
		/// Allocate a new staging buffer.
		/// </summary>
		/// <param name="memoryRequirement">Required memory size.</param>
		/// <param name="alignment">Memory alignment.</param>
		/// <returns>Staging buffer information.</returns>
		[[nodiscard]] static RendererAPI::MappedMemoryRange AllocateUploadMemory(uint64_t memoryRequirement, uint32_t alignment);
		/// <summary>
		/// Queue a buffer barrier.
		/// </summary>
		/// <param name="buffer">Buffer to queue a barrier on.</param>
		/// <param name="state">State to transition the buffer to.</param>
		/// <param name="token">Optional output sync token.</param>
		void QueueBufferBarrier(const TRAP::Ref<Buffer>& buffer, RendererAPI::ResourceState state, SyncToken* token);
		/// <summary>
		/// Queue a buffer update.
		/// </summary>
		/// <param name="desc">Description of buffer update.</param>
		/// <param name="token">Optional output sync token.</param>
		void QueueBufferUpdate(const RendererAPI::BufferUpdateDesc& desc, SyncToken* token);
		/// <summary>
		/// Queue a texture load.
		/// </summary>
		/// <param name="desc">Description of texture load.</param>
		/// <param name="token">Optional output sync token.</param>
		void QueueTextureLoad(const RendererAPI::TextureLoadDesc& desc, SyncToken* token);
		/// <summary>
		/// Queue a texture update.
		/// </summary>
		/// <param name="desc">Description of texture update.</param>
		/// <param name="token">Optional output sync token.</param>
		void QueueTextureUpdate(const TextureUpdateDescInternal& textureUpdate, SyncToken* token);
		/// <summary>
		/// Queue a texture copy.
		/// </summary>
		/// <param name="desc">Description of texture copy.</param>
		/// <param name="token">Optional output sync token.</param>
		void QueueTextureCopy(const RendererAPI::TextureCopyDesc& desc, SyncToken* token);
		/// <summary>
		/// Queue a texture barrier.
		/// </summary>
		/// <param name="texture">Texture to queue a barrier on.</param>
		/// <param name="state">State to transition the texture to.</param>
		/// <param name="token">Optional output sync token.</param>
		void QueueTextureBarrier(TRAP::Graphics::Texture* texture,
		                         RendererAPI::ResourceState state, SyncToken* token);

		/// <summary>
		/// Retrieve memory from pre-allocated staging buffer or create a
		/// temporary buffer if the streamer ran out of memory.
		/// </summary>
		/// <param name="memoryRequirement">Required memory size.</param>
		/// <param name="alignment">Memory alignment.</param>
		/// <returns>Memory from pre-allocated staging buffer or temporary buffer.</returns>
		[[nodiscard]] RendererAPI::MappedMemoryRange AllocateStagingMemory(uint64_t memoryRequirement, uint32_t alignment);
		/// <summary>
		/// Free all upload memory and temporary buffers.
		/// </summary>
		void FreeAllUploadMemory() noexcept;

		/// <summary>
		/// Initialize the copy engine.
		/// </summary>
		void SetupCopyEngine();
		/// <summary>
		/// Cleanup the copy engine.
		/// </summary>
		void CleanupCopyEngine();
		/// <summary>
		/// Wait for a copy engine resource set to complete its work.
		/// </summary>
		/// <param name="activeSet">Copy engine resource set to wait for.</param>
		void WaitCopyEngineSet(std::size_t activeSet) const;
		/// <summary>
		/// Reset a copy engine resource set.
		/// </summary>
		/// <param name="activeSet">Copy engine resource set to reset.</param>
		void ResetCopyEngineSet(std::size_t activeSet);
		/// <summary>
		/// Acquire a command buffer for a copy engine resource set.
		/// Note: If not already recording, this functions also starts command buffer recording.
		/// </summary>
		/// <param name="activeSet">Copy engine resource set to acquire command buffer for.</param>
		/// <returns>Acquired command buffer.</returns>
		[[nodiscard]] CommandBuffer* AcquireCmd(std::size_t activeSet);
		/// <summary>
		/// Resource loader thread.
		/// Flush a copy engine resource set.
		///
		/// 1. Stops command buffer recording.
		/// 2. Submits command buffer to queue.
		/// </summary>
		/// <param name="loader">Copy engine resource set to flush.</param>
		void StreamerFlush(std::size_t activeSet);
		/// <summary>
		/// Check whether there are tasks pending for the resource loader.
		/// </summary>
		/// <returns>True if there are tasks pending, false otherwise.</returns>
		[[nodiscard]] constexpr bool AreTasksAvailable() const noexcept;

		/// <summary>
		/// Determine the resources start state from a buffer description.
		/// </summary>
		/// <param name="desc">Buffer description.</param>
		/// <returns>Determined start state of the buffer.</returns>
		[[nodiscard]] static constexpr RendererAPI::ResourceState UtilDetermineResourceStartState(const RendererAPI::BufferDesc& desc) noexcept;
		/// <summary>
		/// Determine the resources start state.
		/// </summary>
		/// <param name="uav">Is unordered access view.</param>
		/// <returns>Determined start state of the buffer.</returns>
		[[nodiscard]] static constexpr RendererAPI::ResourceState UtilDetermineResourceStartState(bool uav) noexcept;

		/// <summary>
		/// Generate mip maps for a texture.
		///
		/// Uses vkCmdBlitImage to generate mip maps.
		/// Note: This function only works for Vulkan API.
		///       This function will be replaced in future with an API agnostic way.
		/// </summary>
		/// <param name="texture">Texture to generate mip maps for.</param>
		/// <param name="cmd">Command buffer to record the commands on.</param>
		static void VulkanGenerateMipMaps(TRAP::Graphics::Texture* texture, CommandBuffer* cmd);

		enum class UploadFunctionResult
		{
			Completed,
			StagingBufferFull,
			InvalidRequest
		};

		enum class MappedRangeFlag : uint32_t
		{
			UnMapBuffer = BIT(0u),
			TempBuffer = BIT(1u)
		};

		/// <summary>
		/// Update a buffer with the specified resource set.
		/// </summary>
		/// <param name="activeSet">Resource set to use.</param>
		/// <param name="bufferUpdateDesc">Description of buffer update.</param>
		/// <return>Result of upload.</return>
		[[nodiscard]] UploadFunctionResult UpdateBuffer(std::size_t activeSet,
		                                                const RendererAPI::BufferUpdateDesc& bufferUpdateDesc);
		/// <summary>
		/// Update a texture with the specified resource set.
		/// </summary>
		/// <param name="activeSet">Resource set to use.</param>
		/// <param name="textureUpdateDesc">Description of texture update.</param>
		/// <param name="images">Images to update.</param>
		/// <return>Result of upload.</return>
		[[nodiscard]] UploadFunctionResult UpdateTexture(std::size_t activeSet, const TextureUpdateDescInternal& textureUpdateDesc,
		                                                 const std::array<const TRAP::Image*, 6>* images);
		/// <summary>
		/// Load a texture with the specified resource set.
		/// </summary>
		/// <param name="activeSet">Resource set to use.</param>
		/// <param name="textureUpdate">Texture update request.</param>
		/// <return>Result of upload.</return>
		[[nodiscard]] UploadFunctionResult LoadTexture(std::size_t activeSet, UpdateRequest& textureUpdate);

		/// <summary>
		/// Copy a texture to a buffer with the specified resource set.
		/// </summary>
		/// <param name="activeSet">Resource set to use.</param>
		/// <param name="textureCopy">Texture copy request.</param>
		/// <returns>Result of copy.</returns>
		[[nodiscard]] UploadFunctionResult CopyTexture(std::size_t activeSet, RendererAPI::TextureCopyDesc& textureCopy);

		RendererAPI::ResourceLoaderDesc m_desc;

		std::atomic<bool> m_run = true;
		std::jthread m_thread{};

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
			/// <summary>
			/// Constructor for buffer update request.
			/// </summary>
			/// <param name="buffer">Description of buffer update.</param>
			constexpr explicit UpdateRequest(const RendererAPI::BufferUpdateDesc& buffer) noexcept;
			/// <summary>
			/// Constructor for texture load request.
			/// </summary>
			/// <param name="texture">Description of texture load.</param>
			constexpr explicit UpdateRequest(const RendererAPI::TextureLoadDesc& texture) noexcept;
			/// <summary>
			/// Constructor for texture copy request.
			/// </summary>
			/// <param name="texture">Description of texture copy.</param>
			constexpr explicit UpdateRequest(const RendererAPI::TextureCopyDesc& textureCopy) noexcept;
			/// <summary>
			/// Constructor for texture update request.
			/// </summary>
			/// <param name="texture">Description of texture update.</param>
			explicit UpdateRequest(TRAP::Graphics::API::ResourceLoader::TextureUpdateDescInternal texture) noexcept;
			/// <summary>
			/// Constructor for buffer barrier request.
			/// </summary>
			/// <param name="buffer">Description of buffer barrier.</param>
			constexpr explicit UpdateRequest(const RendererAPI::BufferBarrier& barrier) noexcept;
			/// <summary>
			/// Constructor for texture barrier request.
			/// </summary>
			/// <param name="texture">Description of texture barrier.</param>
			constexpr explicit UpdateRequest(const RendererAPI::TextureBarrier& barrier) noexcept;

			UpdateRequestType Type = UpdateRequestType::Invalid;
			uint64_t WaitIndex = 0;
			TRAP::Ref<Buffer> UploadBuffer = nullptr;
			TRAP::Scope<TRAP::Image> Image = nullptr;
			std::variant<RendererAPI::BufferUpdateDesc,
			             TRAP::Graphics::API::ResourceLoader::TextureUpdateDescInternal,
						 RendererAPI::TextureLoadDesc, RendererAPI::TextureCopyDesc,
						 RendererAPI::BufferBarrier, RendererAPI::TextureBarrier> Desc;
		};
		std::vector<UpdateRequest> m_requestQueue{};

		std::atomic<uint64_t> m_tokenCompleted = 0;
		std::atomic<uint64_t> m_tokenSubmitted = 0;
		std::atomic<uint64_t> m_tokenCounter = 0;

		TracyLockable(std::mutex, m_semaphoreMutex);

		std::array<SyncToken, TRAP::Graphics::RendererAPI::ImageCount> m_currentTokenState{};

		struct CopyEngine
		{
			TRAP::Ref<TRAP::Graphics::Queue> Queue;
			struct CopyResourceSet
			{
				TRAP::Ref<TRAP::Graphics::Fence> Fence;
				CommandBuffer* Cmd;
				TRAP::Ref<TRAP::Graphics::CommandPool> CommandPool;
				TRAP::Ref<TRAP::Graphics::Buffer> Buffer;
				uint64_t AllocatedSpace;

				//Buffers created in case we ran out of space in the original staging buffer
				//Will be cleaned up after the fence for this set is complete
				std::vector<TRAP::Ref<Graphics::Buffer>> TempBuffers;

				TRAP::Ref<TRAP::Graphics::Semaphore> CopyCompletedSemaphore;
			};
			std::vector<CopyResourceSet> ResourceSets{};
			uint64_t BufferSize{};
			uint32_t BufferCount{};
			bool IsRecording{};
			TRAP::Ref<TRAP::Graphics::Semaphore> LastCompletedSemaphore;

			//For reading back GPU generated textures, we need to ensure writes have completed before performing the copy.
			std::vector<TRAP::Ref<Semaphore>> WaitSemaphores;
		} m_copyEngine;
		uint32_t m_nextSet = 0;
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

[[nodiscard]] constexpr uint32_t TRAP::Graphics::API::ResourceLoader::UtilGetSurfaceSize(const TRAP::Graphics::API::ImageFormat fmt,
                                                                                         const uint32_t width, const uint32_t height,
																                         const uint32_t depth, const uint32_t rowStride,
																                         const uint32_t sliceStride,
																                         const uint32_t baseMipLevel,
																                         const uint32_t mipLevels,
																                         const uint32_t baseArrayLayer,
																                         const uint32_t arrayLayers) noexcept
{
	uint32_t requiredSize = 0;

	for(uint32_t s = baseArrayLayer; s < baseArrayLayer + arrayLayers; ++s)
	{
		uint32_t w = width;
		uint32_t h = height;
		uint32_t d = depth;

		for(uint32_t m = baseMipLevel; m < (baseMipLevel + mipLevels); ++m)
		{
			uint32_t rowBytes = 0;
			uint32_t numRows = 0;

			if(!UtilGetSurfaceInfo(w, h, fmt, nullptr, &rowBytes, &numRows))
				return 0u;

			const uint32_t temp = ((rowBytes + rowStride - 1) / rowStride) * rowStride;
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

[[nodiscard]] constexpr bool TRAP::Graphics::API::ResourceLoader::UtilGetSurfaceInfo(const uint32_t width, const uint32_t height,
	                                                                                 const TRAP::Graphics::API::ImageFormat fmt,
															                         uint32_t* const outNumBytes, uint32_t* const outRowBytes,
															                         uint32_t* const outNumRows)
{
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

		rowBytes = NumericCast<uint64_t>(numBlocksWide) * (bpp >> 3u);
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
		{
			numBytes += NumericCast<uint64_t>(TRAP::Graphics::API::ImageFormatPlaneWidth(fmt, i, width)) *
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

		rowBytes = (NumericCast<uint64_t>(width) * bpp + 7u) / 8u; //Round up to nearest byte
		numRows = NumericCast<uint64_t>(height);
		numBytes = rowBytes * height;
	}

	if(numBytes > std::numeric_limits<uint32_t>::max() ||
	   rowBytes > std::numeric_limits<uint32_t>::max() ||
	   numRows > std::numeric_limits<uint32_t>::max())
	{
		return false;
	}

	if(outNumBytes != nullptr)
		*outNumBytes = NumericCast<uint32_t>(numBytes);
	if(outRowBytes != nullptr)
		*outRowBytes = NumericCast<uint32_t>(rowBytes);
	if(outNumRows != nullptr)
		*outNumRows = NumericCast<uint32_t>(numRows);

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