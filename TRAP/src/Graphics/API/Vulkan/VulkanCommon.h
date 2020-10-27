#ifndef _TRAP_VULKANCOMMON_H_
#define _TRAP_VULKANCOMMON_H_

namespace TRAP::Graphics::API
{
	bool ErrorCheck(VkResult result, const char* function, const char* file, int32_t line);
	std::string VkShaderStageFlagBitsToString(VkShaderStageFlagBits stage);
	std::string BaseTypeToString(spirv_cross::SPIRType::BaseType type);
	std::string StorageClassToString(spv::StorageClass sc);
	std::string DecorationToString(spv::Decoration dec);
	std::string VkDescriptorTypeToString(VkDescriptorType type);
}

//-------------------------------------------------------------------------------------------------------------------//

namespace TRAP::Graphics
{
#ifdef TRAP_DEBUG
#if __cplusplus > 201703L
	#define VkCall(x) std::source_location loc = std::source_location::current(); ::TRAP::Graphics::API::ErrorCheck(x, #x, loc.file_name, loc.line);
#else
	#define VkCall(x) ::TRAP::Graphics::API::ErrorCheck(x, #x, __FILE__, __LINE__);
#endif
#else
	#define VkCall(x) x;
#endif
}

#endif /*_TRAP_VULKANCOMMON_H_*/