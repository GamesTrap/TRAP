#ifndef _TRAP_OPENGLCONTEXT_H_
#define _TRAP_OPENGLCONTEXT_H_

#include "Graphics/API/Context.h"

namespace TRAP::Graphics::API
{
	class OpenGLContext final : public Context
	{
	public:
		explicit OpenGLContext(Window* window);
		~OpenGLContext();
		OpenGLContext(const OpenGLContext&) = default;
		OpenGLContext& operator=(const OpenGLContext&) = default;
		OpenGLContext(OpenGLContext&&) = default;
		OpenGLContext& operator=(OpenGLContext&&) = default;

		void SetVSyncIntervalInternal(unsigned int interval) override;

		static void Present(Window* window);
		void UseInternal(Window* window) override;

		static OpenGLContext* Get();

		static bool IsOpenGLCapable();

	private:
		static bool s_IsGladInitialized;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::OpenGLContext* TRAP::Graphics::API::OpenGLContext::Get()
{
	return dynamic_cast<OpenGLContext*>(s_Context.get());
}

#endif /*_TRAP_OPENGLCONTEXT_H_*/