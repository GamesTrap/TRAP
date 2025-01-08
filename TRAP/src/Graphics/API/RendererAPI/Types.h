#ifndef TRAP_RENDERERAPI_TYPES_H
#define TRAP_RENDERERAPI_TYPES_H

#include <filesystem>
#include <variant>

#include "Core/Types.h"
#include "Graphics/API/ImageFormat.h"
#include "Utils/Bit.h"
#include "Utils/Enum.h"
#include "Utils/Map.h"
#include "Utils/Optional.h"
#include "Utils/SmartPtr.h"
#include "Utils/Utils.h"
#include "Utils/String/ConvertToType.h"

//-------------------------------------------------------------------------------------------------------------------//
//Forward declarations-----------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

struct VkSamplerYcbcrConversionInfo;

namespace TRAP
{
    class Image;
    class Window;

    namespace Graphics
    {
        class Texture;
        class Shader;
        class Sampler;
        class RootSignature;
        class Queue;
        class Fence;
        class Semaphore;
        class CommandBuffer;
        class RenderTarget;
        class PipelineCache;
        class SwapChain;
        class Buffer;
        class Pipeline;
        class DescriptorSet;

        namespace API::ShaderReflection
        {
	        enum class TextureDimension : u32;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//
//Enums--------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

namespace TRAP::Graphics
{
	/// @brief Supported render APIs.
    enum class RenderAPI : u8
    {
        NONE = 0u,
        Vulkan
    };

    /// @brief Enum bit flag for the different wave operations.
    enum class WaveOpsSupportFlags : u32
    {
        None = 0u,
        Basic = BIT(0u),
        Vote = BIT(1u),
        Arithmetic = BIT(2u),
        Ballot = BIT(3u),
        Shuffle = BIT(4u),
        ShuffleRelative = BIT(5u),
        Clustered = BIT(6u),
        Quad = BIT(7u),
        PartitionedNV = BIT(8u)
    };

    /// @brief Enum describing the different types of queues.
    enum class QueueType : u8
    {
        Graphics = 0u,
        Transfer,
        Compute
    };

    /// @brief Enum describing queue flags.
    enum class QueueFlag : u8
    {
        None = 0u,
        DisableGPUTimeout = BIT(0u), //Disable the GPU timeout for this command queue (DirectX 12)
    };

    /// @brief Enum describing the different priorities for a queue (DirectX 12).
    enum class QueuePriority : u8
    {
        Normal,
        High,
        GlobalRealtime
    };

    /// @brief Enum describing the status of a fence.
    enum class FenceStatus : u8
    {
        Complete = 0u,
        Incomplete,
        NotSubmitted
    };

    /// @brief Enum describing the different types of load actions.
    enum class LoadActionType : u8
    {
        DontCare,
        Load,
        Clear
    };

    /// @brief Enum describing the different types of store actions.
    enum class StoreActionType : u8
    {
        //Store is the most common use case so keep that as default
        Store,
        DontCare,
        None
    };

    /// @brief Enum describing the different types of cube textures.
    enum class TextureCubeType : u8
    {
        Cross,
        //TODO
        //Equirectangular
    };

    /// @brief Enum describing the different sample counts.
    enum class SampleCount : u8
    {
        One = BIT(0u),
        Two = BIT(1u),
        Four = BIT(2u),
        Eight = BIT(3u),
        Sixteen = BIT(4u)
    };

    /// @brief Enum describing the different anti aliasing methods.
    enum class AntiAliasing : u8
    {
        Off,
        MSAA
    };

    /// @brief Enum describing the different GPU vendors.
    enum class GPUVendor : i32
    {
        Unknown     = -1,
        AMD         = 0x1002,
        ImgTec      = 0x1010,
        NVIDIA      = 0x10DE,
        ARM         = 0x13B5,
        Broadcom    = 0x14E4,
        Qualcomm    = 0x5143,
        Intel       = 0x8086,
        Apple       = 0x106B,
        Vivante     = 0x7A05,
        VeriSilicon = 0x1EB1,
        Kazan       = 0x10003,
        Codeplay    = 0x10004,
        Mesa        = 0x10005
    };

    /// @brief Enum bit flags used by texture creation.
    enum class TextureCreationFlags : u32
    {
        //Default flag (Texture will use default allocation strategy decided by the API specific allocator)
        None = 0u,
        //Texture will allocate its own memory (COMMITTED resource)
        OwnMemory = BIT(0u),
        //Use on-tile memory to store this texture
        OnTile = BIT(5u),
        //Force 2D instead of automatically determining dimension based on width, height, depth
        Force2D = BIT(7u),
        //Force 3D instead of automatically determining dimension based on width, height, depth
        Force3D = BIT(8u),
        //Display target //TODO Implement
        AllowDisplayTarget = BIT(9u),
        //Create an sRGB texture //TODO Implement
        SRGB = BIT(10u),
        //Create a normal map texture //TODO Implement
        NormalMap = BIT(11u),
        //Fast clear //TODO Implement
        FastClear = BIT(12u),
        //Fragment mask //TODO Implement
        FragMask = BIT(13u),
        //Create a storage texture
        Storage = BIT(14u)
    };

    /// @brief Enum describing the state of a resource.
    enum class ResourceState : u32
    {
        Undefined = 0u,
        VertexAndConstantBuffer = BIT(0u),
        IndexBuffer = BIT(1u),
        RenderTarget = BIT(2u),
        UnorderedAccess = BIT(3u),
        DepthWrite = BIT(4u),
        DepthRead = BIT(5u),
        NonPixelShaderResource = BIT(6u),
        PixelShaderResource = BIT(7u),
        ShaderResource = BIT(6u) | BIT(7u),
        StreamOut = BIT(8u),
        IndirectArgument = BIT(9u),
        CopyDestination = BIT(10u),
        CopySource = BIT(11u),
        GenericRead = (((((BIT(0u) | BIT(1u)) | BIT(6u)) | BIT(7u)) | BIT(9u)) | BIT(11u)),
        Present = BIT(12u),
        Common = BIT(13u),
        RayTracingAccelerationStructure = BIT(14u),
        ShadingRateSource = BIT(15u)
    };

    /// @brief Enum describing the type of a descriptor.
    enum class DescriptorType : u32
    {
        Undefined = 0u,
        Sampler = BIT(0u),
        //SRV read only Texture
        Texture = BIT(1u),
        //UAV Texture
        RWTexture = BIT(2u),
        //SRV read only Buffer
        Buffer = BIT(3u),
        BufferRaw = (Buffer | (Buffer << 1u)),
        //UAV Buffer
        RWBuffer = BIT(5u),
        RWBufferRaw = (RWBuffer | (RWBuffer << 1u)),
        //Uniform buffer
        UniformBuffer = BIT(7u),
        UniformBufferDynamic = BIT(8u),
        //Push constant / Root constant
        RootConstant = BIT(9u),
        //IA
        VertexBuffer = BIT(10u),
        IndexBuffer = BIT(11u),
        IndirectBuffer = BIT(12u),
        //Cubemap SRV
        TextureCube = (Texture | (IndirectBuffer << 1u)),
        //RTV / DSV per mip slice
        RenderTargetMipSlices = BIT(14u),
        //RTV / DSV per array slice
        RenderTargetArraySlices = BIT(15u),
        //RTV / DSV per depth sice
        RenderTargetDepthSlices = BIT(16u),
        RayTracing = BIT(17u),

        //Vulkan
        //Subpass input (descriptor type only available in Vulkan)
        InputAttachment = BIT(18u),
        TexelBuffer = BIT(19u),
        RWTexelBuffer = BIT(20u),
        CombinedImageSampler = BIT(21u),

        //Khronos ray tracing extension
        AccelerationStructure = BIT(22u),
        AccelerationStructureBuildInput = BIT(23u),
        ShaderDeviceAddress  = BIT(24u),
        ShaderBindingTable = BIT(25u)
    };

    /// @brief Enum describing the different memory usage types for a resource.
    enum class ResourceMemoryUsage : u8
    {
        //No intended memory usage specified
        Unknown = 0u,
        //Memory will be used on device only, no need to be mapped on host.
        GPUOnly = 1u,
        //Memory will be mapped on host.
        //Could be used for transfer to device.
        CPUOnly = 2u,
        //Memory will be used for frequent (dynamic) updates from host and reads on device.
        CPUToGPU = 3u,
        //Memory will be used for writing on device and readback on host.
        GPUToCPU = 4u
    };

    /// @brief Enum describing flags for the buffer creation.
    enum class BufferCreationFlags : u32
    {
        //Default flag (Buffer will use aliased memory, buffer will not be CPU accessible until MapBuffer
        //is called)
        None = 0u,
        //Buffer will allocate its own memory (COMMITTED resource)
        OwnMemory = BIT(0u),
        //Buffer will be persistently mapped
        PersistentMap = BIT(1u),
        //Use ESRAM to store this buffer
        ESRAM = BIT(2u),
        //Flag to specify not to allocate descriptors for the resource
        NoDescriptorViewCreation = BIT(3u),

        //Vulkan
        HostVisible = 0x100u,
        HostCoherent = 0x200u
    };

    /// @brief Enum describing the different types of buffers for clearing.
    enum class ClearBufferType : u8
    {
        Color = BIT(0u),
        Depth = BIT(1u),
        Stencil = BIT(2u),
        Color_Depth = Color | Depth,
        Color_Stencil = Color | Stencil,
        Color_Depth_Stencil = Color | Stencil | Depth,
        Depth_Stencil = Depth | Stencil
    };

#ifndef TRAP_HEADLESS_MODE
    /// @brief Enum describing the different latency modes for NVIDIA Reflex.
    /// @remark @headless This function is not available in headless mode.
    enum class NVIDIAReflexLatencyMode : u8
    {
        Disabled,
        Enabled,
        EnabledBoost
    };

    enum class NVIDIAReflexLatencyMarker : u8
    {
        SimulationStart = 0u,
        SimulationEnd = 1u,
        RenderSubmitStart = 2u,
        RenderSubmitEnd = 3u,
        PresentStart = 4u,
        PresentEnd = 5u,
        InputSample = 6u,
        TriggerFlash = 7u,
        OutOfBandRenderSubmitStart = 8u,
        OutOfBandRenderSubmitEnd = 9u,
        OutOfBandPresentStart = 10u,
        OutOfBandPresentEnd = 11u
    };

    enum class AMDAntiLagMode : u8
    {
        DriverControl = 0u, //AMD Anti Lag will be enabled/disabled based on driver settings
        Enabled = 1u,
        Disabled = 2u
    };

    enum class AMDAntiLagMarker : u8
    {
        InputStage, //Specifies the stage before processing input
        PresentStage //Specifies the stage before calling vkQueuePresentKHR().
    };
#endif /*TRAP_HEADLESS_MODE*/

    /// @brief Enum describing the type of an indirect argument.
    enum class IndirectArgumentType : u8
    {
        IndirectDraw,
        IndirectDrawIndex,
        IndirectDispatch,
        IndirectVertexBuffer,
        IndirectIndexBuffer,
        IndirectConstant,
        IndirectDescriptorTable,
        IndirectPipeline
    };

    /// @brief Enum describing the update frequency of a descriptor.
    /// @note: The same update frequency order should be used by shaders.
    enum class DescriptorUpdateFrequency : u8
    {
        Static = 0u,
        Dynamic
    };

    /// @brief Enum describing the different types of filtering used by samplers.
    enum class FilterType : u8
    {
        Nearest = 0u,
        Linear
    };

    /// @brief Enum describing the different modes of mip mapping used by samplers.
    enum class MipMapMode : u8
    {
        Nearest = 0u,
        Linear
    };

    /// @brief Enum describing the different address modes used by samplers.
    enum class AddressMode : u8
    {
        Mirror,
        Repeat,
        ClampToEdge,
        ClampToBorder
    };

    /// @brief Enum describing the different compare modes used by depth/stencil testing and samplers.
    enum class CompareMode : u8
    {
        Never,
        Less,
        Equal,
        LessOrEqual,
        Greater,
        NotEqual,
        GreaterOrEqual,
        Always
    };

    /// @brief Enum describing the different shader stages making up a shader.
    enum class ShaderStage : u32
    {
        None = 0u,
        Vertex = BIT(0u),
        TessellationControl = BIT(1u),
        TessellationEvaluation = BIT(2u),
        Geometry = BIT(3u),
        Fragment = BIT(4u),
        Compute = BIT(5u),
        RayTracing = BIT(6u),

        AllGraphics = (Vertex | TessellationControl | TessellationEvaluation | Geometry | Fragment),

        //DirectX 12 name aliases
        Hull = TessellationControl,
        Domain = TessellationEvaluation,
        Pixel = Fragment
    };

    /// @brief Enum describing the overall type of a shader.
    enum class ShaderType : u8
    {
        Graphics,
        Compute,
        // RayTracing //TODO
    };

    /// @brief Enum describing the different flags for root signature creation.
    enum class RootSignatureFlags : u32
    {
        //Default flag
        None = 0u,
    };

    /// @brief Enum describing the different pipeline types.
    enum class PipelineType : u32
    {
        Compute,
        Graphics,
        RayTracing
    };

    /// @brief Enum describing the different data types used by index buffers.
    enum class IndexType : u8
    {
        UInt32 = 0u,
        UInt16
    };

    /// @brief Enum describing the different blend constants used for blending.
    enum class BlendConstant : u8
    {
        Zero = 0u,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        SrcAlphaSaturate,
        BlendFactor,
        OneMinusBlendFactor
    };

    /// @brief Enum describing the different blend modes/functions used for blending.
    enum class BlendMode : u8
    {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    /// @brief Enum describing which render target to affect with the blend state.
    enum class BlendStateTargets : u8
    {
        BlendStateTarget0 = BIT(0u),
        BlendStateTarget1 = BIT(1u),
        BlendStateTarget2 = BIT(2u),
        BlendStateTarget3 = BIT(3u),
        BlendStateTarget4 = BIT(4u),
        BlendStateTarget5 = BIT(5u),
        BlendStateTarget6 = BIT(6u),
        BlendStateTarget7 = BIT(7u),

        BlendStateTargetAll = 0xFFu,
    };

    /// @brief Enum describing the different stencil/depth operations.
    enum class StencilOp : u8
    {
        Keep,
        SetZero,
        Replace,
        Invert,
        Increment,
        Decrement,
        IncrementSaturation,
        DecrementSaturation
    };

    /// @brief Enum describing the different cull modes.
    enum class CullMode : u8
    {
        None = 0u,
        Back,
        Front,
        Both
    };

    /// @brief Enum describing the different front face winding orders.
    enum class FrontFace : u32
    {
        CounterClockwise = 0u,
        Clockwise
    };

    /// @brief Enum describing the different fill modes.
    enum class FillMode : u8
    {
        Solid,
        Line,
        Point
    };

    /// @brief Enum describing flags for pipeline cache creation.
    enum class PipelineCacheFlags : u8
    {
        None = 0u,
        ExternallySynchronized = BIT(0u)
    };

    /// @brief Enum describing the different primitive topologies.
    enum class PrimitiveTopology : u8
    {
        PointList = 0u,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        PatchList
    };

    /// @brief Enum describing the different vertex attribute rates.
    enum class VertexAttributeRate : u8
    {
        Vertex = 0u,
        Instance = 1u
    };

    /// @brief Enum describing the different types of queries.
    enum class QueryType : u8
    {
        Timestamp = 0u,
        PipelineStatistics,
        Occlusion
    };

    /// @brief Enum describing the different presentation statuses.
    enum class PresentStatus : u8
    {
        Success = 0u,
        DeviceReset = 1u,
        Failed = 2u,
        OutOfDate = 3u
    };

    /// @brief Enum describing the different sampler ranges used by YUV conversion samplers.
    enum class SamplerRange : u8
    {
        Full = 0u,
        Narrow = 1u
    };

    /// @brief Enum describing the different sampler models used by YUV conversion samplers.
    enum class SamplerModelConversion : u8
    {
        RGBIdentity = 0u,
        YCBCRIdentity = 1u,
        YCBCR709 = 2u,
        YCBCR601 = 3u,
        YCBCR2020 = 4u
    };

    /// @brief Enum describing the different sample locations used by YUV conversion samplers.
    enum class SampleLocation : u8
    {
        Cosited = 0u,
        Midpoint = 1u
    };

    /// @brief Enum describing the shading rates used by fragment/pixel shaders.
    enum class ShadingRate : u8
    {
        NotSupported = 0u,
        Full = BIT(0u),
        Half = BIT(1u),
        Quarter = BIT(2u),
        Eighth = BIT(3u),
        OneXTwo = BIT(4u),
        TwoXOne = BIT(5u),
        TwoXFour = BIT(6u),
        FourXTwo = BIT(7u)
    };

    /// @brief Enum describing how to combine different shading rates.
    enum class ShadingRateCombiner : u8
    {
        Passthrough = BIT(0u),
        Override = BIT(1u),
        Min = BIT(2u),
        Max = BIT(3u),
        Sum = BIT(4u)
    };

    /// @brief Enum describing the shading rate capabilities supported by the GPU.
    enum class ShadingRateCaps : u8
    {
        NotSupported = 0u,
        PerDraw = BIT(0u),
        PerTile = BIT(1u),

        //Vulkan only
        PerPrimitive = BIT(2u)
    };

    /// @brief Enum describing flags that change the behaviour of a command pool.
    enum class CommandPoolCreateFlags : u8
    {
        None = 0u,
        Transient = BIT(0u), //Indicate that the allocated command buffers will be short lived
        ResetCommandBuffer = BIT(1u) //Allows to reset command buffers individually to initial state.
    };

    enum class MappedRangeFlags : u8
    {
        None = 0u,
        UnMapBuffer = BIT(0u),
        TempBuffer = BIT(1u)
    };

    MAKE_ENUM_FLAG(WaveOpsSupportFlags);
    MAKE_ENUM_FLAG(QueueFlag);
    MAKE_ENUM_FLAG(TextureCreationFlags);
    MAKE_ENUM_FLAG(ResourceState);
    MAKE_ENUM_FLAG(DescriptorType);
    MAKE_ENUM_FLAG(BufferCreationFlags);
    MAKE_ENUM_FLAG(ClearBufferType);
    MAKE_ENUM_FLAG(ShaderStage);
    MAKE_ENUM_FLAG(BlendStateTargets);
    MAKE_ENUM_FLAG(PipelineCacheFlags);
    MAKE_ENUM_FLAG(ShadingRate);
    MAKE_ENUM_FLAG(ShadingRateCombiner);
    MAKE_ENUM_FLAG(ShadingRateCaps);
    MAKE_ENUM_FLAG(CommandPoolCreateFlags);
    MAKE_ENUM_FLAG(MappedRangeFlags);
}

//-------------------------------------------------------------------------------------------------------------------//
//Structs------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

namespace TRAP::Graphics
{
    struct Color
    {
        //Doubles are used because they are large enough to precisely
        //hold 32-bit signed/unsigned integers and single-precision floats.

        f64 Red;
        f64 Green;
        f64 Blue;
        f64 Alpha;
    };

    struct DepthStencil
    {
        f32 Depth;
        u32 Stencil;
    };

    using ClearValue = std::variant<Color, DepthStencil>;

    /// @brief Description of a subresource.
    /// Used to update a existing resource.
    struct SubresourceDataDesc
    {
        u64 SrcOffset;
        u32 MipLevel;
        u32 ArrayLayer;

        u32 RowPitch;
        u32 SlicePitch;
    };

    /// @brief Description of a render target.
    struct RenderTargetDesc
    {
        //Texture creation flags (decides memory allocation strategy, sharing access, ...)
        TextureCreationFlags Flags{};
        //Width
        u32 Width{};
        //Height
        u32 Height{};
        //Depth (should be 1 if type is not TextureType::TextureCube)
        u32 Depth{};
        //Texture array size (should be 1 if texture is not a texture array or cubemap)
        u32 ArraySize{};
        //Number of mip levels
        u32 MipLevels{};
        //Multisample anti-aliasing (MSAA)
        TRAP::Graphics::SampleCount SampleCount = SampleCount::One;
        //Internal image format
        TRAP::Graphics::API::ImageFormat Format{};
        //What state will the texture get created in
        ResourceState StartState{};
        //Optimized clear value (recommended to use the same value when clearing the renderTarget)
        TRAP::Graphics::ClearValue ClearValue{};
        //The image quality level.
        //The higher the quality, the lower the performance.
        //The valid range is between 0 and the value appropriate for SampleCount
        u32 SampleQuality{};
        //Descriptor creation
        DescriptorType Descriptors{};
        //Debug name used in GPU profile
        std::string Name{};

        void* NativeHandle{};
    };

    /// @brief Description of a texture.
    struct TextureDesc
    {
        //Texture creation flags (decides memory allocation strategy, sharing access, ...)
        TextureCreationFlags Flags{};
        //Width
        u32 Width = 1;
        //Height;
        u32 Height = 1;
        //Depth (should be 1 if Type is not TextureType::TextureCube)
        u32 Depth = 1;
        //Texture array size (should be 1 if texture is not a texture array or cubemap)
        u32 ArraySize = 1;
        //Number of mip levels
        u32 MipLevels = 1;
        //Number of multisamples per pixel (currently Textures created with Usage TextureUsage::SampledImage
        //only support One).
        TRAP::Graphics::SampleCount SampleCount = SampleCount::One;
        //The image quality level.
        //The higher the quality, the lower the performance.
        //The valid range is between 0 and the value appropriate for SampleCount.
        u32 SampleQuality{};
        //Image format
        TRAP::Graphics::API::ImageFormat Format{};
        //Optimized clear value (recommended to use the same value when clearing the renderTarget)
        TRAP::Graphics::ClearValue ClearValue{};
        //What state will the texture get created in
        ResourceState StartState{};
        //Descriptor creation
        DescriptorType Descriptors{};
        //Pointer to native texture handle if the texture does not own underlying resource
        void* NativeHandle{};
        //Debug name used in GPU profile
        std::string Name{};

        //Optional Vulkan YCbCr conversion info
        ::VkSamplerYcbcrConversionInfo* VkSamplerYcbcrConversionInfo{};
    };

    /// @brief Description for a texture load.
    struct TextureLoadDesc
    {
        //Target to load texture info into.
        TRAP::Graphics::Texture* Texture{};
        //Load empty texture
        TextureDesc* Desc{};
        //Filepath with extension
        std::vector<std::filesystem::path> Filepaths;
        //Loaded images (used to load in memory images, if set Filepaths are ignored)
        std::vector<const Image*> Images{};
        //Following is ignored if Desc != nullptr.
        //Desc->Flags will be considered instead.
        TextureCreationFlags CreationFlag = TextureCreationFlags::None;
        //Is Texture cubemap?
        bool IsCubemap{};
        //Type of cubemap texture (NullOpt + IsCubemap == true means Multi-file cube texture)
        TRAP::Optional<TextureCubeType> Type = TRAP::NullOpt;
    };

    /// @brief Description of a buffer.
    struct BufferDesc
    {
        //Size of the buffer (in bytes)
        u64 Size{};
        //Alignment
        u32 Alignment{};
        //Decides which heap buffer will be used (default, upload, readback)
        ResourceMemoryUsage MemoryUsage{};
        //Creation flags of the buffer
        BufferCreationFlags Flags{};
        //What type of queue the buffer is owned by
        TRAP::Graphics::QueueType QueueType{};
        //What state will the buffer get created in
        ResourceState StartState{};
        //Number of elements in the buffer
        u64 ElementCount{};
        //Size of each element (in bytes) in the buffer
        u64 StructStride{};
        //ICB draw type
        IndirectArgumentType ICBDrawType{};
        //ICB max vertex buffers slots count
        u32 ICBMaxVertexBufferBind{};
        //ICB max fragment buffers slots count
        u32 ICBMaxFragmentBufferBind{};
        //Set this to specify a counter buffer for this buffer
        //Buffer* CounterBuffer;
        //Format of the buffer
        TRAP::Graphics::API::ImageFormat Format{};
        //Flags specifying the suitable usage of this buffer (Uniform Buffer, Vertex Buffer, Index Buffer, ...)
        DescriptorType Descriptors{};
        //Debug name used in GPU-profile
        std::string Name{};
    };

    /// @brief Description of a sampler.
    struct SamplerDesc
    {
        //Minification filter
        FilterType MinFilter{};
        //Magnification filter
        FilterType MagFilter{};
        //Mip mapping mode.
        TRAP::Graphics::MipMapMode MipMapMode{};
        //Address mode for U coordinate
        AddressMode AddressU{};
        //Address mode for V coordinate
        AddressMode AddressV{};
        //Address mode for W coordinate
        AddressMode AddressW{};
        //Mip lod bias (offset from the calculated mip map level)
        f32 MipLodBias{};
        //Whether to use the given lod range or not
        bool SetLodRange{};
        //Minimum lod value
        f32 MinLod{};
        //Maximum lod value
        f32 MaxLod{};
        //Enable/Disable Anisotropic filtering
        bool EnableAnisotropy = true;
        //Override Anisotropic filtering level (0.0f = auto)
        f32 OverrideAnisotropyLevel{};
        //Comparison function compares sampled data against existing sampled data
        CompareMode CompareFunc{};
        //Debug name used in GPU-profile
        std::string Name{};

        /// @brief Description of YCbCr(YUV) conversion sampler.
        struct SamplerConversionDesc
        {
            //YCbCr(YUV) image format
            TRAP::Graphics::API::ImageFormat Format;
            //Conversion model
            SamplerModelConversion Model;
            //Range
            SamplerRange Range;
            //X offset for chroma
            SampleLocation ChromaOffsetX;
            //Y offset for chroma
            SampleLocation ChromaOffsetY;
            //Chroma filter
            FilterType ChromaFilter;
            //Explicitly force the reconstruction
            bool ForceExplicitReconstruction;

            [[nodiscard]] constexpr bool operator==(const SamplerConversionDesc& other) const noexcept = default;
            [[nodiscard]] constexpr bool operator!=(const SamplerConversionDesc& other) const noexcept = default;
        } SamplerConversionDesc{};

        [[nodiscard]] constexpr bool operator==(const SamplerDesc& other) const noexcept = default;
        [[nodiscard]] constexpr bool operator!=(const SamplerDesc& other) const noexcept = default;
    };

    /// @brief Description of a binary shader stage.
    struct BinaryShaderStageDesc
    {
        //Shader data
        std::vector<u32> ByteCode{};
    };

    /// @brief Description of a binary shader.
    struct BinaryShaderDesc
    {
        //Shader stages contained in the binary shader
        ShaderStage Stages{};
        //Vertex shader stage data
        BinaryShaderStageDesc Vertex{};
        //Fragment shader stage data
        BinaryShaderStageDesc Fragment{};
        //Geometry shader stage data
        BinaryShaderStageDesc Geometry{};
        //TessellationContol shader stage data
        BinaryShaderStageDesc TessellationControl{};
        //TessellationEvaluation shader stage data
        BinaryShaderStageDesc TessellationEvaluation{};
        //Compute shader stage data
        BinaryShaderStageDesc Compute{};
    };

    /// @brief Description of a root signature.
    struct RootSignatureDesc
    {
        //Shaders to manage
        std::vector<Shader*> Shaders{};
        //Max number of bindless texture (DirectX 12)
        u32 MaxBindlessTextures{};
        //Static sampler names
        std::vector<const char*> StaticSamplerNames{};
        //Static samplers
        std::vector<TRAP::Ref<Sampler>> StaticSamplers{};
        //Root signature creation flags
        RootSignatureFlags Flags{};
    };

    /// @brief Struct containing information about a descriptor.
    struct DescriptorInfo
    {
        //Name of descriptor
        std::string Name{};
        //Type of descriptor
        DescriptorType Type{};
        //Dimension of texture
        API::ShaderReflection::TextureDimension Dimension{};
        //Is root descriptor?
        bool RootDescriptor{};
        //Descriptor set
        u32 Set{};
        //Size of descriptor
        u32 Size{};
        //Index in the descriptor set
        u32 IndexInParent{};
        //Index of the handle in the root signature
        u32 HandleIndex{};

        //Resource binding location
        u32 Reg{};
        //Index into the root descriptor.
        u32 RootDescriptorIndex{};
        //Shader stages using the descriptor (Vulkan)
        u32 VkStages{};
    };

    /// @brief Description of a descriptor set.
    struct DescriptorSetDesc
    {
        //Root signature to use
        TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
        //Descriptor set index
        u32 Set;
        //Max amount of sets to allocate
        u32 MaxSets{};
    };

    /// @brief Description of a command pool.
    struct CommandPoolDesc
    {
        //Queue to be used by the command pool
        TRAP::Ref<TRAP::Graphics::Queue> Queue;
        CommandPoolCreateFlags CreateFlags = CommandPoolCreateFlags::None;
        //Debug name used in GPU-profile
        std::string Name{};
    };

    /// @brief Description of a queue.
    struct QueueDesc
    {
        //Type of queue
        QueueType Type{};
        //Queue creation flags
        QueueFlag Flag{};
        //Priority for the queue (DirectX 12)
        QueuePriority Priority{};
        //Debug name used in GPU-profile
        std::string Name{};
    };

    /// @brief Read range used for buffer mapping.
    struct ReadRange
    {
        //Offset from the start of the buffer
        u64 Offset{};
        //Range to read (DirectX 12)
        u64 Range{};
    };

    /// @brief Description for a queue submission.
    struct QueueSubmitDesc
    {
        //Command buffers to submit
        std::vector<std::reference_wrapper<CommandBuffer>> Cmds{};
        //Fence to signal
        TRAP::Ref<Fence> SignalFence{};
        //Semaphores to wait on
        std::vector<TRAP::Ref<Semaphore>> WaitSemaphores{};
        //Semaphores to signal
        std::vector<TRAP::Ref<Semaphore>> SignalSemaphores{};

        //NVIDIA-Reflex Present ID (Optional)
        TRAP::Optional<u64> ReflexPresentID = TRAP::NullOpt;
    };

    /// @brief Description of a blend state.
    struct BlendStateDesc
    {
        //Source blend factor per render target.
        std::array<BlendConstant, 8u> SrcFactors{};
        //Destination blend factor per render target.
        std::array<BlendConstant, 8u> DstFactors{};
        //Source alpha blend factor per render target.
        std::array<BlendConstant, 8u> SrcAlphaFactors{};
        //Destination alpha blend factor per render target.
        std::array<BlendConstant, 8u> DstAlphaFactors{};
        //Blend mode per render target.
        std::array<BlendMode, 8u> BlendModes{};
        //Alpha blend mode per render target.
        std::array<BlendMode, 8u> BlendAlphaModes{};
        //Write mask per render target.
        std::array<i32, 8u> Masks{};
        //Mask that identifies the render targets affected by the blend state.
        BlendStateTargets RenderTargetMask{};
        //Set whether each render target has an unique blend function.
        //When false the blend function in slot 0 will be used for all render targets.
        bool IndependentBlend{};
    };

    /// @brief Description of a depth state.
    struct DepthStateDesc
    {
        //Enable depth testing?
        bool DepthTest{};
        //Enable depth writing?
        bool DepthWrite{};
        //Depth comparison function
        CompareMode DepthFunc{};
        //Enable stencil testing?
        bool StencilTest{};
        //Stencil read mask
        u8 StencilReadMask{};
        //Stencil write mask
        u8 StencilWriteMask{};
        //Function to use on the front for stencil testing
        CompareMode StencilFrontFunc{};
        //Stencil action to perform when stencil testing fails
        StencilOp StencilFrontFail{};
        //Depth action to perform when depth testing fails
        StencilOp DepthFrontFail{};
        //Stencil action to perform when passed
        StencilOp StencilFrontPass{};
        //Function to use on the back for stencil testing
        CompareMode StencilBackFunc{};
        //Stencil action to perform when stencil testing fails
        StencilOp StencilBackFail{};
        //Depth action to perform when depth testing fails
        StencilOp DepthBackFail{};
        //Stencil action to perform when passed
        StencilOp StencilBackPass{};
    };

    /// @brief Description of a rasterizer state.
    struct RasterizerStateDesc
    {
        //Cull mode
        TRAP::Graphics::CullMode CullMode{};
        //Depth bias
        i32 DepthBias{};
        //Depth bias slope factor.
        f32 SlopeScaledDepthBias{};
        //Fill mode
        TRAP::Graphics::FillMode FillMode{};
        //Front face winding order
        TRAP::Graphics::FrontFace FrontFace{};
        //Clamp depth to min/max depth
        bool DepthClampEnable{};
    };

    /// @brief Description of a pipeline cache.
    struct PipelineCacheDesc
    {
        //Pipeline data
        std::vector<u8> Data{};
        //Pipeline cache creation flags
        PipelineCacheFlags Flags{};
        //Debug name used in GPU profile
        std::string Name{};
    };

    /// @brief Description of a pipeline cache to load.
    struct PipelineCacheLoadDesc
    {
        //Path to pipeline cache
        std::filesystem::path Path{};
        //Pipeline cache creation flags
        PipelineCacheFlags Flags{};
        //Debug name used in GPU profile
        std::string Name{};
    };

    /// @brief Description of a compute pipeline.
    struct ComputePipelineDesc
    {
        //Shader to use
        Shader* ShaderProgram{};
        //Root signature to use
        TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
    };

    /// @brief Description of a single vertex attribute.
    struct VertexAttribute
    {
        //Attribute format
        TRAP::Graphics::API::ImageFormat Format{};
        //Attribute binding
        u32 Binding{};
        //Attribute location
        u32 Location{};
        //Attribute offset
        u32 Offset{};
        //Attribute rate
        VertexAttributeRate Rate{};
    };

    /// @brief Description of a vertex layout.
    struct VertexLayout
    {
        //Amount of attributes in the layout
        u32 AttributeCount{};
        //Attributes
        std::array<VertexAttribute, 15> Attributes{};
    };

    /// @brief Description of a graphics pipeline.
    struct GraphicsPipelineDesc
    {
        //Shader to use
        Shader* ShaderProgram{};
        //Root signature to use
        TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
        //Vertex layou to use
        TRAP::Ref<TRAP::Graphics::VertexLayout> VertexLayout{};
        //Blend state to use
        TRAP::Ref<BlendStateDesc> BlendState{};
        //Depth state to use
        TRAP::Ref<DepthStateDesc> DepthState{};
        //Rasterizer state to use
        TRAP::Ref<RasterizerStateDesc> RasterizerState{};
        //Color formats for the render targets
        std::vector<TRAP::Graphics::API::ImageFormat> ColorFormats{};
        //How many render targets to use
        u32 RenderTargetCount{};
        //Sample count to use by render targets
        TRAP::Graphics::SampleCount SampleCount = SampleCount::One;
        //Sample quality to use by render targets (DirectX 12)
        u32 SampleQuality{};
        //Depth/Stencil image format to use
        TRAP::Graphics::API::ImageFormat DepthStencilFormat{};
        //Primitive topology to use
        TRAP::Graphics::PrimitiveTopology PrimitiveTopology{};
        //Shading rate to use (only if supported)
        TRAP::Graphics::ShadingRate ShadingRate{};
        //Shading rate combiners to use (only if supported)
        std::array<ShadingRateCombiner, 2u> ShadingRateCombiners{};
        //Shading rate texture to use (only if ShadingRateCaps::PerTile is supported, disables fixed ShadingRate)
        TRAP::Ref<RenderTarget> ShadingRateTexture{};
    };

    /// @brief Description of a RayTracing pipeline.
    struct RayTracingPipelineDesc
    {
        //TODO
    };

    /// @brief Description of a pipeline.
    struct PipelineDesc
    {
        //Type of pipeline
        PipelineType Type{};
        //Pipeline
        std::variant<ComputePipelineDesc,
                     GraphicsPipelineDesc,
                     RayTracingPipelineDesc> Pipeline{GraphicsPipelineDesc()};
        //Pipeline cache
        TRAP::Ref<PipelineCache> Cache{};
        //Pipeline extensions (DirectX 12)
        void* PipelineExtensions{};
        //Pipeline extensions count (DirectX 12)
        u32 PipelineExtensionCount{};

        //Name for the pipeline
        std::string Name{};
    };

    /// @brief Description of a query pool.
    struct QueryPoolDesc
    {
        //Type of query to hold
        QueryType Type{};
        //Max amount of queries to manage
        u32 QueryCount{};
        //Debug name used in GPU profile
        std::string Name{};
    };

    /// @brief Description of a query.
    struct QueryDesc
    {
        //Index of the query into the query pool
        u32 Index{};
    };

    /// @brief Struct holding indirect draw arguments.
    struct IndirectDrawArguments
    {
        //How many vertices to draw
        u32 VertexCount{};
        //How many instances to draw
        u32 InstanceCount{};
        //Vertex offset to start at
        u32 StartVertex{};
        //Instance offset to start at
        u32 StartInstance{};
    };

    /// @brief Struct holding indirect indexed draw arguments.
    struct IndirectDrawIndexArguments
    {
        //How many indices to draw
        u32 IndexCount{};
        //How many instances to draw
        u32 InstanceCount{};
        //Index buffer offset to start at
        u32 StartIndex{};
        //Vertex offset to start at
        u32 VertexOffset{};
        //Instance offset to start at
        u32 StartInstance{};
    };

    /// @brief Struct holding indirect dispatch arguments.
    struct IndirectDispatchArguments
    {
        //X work group size
        u32 GroupCountX{};
        //Y work group size
        u32 GroupCountY{};
        //Z work group size
        u32 GroupCountZ{};
    };

    /// @brief Struct holding indirect descriptor argument.
    struct IndirectArgumentDescriptor
    {
        //Type of indirect argument
        IndirectArgumentType Type{};
        //Name of descriptor
        std::string Name{};
        //Index of descriptor
        u32 Index{};
    };

    /// @brief Description of a command signature.
    struct CommandSignatureDesc
    {
        //Root signature.
        TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
        //Indirect argument descriptions
        std::vector<IndirectArgumentDescriptor> ArgDescs{};
        //Set to true if indirect argument struct should not be aligned to 16 bytes
        bool Packed{};
    };

    /// @brief Description of a swapchain.
    struct SwapChainDesc
    {
        //Window handle
        TRAP::Window* Window{};
        //Queues which should be allowed to present
        std::vector<TRAP::Ref<Queue>> PresentQueues{};
        //Number of back buffers in this swapchain
        u32 ImageCount{};
        //Width of the swapchain
        u32 Width{};
        //Height of the swapchain
        u32 Height{};
        //Color format of the swapchain
        TRAP::Graphics::API::ImageFormat ColorFormat{};
        //Clear value
        TRAP::Graphics::ClearValue ClearValue{};
        //Set whether swapchain will be presented using VSync
        bool EnableVSync{};
        //Anti aliasing sample count (1 = no AA)
        TRAP::Graphics::SampleCount SampleCount = SampleCount::One;

        //Old swapchain, may aid in resource reusage (Vulkan)
        SwapChain* OldSwapChain = nullptr;
    };

    /// @brief Struct holding a render target barrier.
    struct RenderTargetBarrier
    {
        //Render target
        std::reference_wrapper<const TRAP::Graphics::RenderTarget> RenderTarget;
        //Current resource state of the render target
        ResourceState CurrentState{};
        //Target resource state of the render target
        ResourceState NewState{};
        //Starts a barrier transition in a new state, putting a resource in a temporary no-access condition (DirectX 12)
        bool BeginOnly{};
        //This barrier completes a transition, setting a new state and restoring active access to a resource (DirectX 12)
        bool EndOnly{};
        //Acquire ownershop of the queue family
        bool Acquire{};
        //Release ownership of the queue family
        bool Release{};
        //Type of queue to acquire or release
        TRAP::Graphics::QueueType QueueType{};
        //Specify whether following barrier targets particular subresource
        bool SubresourceBarrier{};

        //Following values are ignored if SubresourceBarrier is false

        //Mip level
        u8 MipLevel{};
        //Array layer
        u16 ArrayLayer{};
    };

    /// @brief Struct holding a buffer barrier.
    struct BufferBarrier
    {
        //Buffer
        std::reference_wrapper<const TRAP::Graphics::Buffer> Buffer;
        //Current resource state of the buffer
        ResourceState CurrentState{};
        //Target resource state of the buffer
        ResourceState NewState{};
        //Starts a barrier transition in a new state, putting a resource in a temporary no-access condition (DirectX 12)
        bool BeginOnly{};
        //This barrier completes a transition, setting a new state and restoring active access to a resource (DirectX 12)
        bool EndOnly{};
        //Acquire ownership of the queue family
        bool Acquire{};
        //Release ownership of the queue family
        bool Release{};
        //Type of queue to acquire or release
        TRAP::Graphics::QueueType QueueType{};
    };

    /// @brief Struct holding a texture barrier.
    struct TextureBarrier
    {
        //Texture
        std::reference_wrapper<const TRAP::Graphics::Texture> Texture;
        //Current resource state of the texture
        ResourceState CurrentState{};
        //Target resource state of the texture
        ResourceState NewState{};
        //Starts a barrier transition in a new state, putting a resource in a temporary no-access condition (DirectX 12)
        bool BeginOnly{};
        //This barrier completes a transition, setting a new state and restoring active access to a resource (DirectX 12)
        bool EndOnly{};
        //Acquire ownership of the queue family
        bool Acquire{};
        //Release ownership of the queue family
        bool Release{};
        //Type of queue to acquire or release
        TRAP::Graphics::QueueType QueueType{};
        //Specify whether the following barrier targets particular subresource
        bool SubresourceBarrier{};

        //Following values are ignored if SubresourceBarrier is false

        //Mip level
        u8 MipLevel{};
        //Array layer
        u16 ArrayLayer{};
    };

    using DescriptorResource = std::variant<std::vector<const Texture*>,
                                            std::vector<const Sampler*>,
                                            std::vector<const Buffer*>,
                                            std::vector<Pipeline*>,
                                            std::vector<DescriptorSet*>>;

    /// @brief Struct holding a data of a descriptor.
    struct DescriptorData
    {
        //User can either set name of descriptor or index (index in RootSignature->Descriptors array)
        //Name of descriptor
        std::string Name{};
        /// @brief Range(s) to bind (buffer, offset, size)
        struct BufferOffset
        {
            //Offset to bind the buffer descriptor
            std::vector<u64> Offsets{};
            //Size of the buffer to bind
            std::vector<u64> Sizes{};
        };

        /// @brief Descriptor set buffer extraction options
        struct DescriptorSetExtraction
        {
            //Index of the descriptor set to extract
            u32 DescriptorSetBufferIndex{};
            Ref<Shader> DescriptorSetShader{};
            ShaderStage DescriptorSetShaderStage{};
        };

        struct TextureSlice
        {
            //When binding UAV, control the mip slice to bind for UAV (example - generating mipmaps in a compute shader)
            u32 UAVMipSlice{};
            //Binds entire mip chain as array of UAV
            bool BindMipChain{};
        };
        std::variant<BufferOffset, DescriptorSetExtraction, TextureSlice, bool> Offset{false};
        //Array of resources containing descriptor handles or constant to be used in ring buffer memory
        //DescriptorRange can hold only one resource type array
        //Can be one of the following:
        //Array of texture descriptors (SRV and UAV textures)
        //Array of sampler descriptors
        //Array of buffer descriptors (SRV, UAV and CBV buffers)
        //Array of pipeline descriptors
        //DescriptorSet buffer extraction
        //Custom binding (RayTracing acceleration structure ...)
        DescriptorResource Resource{std::vector<const Texture*>()}; //TODO RayTracing acceleration structure

        //Number of resources in the descriptor(applies to array of textures, buffers, ...)
        u32 Count{};
        //Index into RootSignature->Descriptors array
        u32 Index = std::numeric_limits<u32>::max();
    };

    /// @brief Description of a queue presentation.
    struct QueuePresentDesc
    {
        //Swapchain to preesent
        TRAP::Ref<TRAP::Graphics::SwapChain> SwapChain{};
        //Semaphores to wait on before presenting
        std::vector<TRAP::Ref<Semaphore>> WaitSemaphores{};
        //Render target of the swapchain to present
        u32 Index{};
    };

    /// @brief Description of actions to perform on load.
    struct LoadActionsDesc
    {
        //Action to perform on the color attachment(s) on load.
        std::vector<LoadActionType> LoadActionsColor{};
        //Action to perform on the depth attachment on load.
        LoadActionType LoadActionDepth{};
        //Action to perform on the stencil attachment on load.
        LoadActionType LoadActionStencil{};
        //Clear color(s)
        std::vector<Color> ClearColorValues{};
        //Clear depth/stencil value
        DepthStencil ClearDepthStencil{};
        //Action to perform on the color attachment(s) on store.
        std::vector<StoreActionType> StoreActionsColor{};
        //Action to perform on the depth attachment on store.
        StoreActionType StoreActionDepth{};
        //Action to perform on the stencil attachment on store.
        StoreActionType StoreActionStencil{};
    };

    /// @brief Map resolving a name to its descriptor index in a root signature.
    using DescriptorIndexMap = TRAP::Utils::UnorderedStringMap<u32>;

    /// @brief Struct holding data about a mapped memory range.
    struct MappedMemoryRange
    {
        //Pointer to mapped buffer data with offset already applied to
        std::span<u8> Data{};
        //Mapped buffer
        TRAP::Ref<TRAP::Graphics::Buffer> Buffer = nullptr;
        //Offset in the buffer to start at
        u64 Offset = 0; //TODO This may be unnecessary
        //Mapped size
        u64 Size = 0; //TODO This may be unnecessary
        MappedRangeFlags Flags = MappedRangeFlags::None;
    };

    /// @brief Description for a texture update.
    /// @note Only use for procedural textures which are created on CPU (noise textures, font texture, ...)
    struct TextureUpdateDesc
    {
        //Texture to update
        TRAP::Graphics::Texture* Texture = nullptr;
        //Mip level to update
        u32 MipLevel = 0;
        //Array layer to update
        u32 ArrayLayer = 0;

        //To be filled by the caller
        //Example:
        //TRAP::Graphics::TextureUpdateDesc update = {Texture};
        //TRAP::Graphics::RendererAPI::GetResourceLoader()->BeginUpdateResource(update);
        //Row by row copy is required if DstRowStride > SrcRowStride.
        //Single memcpy will work if DstRowStride == SrcRowStride.
        //2D:
        //for (u32 r = 0; r < update.RowCount; ++r)
        //    memcpy(update.MappedData + r * update.DstRowStride, srcPixels + r * update.SrcRowStride, update.SrcRowStride);
        //3D:
        //for (u32 z = 0; z < depth; ++z)
        //{
        //    u8* dstData = update.MappedData + update.DstSliceStride * z;
        //    u8* srcData = srcPixels + update.SrcSliceStride * z;
        //    for (u32 r = 0; r < update.RowCount; ++r)
        //        memcpy(dstData + r * update.DstRowStride, srcData + r * update.SrcRowStride, update.SrcRowStride);
        //}
        //TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(update, &token);
        std::span<u8> MappedData{};
        //Size of each row in destination including padding - Needs to be respected
        //otherwise texture data will be corrupted if dst row stride is not the same as src row stride
        u32 DstRowStride{};
        //Number of rows in this slice of the texture
        u32 RowCount{};
        //Src row stride for convenience (RowCount * width * texture format size)
        u32 SrcRowStride{};
        //Size of each slice in destination including padding - Use for offsetting dst data updating 3D textures
        u32 DstSliceStride{};
        //Size of each slice in src - Use for offsetting src data when updating 3D textures
        u32 SrcSliceStride{};

        //Internal
        struct
        {
            MappedMemoryRange MappedRange;
        } Internal;
    };

    struct TextureCopyDesc
    {
        //Source texture to copy from
        TRAP::Ref<TRAP::Graphics::Texture> Texture;
        //Destination buffer to copy to
        TRAP::Ref<TRAP::Graphics::Buffer> Buffer;
        //Semaphore to synchronize graphics/compute operations that write
        //to the textures with the texture -> buffer copy.
        TRAP::Ref<Semaphore> WaitSemaphore;
        //Mip level to copy from
        u32 MipLevel;
        //Array layer to copy from
        u32 ArrayLayer;
        //Current texture state
        ResourceState TextureState;
        //Queue the texture is copied from
        TRAP::Graphics::QueueType QueueType;
        //Offset into the destination buffer to start at
        u64 BufferOffset;
    };

    /// @brief Description for the resource loader.
    struct ResourceLoaderDesc
    {
        //Size for each staging buffer
        u64 BufferSize;
        //Amount of staging buffers to create
        u32 BufferCount;
    };

    /// @brief Description for a buffer load.
    struct BufferLoadDesc
    {
        //Output buffer with uploaded data.
        TRAP::Ref<TRAP::Graphics::Buffer> Buffer = nullptr;
        //Data to upload
        const void* Data = nullptr;
        //Description for the buffer
        BufferDesc Desc{};

        //Force Reset Buffer to 0
        bool ForceReset = false;
    };

    /// @brief Description for a buffer update.
    struct BufferUpdateDesc
    {
        //Buffer to update
        TRAP::Ref<TRAP::Graphics::Buffer> Buffer;
        //Offset into buffer to update data at
        u64 DstOffset;
        //Size of data to update
        u64 Size;

        //To be filled by the caller
        std::span<u8> MappedData;

        //Internal
        struct
        {
            MappedMemoryRange MappedRange;
        } Internal;
    };

    static constexpr u32 ImageCount = 3u; //Triple Buffered
    /// @brief The maximum amount of descriptor sets that can be used.
    static constexpr u32 MaxDescriptorSets = 4;
}

//-------------------------------------------------------------------------------------------------------------------//
//fmt specializatons-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::RenderAPI>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::RenderAPI renderAPI,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(renderAPI)
        {
        case TRAP::Graphics::RenderAPI::Vulkan:
            enumStr = "Vulkan";
            break;
        case TRAP::Graphics::RenderAPI::NONE:
            enumStr = "NONE";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::AntiAliasing>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::AntiAliasing antiAliasing,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(antiAliasing)
        {
        case TRAP::Graphics::AntiAliasing::Off:
            enumStr = "Off";
            break;
        case TRAP::Graphics::AntiAliasing::MSAA:
            enumStr = "MSAA";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::SampleCount>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::SampleCount sampleCount,
	                                            fmt::format_context& ctx)
    {
		u32 samples = 1;

        switch(sampleCount)
        {
        case TRAP::Graphics::SampleCount::One:
			[[fallthrough]];
        case TRAP::Graphics::SampleCount::Two:
			[[fallthrough]];
        case TRAP::Graphics::SampleCount::Four:
			[[fallthrough]];
        case TRAP::Graphics::SampleCount::Eight:
			[[fallthrough]];
        case TRAP::Graphics::SampleCount::Sixteen:
			samples = std::to_underlying(sampleCount);
			break;
        }

		return fmt::format_to(ctx.out(), "{}", samples);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::GPUVendor>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::GPUVendor gpuVendor,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(gpuVendor)
        {
        case TRAP::Graphics::GPUVendor::Unknown:
            enumStr = "Unknown";
            break;
		case TRAP::Graphics::GPUVendor::AMD:
            enumStr = "AMD";
            break;
		case TRAP::Graphics::GPUVendor::ImgTec:
            enumStr = "ImgTec";
            break;
		case TRAP::Graphics::GPUVendor::NVIDIA:
            enumStr = "NVIDIA";
            break;
		case TRAP::Graphics::GPUVendor::ARM:
            enumStr = "ARM";
            break;
		case TRAP::Graphics::GPUVendor::Broadcom:
            enumStr = "Broadcom";
            break;
		case TRAP::Graphics::GPUVendor::Qualcomm:
            enumStr = "Qualcomm";
            break;
		case TRAP::Graphics::GPUVendor::Intel:
            enumStr = "Intel";
            break;
		case TRAP::Graphics::GPUVendor::Apple:
            enumStr = "Apple";
            break;
		case TRAP::Graphics::GPUVendor::Vivante:
            enumStr = "Vivante";
            break;
		case TRAP::Graphics::GPUVendor::VeriSilicon:
            enumStr = "VeriSilicon";
            break;
		case TRAP::Graphics::GPUVendor::Kazan:
            enumStr = "Kazan";
            break;
		case TRAP::Graphics::GPUVendor::Codeplay:
            enumStr = "Codeplay";
            break;
		case TRAP::Graphics::GPUVendor::Mesa:
            enumStr = "Mesa";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
template<>
struct fmt::formatter<TRAP::Graphics::NVIDIAReflexLatencyMode>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::NVIDIAReflexLatencyMode latencyMode,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(latencyMode)
        {
        case TRAP::Graphics::NVIDIAReflexLatencyMode::Disabled:
            enumStr = "Disabled";
            break;
        case TRAP::Graphics::NVIDIAReflexLatencyMode::Enabled:
            enumStr = "Enabled";
            break;
        case TRAP::Graphics::NVIDIAReflexLatencyMode::EnabledBoost:
            enumStr = "Enabled+Boost";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
template<>
struct fmt::formatter<TRAP::Graphics::AMDAntiLagMode>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::AMDAntiLagMode mode,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(mode)
        {
		case TRAP::Graphics::AMDAntiLagMode::DriverControl:
			enumStr = "DriverControl";
			break;
        case TRAP::Graphics::AMDAntiLagMode::Disabled:
            enumStr = "Disabled";
            break;
        case TRAP::Graphics::AMDAntiLagMode::Enabled:
            enumStr = "Enabled";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::ResourceState>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::ResourceState resourceState,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(resourceState)
        {
        case TRAP::Graphics::ResourceState::Undefined:
            enumStr = "Undefined";
            break;
        case TRAP::Graphics::ResourceState::VertexAndConstantBuffer:
            enumStr = "VertexAndConstantBuffer";
            break;
        case TRAP::Graphics::ResourceState::IndexBuffer:
            enumStr = "IndexBuffer";
            break;
        case TRAP::Graphics::ResourceState::RenderTarget:
            enumStr = "RenderTarget";
            break;
        case TRAP::Graphics::ResourceState::UnorderedAccess:
            enumStr = "UnorderedAccess";
            break;
        case TRAP::Graphics::ResourceState::DepthWrite:
            enumStr = "DepthWrite";
            break;
        case TRAP::Graphics::ResourceState::DepthRead:
            enumStr = "DepthRead";
            break;
        case TRAP::Graphics::ResourceState::NonPixelShaderResource:
            enumStr = "NonPixelShaderResource";
            break;
        case TRAP::Graphics::ResourceState::PixelShaderResource:
            enumStr = "PixelShaderResource";
            break;
        case TRAP::Graphics::ResourceState::ShaderResource:
            enumStr = "ShaderResource";
            break;
        case TRAP::Graphics::ResourceState::StreamOut:
            enumStr = "StreamOut";
            break;
        case TRAP::Graphics::ResourceState::IndirectArgument:
            enumStr = "IndirectArgument";
            break;
        case TRAP::Graphics::ResourceState::CopyDestination:
            enumStr = "CopyDestination";
            break;
        case TRAP::Graphics::ResourceState::CopySource:
            enumStr = "CopySource";
            break;
        case TRAP::Graphics::ResourceState::GenericRead:
            enumStr = "GenericRead";
            break;
        case TRAP::Graphics::ResourceState::Present:
            enumStr = "Present";
            break;
        case TRAP::Graphics::ResourceState::Common:
            enumStr = "Common";
            break;
        case TRAP::Graphics::ResourceState::RayTracingAccelerationStructure:
            enumStr = "RayTracingAccelerationStructure";
            break;
        case TRAP::Graphics::ResourceState::ShadingRateSource:
            enumStr = "ShadingRateSource";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::QueueType>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::QueueType queueType,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(queueType)
        {
        case TRAP::Graphics::QueueType::Graphics:
            enumStr = "Graphics";
            break;
        case TRAP::Graphics::QueueType::Transfer:
            enumStr = "Transfer";
            break;
        case TRAP::Graphics::QueueType::Compute:
            enumStr = "Compute";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::ShaderStage>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::ShaderStage shaderStage,
	                                            fmt::format_context& ctx)
    {
		if(shaderStage == TRAP::Graphics::ShaderStage::None)
			return ctx.out();

        std::string enumStr{};

		if((shaderStage & TRAP::Graphics::ShaderStage::Vertex) != TRAP::Graphics::ShaderStage::None)
		{
			if(!enumStr.empty())
				enumStr += '/';
			enumStr += "Vertex";
		}
		if((shaderStage & TRAP::Graphics::ShaderStage::TessellationControl) != TRAP::Graphics::ShaderStage::None)
		{
			if(!enumStr.empty())
				enumStr += '/';
			enumStr += "TessellationControl";
		}
		if((shaderStage & TRAP::Graphics::ShaderStage::TessellationEvaluation) != TRAP::Graphics::ShaderStage::None)
		{
			if(!enumStr.empty())
				enumStr += '/';
			enumStr += "TessellationEvaluation";
		}
		if((shaderStage & TRAP::Graphics::ShaderStage::Geometry) != TRAP::Graphics::ShaderStage::None)
		{
			if(!enumStr.empty())
				enumStr += '/';
			enumStr += "Geometry";
		}
		if((shaderStage & TRAP::Graphics::ShaderStage::Fragment) != TRAP::Graphics::ShaderStage::None)
		{
			if(!enumStr.empty())
				enumStr += '/';
			enumStr += "Fragment";
		}
		if((shaderStage & TRAP::Graphics::ShaderStage::Compute) != TRAP::Graphics::ShaderStage::None)
		{
			if(!enumStr.empty())
				enumStr += '/';
			enumStr += "Compute";
		}
		if((shaderStage & TRAP::Graphics::ShaderStage::RayTracing) != TRAP::Graphics::ShaderStage::None)
		{
			if(!enumStr.empty())
				enumStr += '/';
			enumStr += "RayTracing";
		}

		if(enumStr.empty())
		{
			TRAP_ASSERT(false, "fmt::formatter<TRAP::Graphics::ShaderStage>: Missing enum value!");
			enumStr = "<MISSING ENUM VALUE>";
		}

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::ShaderType>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::ShaderType shaderType,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};

		switch(shaderType)
		{
		case TRAP::Graphics::ShaderType::Graphics:
			enumStr = "Graphics";
			break;

		case TRAP::Graphics::ShaderType::Compute:
			enumStr = "Compute";
			break;
		}

		if(enumStr.empty())
		{
			TRAP_ASSERT(false, "fmt::formatter<TRAP::Graphics::ShaderType>: Missing enum value!");
			enumStr = "<MISSING ENUM VALUE>";
		}

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::ShadingRate>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::ShadingRate shadingRate,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};

		switch(shadingRate)
		{
		case TRAP::Graphics::ShadingRate::NotSupported:
			enumStr = "1x1";
			break;
		case TRAP::Graphics::ShadingRate::Full:
			enumStr = "1x1";
			break;
		case TRAP::Graphics::ShadingRate::Half:
			enumStr = "2x2";
			break;
		case TRAP::Graphics::ShadingRate::Quarter:
			enumStr = "4x4";
			break;
		case TRAP::Graphics::ShadingRate::Eighth:
			enumStr = "8x8";
			break;
		case TRAP::Graphics::ShadingRate::OneXTwo:
			enumStr = "1x2";
			break;
		case TRAP::Graphics::ShadingRate::TwoXOne:
			enumStr = "2x1";
			break;
		case TRAP::Graphics::ShadingRate::TwoXFour:
			enumStr = "2x4";
			break;
		case TRAP::Graphics::ShadingRate::FourXTwo:
			enumStr = "4x2";
			break;
		}

		if(enumStr.empty())
		{
			TRAP_ASSERT(false, "fmt::formatter<TRAP::Graphics::ShadingRate>: Missing enum value!");
			enumStr = "<MISSING ENUM VALUE>";
		}

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//
//ConvertToType------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
template<>
[[nodiscard]] constexpr TRAP::Graphics::NVIDIAReflexLatencyMode TRAP::Utils::String::ConvertToType(const std::string& input)
{
    if(Utils::String::CompareAnyCase("Enabled", input))
        return Graphics::NVIDIAReflexLatencyMode::Enabled;
    if(Utils::String::CompareAnyCase("Enabled+Boost", input))
        return Graphics::NVIDIAReflexLatencyMode::EnabledBoost;
    if(Utils::String::CompareAnyCase("Disabled", input))
        return Graphics::NVIDIAReflexLatencyMode::Disabled;

    TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::NVIDIAReflexLatencyMode!");
    return Graphics::NVIDIAReflexLatencyMode::Disabled;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
template<>
[[nodiscard]] constexpr TRAP::Graphics::AMDAntiLagMode TRAP::Utils::String::ConvertToType(const std::string& input)
{
    if(Utils::String::CompareAnyCase("Enabled", input))
        return Graphics::AMDAntiLagMode::Enabled;
    if(Utils::String::CompareAnyCase("Disabled", input))
        return Graphics::AMDAntiLagMode::Disabled;

    TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::AMDAntiLagMode!");
    return Graphics::AMDAntiLagMode::Disabled;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] constexpr TRAP::Graphics::RenderAPI TRAP::Utils::String::ConvertToType(const std::string& input)
{
    if (Utils::String::CompareAnyCase("Vulkan", input) || Utils::String::CompareAnyCase("VulkanAPI", input))
        return Graphics::RenderAPI::Vulkan;

    if (Utils::String::CompareAnyCase("NONE", input))
        return Graphics::RenderAPI::NONE;

    TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::RenderAPI!");
    return Graphics::RenderAPI::NONE;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] constexpr TRAP::Graphics::AntiAliasing TRAP::Utils::String::ConvertToType(const std::string& input)
{
    if (Utils::String::CompareAnyCase("Off", input))
        return Graphics::AntiAliasing::Off;

    if (Utils::String::CompareAnyCase("MSAA", input))
        return Graphics::AntiAliasing::MSAA;

    TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::AntiAliasing!");
    return Graphics::AntiAliasing::Off;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] constexpr TRAP::Graphics::SampleCount TRAP::Utils::String::ConvertToType(const std::string& input)
{
    if (Utils::String::CompareAnyCase("1", input))
        return Graphics::SampleCount::One;

    if (Utils::String::CompareAnyCase("2", input))
        return Graphics::SampleCount::Two;

    if (Utils::String::CompareAnyCase("4", input))
        return Graphics::SampleCount::Four;

    if (Utils::String::CompareAnyCase("8", input))
        return Graphics::SampleCount::Eight;

    if (Utils::String::CompareAnyCase("16", input))
        return Graphics::SampleCount::Sixteen;

    TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::SampleCount!");
    return Graphics::SampleCount::One;
}

#endif /*TRAP_RENDERERAPI_TYPES_H*/
