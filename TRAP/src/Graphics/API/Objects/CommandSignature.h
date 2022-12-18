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
		static TRAP::Ref<CommandSignature> Create(const RendererAPI::CommandSignatureDesc& desc);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~CommandSignature();

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

#endif /*TRAP_COMMANDSIGNATURE_H*/