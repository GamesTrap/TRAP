#pragma once

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

		static OpenGLContext* Get() { return dynamic_cast<OpenGLContext*>(s_Context.get()); }
	};
}
