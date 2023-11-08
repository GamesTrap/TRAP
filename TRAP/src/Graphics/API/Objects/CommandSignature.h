#ifndef TRAP_COMMANDSIGNATURE_H
#define TRAP_COMMANDSIGNATURE_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class CommandSignature
	{
	public:
		/// @brief Create a new command signature from the given description.
		/// @param desc Command signature description.
		/// @return Created command signature.
		[[nodiscard]] static TRAP::Ref<CommandSignature> Create(const RendererAPI::CommandSignatureDesc& desc);

		/// @brief Destructor.
		virtual ~CommandSignature();

		/// @brief Copy constructor.
		consteval CommandSignature(const CommandSignature&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval CommandSignature& operator=(const CommandSignature&) noexcept = delete;
		/// @brief Move constructor.
		constexpr CommandSignature(CommandSignature&&) noexcept = default;
		/// @brief Move assignment operator.
		CommandSignature& operator=(CommandSignature&&) noexcept = default;

	protected:
		/// @brief Constructor.
		CommandSignature();

		//No Graphic API independent data
	};
}

#endif /*TRAP_COMMANDSIGNATURE_H*/