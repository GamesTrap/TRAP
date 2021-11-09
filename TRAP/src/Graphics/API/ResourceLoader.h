#ifndef TRAP_RESOURCELOADER_H
#define TRAP_RESOURCELOADER_H

#include "RendererAPI.h"
#include "ImageLoader/Image.h"

namespace TRAP::Graphics
{
	class CommandPool;
	class TextureBase;
}

namespace TRAP::Graphics::API
{
	typedef uint64_t SyncToken;
	class ResourceLoader //TODO Maybe use ThreadPool for more parallelization?!
	{
	public:
		explicit ResourceLoader(const RendererAPI::ResourceLoaderDesc* desc = nullptr);
		~ResourceLoader();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		ResourceLoader(const ResourceLoader&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		ResourceLoader& operator=(const ResourceLoader&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		ResourceLoader(ResourceLoader&&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		ResourceLoader& operator=(ResourceLoader&&) = delete;

		//Adding and updating resources can be done using a AddResource or BeginUpdateResource/EndUpdateResource
		//pair.
		//If AddResource(BufferLoadDesc) is called with a data size larger than the ResourceLoader's
		//staging buffer, the ResourceLoader will perform multiple copies/flushes rather than failing the copy.

		//If token is nullptr, the resource will be available when AllResourceLoadsCompleted() returns true.
		//If token is non nullptr, the resource will be available after IsTokenCompleted(token) return true.
		void AddResource(RendererAPI::BufferLoadDesc& desc, SyncToken* token);
		void AddResource(RendererAPI::TextureLoadDesc& textureDesc, SyncToken* token);

		static void BeginUpdateResource(RendererAPI::BufferUpdateDesc& desc);
		static void BeginUpdateResource(RendererAPI::TextureUpdateDesc& desc);
		void EndUpdateResource(RendererAPI::BufferUpdateDesc& desc, SyncToken* token);
		void EndUpdateResource(RendererAPI::TextureUpdateDesc& desc, SyncToken* token);

		//Returns whether all submitted resource loads and updates have been completed.
		bool AllResourceLoadsCompleted() const;

		//Blocks the calling thread until AllResourceLoadsCompleted() returns true.
		//Note that if more resource loads or updates are submitted from a different thread
		//while the calling thread is blocked, those loads or updates are not guaranteed to have
		//completed when this function returns.
		void WaitForAllResourceLoads();

		//A SyncToken is an array of monotonically increasing integers.
		//GetLastTokenCompleted() returns the last value for which
		//IsTokenCompleted(token) is guaranteed to return true.
		SyncToken GetLastTokenCompleted() const;
		bool IsTokenCompleted(const SyncToken* token) const;
		void WaitForToken(const SyncToken* token);

	private:
		struct TextureUpdateDescInternal
		{
			TRAP::Ref<TRAP::Graphics::TextureBase> Texture = nullptr;
			RendererAPI::MappedMemoryRange Range{};
			uint32_t BaseMipLevel = 0;
			uint32_t MipLevels = 0;
			uint32_t BaseArrayLayer = 0;
			uint32_t LayerCount = 0;
			bool MipsAfterSlice = false;
		};
		struct UpdateRequest;

		static RendererAPI::ResourceLoaderDesc DefaultResourceLoaderDesc;
		static void StreamerThreadFunc(ResourceLoader* loader);
		static uint64_t UtilGetTextureRowAlignment();
		static uint64_t UtilGetTextureSubresourceAlignment(TRAP::Graphics::API::ImageFormat fmt = TRAP::Graphics::API::ImageFormat::Undefined);
		static uint64_t UtilGetSurfaceSize(TRAP::Graphics::API::ImageFormat fmt, uint32_t width, uint32_t height,
		                                   uint32_t depth, uint64_t rowStride, uint64_t sliceStride,
										   uint32_t baseMipLevel, uint32_t mipLevels, uint32_t baseArrayLayer,
										   uint32_t arrayLayers);
		static bool UtilGetSurfaceInfo(uint32_t width, uint32_t height, TRAP::Graphics::API::ImageFormat fmt,
		                               uint64_t* outNumBytes, uint64_t* outRowBytes, uint64_t* outNumRows);
		static RendererAPI::MappedMemoryRange AllocateUploadMemory(uint64_t memoryRequirement, uint32_t alignment);
		void QueueBufferBarrier(const TRAP::Ref<Buffer>& buffer, RendererAPI::ResourceState state, SyncToken* token);
		void QueueBufferUpdate(const RendererAPI::BufferUpdateDesc& desc, SyncToken* token);
		void QueueTextureLoad(const RendererAPI::TextureLoadDesc& desc, SyncToken* token);
		void QueueTextureUpdate(const TextureUpdateDescInternal& textureUpdate, SyncToken* token);
		void QueueTextureBarrier(const TRAP::Ref<TRAP::Graphics::TextureBase>& texture,
		                         RendererAPI::ResourceState state, SyncToken* token);

		RendererAPI::MappedMemoryRange AllocateStagingMemory(uint64_t memoryRequirement, uint32_t alignment);
		void FreeAllUploadMemory();

		void SetupCopyEngine();
		void CleanupCopyEngine();
		bool WaitCopyEngineSet(std::size_t activeSet, bool wait);
		void ResetCopyEngineSet(std::size_t activeSet);
		CommandBuffer* AcquireCmd(std::size_t activeSet);
		void StreamerFlush(std::size_t activeSet);
		bool AreTasksAvailable() const;

		static RendererAPI::ResourceState UtilDetermineResourceStartState(const RendererAPI::BufferDesc& desc);
		static RendererAPI::ResourceState UtilDetermineResourceStartState(bool uav);

		static void VulkanGenerateMipMaps(TRAP::Ref<TRAP::Graphics::TextureBase> texture, CommandBuffer* cmd);

		enum class UploadFunctionResult
		{
			Completed,
			StagingBufferFull,
			InvalidRequest
		};

		enum class MappedRangeFlag
		{
			UnMapBuffer = BIT(0),
			TempBuffer = BIT(1)
		};

		UploadFunctionResult UpdateBuffer(std::size_t activeSet,
		                                  const RendererAPI::BufferUpdateDesc& bufferUpdateDesc);
		UploadFunctionResult UpdateTexture(std::size_t activeSet, const TextureUpdateDescInternal& bufferUpdateDesc,
		                                   std::array<TRAP::Scope<TRAP::Image>, 6>* images);
		UploadFunctionResult LoadTexture(std::size_t activeSet, UpdateRequest& textureUpdate);

		RendererAPI::ResourceLoaderDesc m_desc;

		volatile int32_t m_run;
		std::thread m_thread;

		std::mutex m_queueMutex;
		std::condition_variable m_queueCond;
		std::mutex m_tokenMutex;
		std::condition_variable m_tokenCond;
		enum class UpdateRequestType
		{
			UpdateBuffer,
			UpdateTexture,
			BufferBarrier,
			TextureBarrier,
			LoadTexture,
			Invalid
		};
		struct UpdateRequest
		{
			explicit UpdateRequest(const RendererAPI::BufferUpdateDesc& buffer);
			explicit UpdateRequest(const RendererAPI::TextureLoadDesc& texture);
			explicit UpdateRequest(TRAP::Graphics::API::ResourceLoader::TextureUpdateDescInternal texture);
			explicit UpdateRequest(const RendererAPI::BufferBarrier& barrier);
			explicit UpdateRequest(const RendererAPI::TextureBarrier& barrier);

			UpdateRequestType Type = UpdateRequestType::Invalid;
			uint64_t WaitIndex = 0;
			TRAP::Ref<Buffer> UploadBuffer = nullptr;
			TRAP::Scope<TRAP::Image> Image = nullptr;
			std::variant<RendererAPI::BufferUpdateDesc,
			             TRAP::Graphics::API::ResourceLoader::TextureUpdateDescInternal,
						 RendererAPI::TextureLoadDesc, RendererAPI::BufferBarrier, RendererAPI::TextureBarrier> Desc;
		};
		std::vector<UpdateRequest> m_requestQueue;

		std::atomic<uint64_t> m_tokenCompleted;
		std::atomic<uint64_t> m_tokenCounter;

		std::array<SyncToken, 3> m_currentTokenState;

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
			};
			std::vector<CopyResourceSet> ResourceSets{};
			uint64_t BufferSize{};
			uint32_t BufferCount{};
			bool IsRecording{};
		} m_copyEngine;
		uint32_t m_nextSet;
	};
}

#endif /*TRAP_RESOURCELOADER_H*/