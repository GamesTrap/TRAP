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
		/// Default Copy Constructor.
		/// </summary>
		CommandSignature(const CommandSignature&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		CommandSignature& operator=(const CommandSignature&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		CommandSignature(CommandSignature&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		CommandSignature& operator=(CommandSignature&&) = default;

	protected:
		CommandSignature();

		//No Graphic API independent data
	};
}

#endif /*TRAP_COMMANDSIGNATURE_H*/