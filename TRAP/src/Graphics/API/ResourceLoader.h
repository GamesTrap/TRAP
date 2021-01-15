#ifndef _TRAP_RESOURCELOADER_H_
#define _TRAP_RESOURCELOADER_H_

#include "RendererAPI.h"

namespace TRAP::Graphics
{
	class CommandPool;
}

namespace TRAP::Graphics::API
{
	typedef uint64_t SyncToken;
	
	class ResourceLoader
	{
	public:
		explicit ResourceLoader(const RendererAPI::ResourceLoaderDesc* desc = nullptr);
		~ResourceLoader();

		//Adding and updating resources can be done using a AddResource or BeginUpdateResource/EndUpdateResource pair.
		//If AddResource(BufferLoadDesc) is called with a data size larger than the ResourceLoader's
		//staging buffer, the ResourceLoader will perform multiple copies/flushes rather than failing the copy.

		//If token is nullptr, the resource will be available when AllResourceLoadsCompleted() returns true.
		//If token is non nullptr, the resource will be available after IsTokenCompleted(token) return true.
		void AddResource(RendererAPI::BufferLoadDesc& desc, SyncToken* token);

		static void BeginUpdateResource(RendererAPI::BufferUpdateDesc& desc);
		void EndUpdateResource(RendererAPI::BufferUpdateDesc& desc, SyncToken* token);

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
		static RendererAPI::ResourceLoaderDesc DefaultResourceLoaderDesc;
		static void StreamerThreadFunc(ResourceLoader* loader);
		static uint32_t UtilGetTextureSubresourceAlignment(RendererAPI::ImageFormat fmt = RendererAPI::ImageFormat::Undefined);
		static RendererAPI::MappedMemoryRange AllocateUploadMemory(uint64_t memoryRequirement, uint32_t alignment);
		void QueueBufferBarrier(const TRAP::Ref<Buffer>& buffer, RendererAPI::ResourceState state, SyncToken* token);
		void QueueBufferUpdate(const RendererAPI::BufferUpdateDesc& desc, SyncToken* token);
		
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

		enum class UploadFunctionResult
		{
			Completed,
			StagingBufferFull,
			InvalidRequest
		};

		enum class MappedRangeFlag
		{
			UnMapBuffer = (1 << 0),
			TempBuffer = (1 << 1)
		};
		
		UploadFunctionResult UpdateBuffer(std::size_t activeSet, const RendererAPI::BufferUpdateDesc& bufferUpdateDesc);
		
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
			//UpdateTexture,
			BufferBarrier,
			//TextureBarrier,
			//LoadTexture,
			Invalid
		};
		struct UpdateRequest
		{
			explicit UpdateRequest(const RendererAPI::BufferUpdateDesc& buffer);
			//UpdateRequest(const RendererAPI::TextureLoadDesc& texture);
			//UpdateRequest(const RendererAPI::TextureUpdateDescInternal& texture);
			explicit UpdateRequest(const RendererAPI::BufferBarrier& barrier);
			//explicit UpdateRequest(const RendererAPI::TextureBarrier& barrier);
			
			UpdateRequestType Type = UpdateRequestType::Invalid;
			uint64_t WaitIndex = 0;
			TRAP::Ref<Buffer> UploadBuffer = nullptr;
			std::variant<RendererAPI::BufferUpdateDesc, RendererAPI::BufferBarrier, RendererAPI::TextureBarrier> Desc; //TODO TextureUpdateDescInternal, TextureLoadDesc
		};
		std::vector<UpdateRequest> m_requestQueue;

		std::atomic<uint64_t> m_tokenCompleted;
		std::atomic<uint64_t> m_tokenCounter;

		std::array<SyncToken, 3> m_currentTokenState;

		struct CopyEngine
		{
			TRAP::Ref<Queue> Queue;
			struct CopyResourceSet
			{
				TRAP::Ref<Fence> Fence;
				CommandBuffer* Cmd;
				TRAP::Ref<CommandPool> CommandPool;
				TRAP::Ref<Buffer> Buffer;
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
		uint32_t m_submittedSets;
	};
}

#endif /*_TRAP_RESOURCELOADER_H_*/