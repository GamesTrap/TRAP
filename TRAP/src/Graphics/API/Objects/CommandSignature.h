#ifndef TRAP_COMMANDSIGNATURE_H
#define TRAP_COMMANDSIGNATURE_H

#include "Core/Base.h"
#include "Utils/SmartPtr.h"

namespace TRAP::Graphics
{
	struct CommandSignatureDesc;

	class CommandSignature
	{
	public:
		/// @brief Create a new command signature from the given description.
		/// @param desc Command signature description.
		/// @return Created command signature.
		[[nodiscard]] static TRAP::Ref<CommandSignature> Create(const CommandSignatureDesc& desc);

		/// @brief Destructor.
		constexpr virtual ~CommandSignature();

		/// @brief Copy constructor.
		consteval CommandSignature(const CommandSignature&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval CommandSignature& operator=(const CommandSignature&) noexcept = delete;
		/// @brief Move constructor.
		constexpr CommandSignature(CommandSignature&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr CommandSignature& operator=(CommandSignature&&) noexcept = default;

	protected:
		/// @brief Constructor.
		constexpr CommandSignature();

		//No Graphic API independent data

	private:
		static void DebugLog(std::string_view msg);
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::CommandSignature::CommandSignature()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	if(!std::is_constant_evaluated())
	{
		DebugLog("Creating CommandSignature");
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::CommandSignature::~CommandSignature()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	if(!std::is_constant_evaluated())
	{
		DebugLog("Destroying CommandSignature");
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

#endif /*TRAP_COMMANDSIGNATURE_H*/
