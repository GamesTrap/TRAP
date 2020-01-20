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

		void SetVSyncIntervalInternal(uint32_t interval) override;

		static void Present(const Scope<Window>& window);
		void UseInternal(const Window* window) override;

		static OpenGLContext* Get();

		static bool IsOpenGLCapable();

	private:
		static bool IsExtensionSupported(const std::string& extension);
		
		static bool s_IsOpenGLInitialized;
	};
}

#endif /*_TRAP_OPENGLCONTEXT_H_*/