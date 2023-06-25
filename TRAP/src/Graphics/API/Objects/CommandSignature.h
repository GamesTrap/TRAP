#ifndef TRAP_COMMANDSIGNATURE_H
#define TRAP_COMMANDSIGNATURE_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class CommandSignature
	{
	public:
		/// <summary>
		/// Create a new command signature from the given description.
		/// </summary>
		/// <param name="desc">Command signature description.</param>
		/// <returns>Created command signature.</returns>
		[[nodiscard]] static TRAP::Ref<CommandSignature> Create(const RendererAPI::CommandSignatureDesc& desc);

		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr virtual ~CommandSignature();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		CommandSignature(const CommandSignature&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		CommandSignature& operator=(const CommandSignature&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		CommandSignature(CommandSignature&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		CommandSignature& operator=(CommandSignature&&) noexcept = default;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		CommandSignature();

		//No Graphic API independent data
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::CommandSignature::~CommandSignature()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererCommandSignaturePrefix, "Destroying CommandSignature");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

#endif /*TRAP_COMMANDSIGNATURE_H*/