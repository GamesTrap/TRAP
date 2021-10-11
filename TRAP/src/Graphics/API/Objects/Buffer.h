#ifndef _TRAP_BUFFER_H_
#define _TRAP_BUFFER_H_
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Buffer
	{
	public:
		static TRAP::Ref<Buffer> Create(const RendererAPI::BufferDesc& desc);

		virtual ~Buffer();

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		Buffer(const Buffer&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		Buffer& operator=(const Buffer&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		Buffer(Buffer&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		Buffer& operator=(Buffer&&) = default;

		virtual uint64_t GetSize() const = 0;
		virtual RendererAPI::DescriptorType GetDescriptors() const = 0;
		virtual RendererAPI::ResourceMemoryUsage GetMemoryUsage() const = 0;

		virtual void* GetCPUMappedAddress() const = 0;

		virtual void MapBuffer(RendererAPI::ReadRange* range) = 0;
		virtual void UnMapBuffer() = 0;

	protected:
		Buffer();

		//CPU address of the mapped buffer (applicable to buffers created in CPU accessible heaps
		//(CPU, CPUToGPU, GPUToCPU))
		void* m_CPUMappedAddress;

		uint64_t m_size;
		RendererAPI::DescriptorType m_descriptors;
		RendererAPI::ResourceMemoryUsage m_memoryUsage;
	};
}

#endif /*_TRAP_BUFFER_H_*/