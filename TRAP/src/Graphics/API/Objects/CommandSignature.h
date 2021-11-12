#ifndef TRAP_COMMANDSIGNATURE_H
#define TRAP_COMMANDSIGNATURE_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class CommandSignature
	{
	public:
		static TRAP::Ref<CommandSignature> Create(const RendererAPI::CommandSignatureDesc& desc);

		virtual ~CommandSignature();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		CommandSignature(const CommandSignature&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		CommandSignature& operator=(const CommandSignature&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		CommandSignature(CommandSignature&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		CommandSignature& operator=(CommandSignature&&) = default;

	protected:
		CommandSignature();

		//No Graphic API independent data
	};
}

#endif /*TRAP_COMMANDSIGNATURE_H*/