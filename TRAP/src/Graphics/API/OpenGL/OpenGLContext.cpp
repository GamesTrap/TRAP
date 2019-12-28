#include "TRAPPCH.h"
#include "OpenGLContext.h"

#include "Window/Window.h"
#include "OpenGLCommon.h"
#include "Window/WindowingAPI.h"

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLContext::s_IsGladInitialized = false;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLContext::OpenGLContext(Window* window)
{
	INTERNAL::WindowingAPI::MakeContextCurrent(window->GetInternalWindow());

	if (!s_IsGladInitialized)
	{
		TRAP_RENDERER_ASSERT(gladLoadGLLoader(reinterpret_cast<GLADloadproc>(INTERNAL::WindowingAPI::GetProcAddress)), "Could not initialize GLAD");
		s_IsGladInitialized = true;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLContext::~OpenGLContext()
{
	TP_DEBUG("[Context][OpenGL] Destroying Context");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLContext::SetVSyncIntervalInternal(const uint32_t interval)
{
	INTERNAL::WindowingAPI::SwapInterval(static_cast<int32_t>(interval));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLContext::Present(const std::unique_ptr<Window>& window)
{
	INTERNAL::WindowingAPI::SwapBuffers(window->GetInternalWindow());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLContext::UseInternal(const std::unique_ptr<Window>& window)
{
	if (window)
	{
		if (Window::GetActiveWindows() > 1)
			INTERNAL::WindowingAPI::MakeContextCurrent(window->GetInternalWindow());
	}
	else
		INTERNAL::WindowingAPI::MakeContextCurrent(nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLContext* TRAP::Graphics::API::OpenGLContext::Get()
{
	return dynamic_cast<OpenGLContext*>(s_Context.get());
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLContext::IsOpenGLCapable()
{
	//Create OpenGL 4.6 Test window
	INTERNAL::WindowingAPI::SetContextAPI(INTERNAL::WindowingAPI::ContextAPI::OpenGL);
	INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Visible, false);
	INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Focused, false);
	const Ref<INTERNAL::WindowingAPI::InternalWindow> OpenGLTestWindow = INTERNAL::WindowingAPI::CreateWindow(800, 600, "OpenGL Version Tester", nullptr, nullptr);
	INTERNAL::WindowingAPI::DefaultWindowHints();
	if (!OpenGLTestWindow)
		return false;

	INTERNAL::WindowingAPI::MakeContextCurrent(OpenGLTestWindow);

	//Check if OpenGL can be loaded
	if (!s_IsGladInitialized)
	{
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(INTERNAL::WindowingAPI::GetProcAddress)))
			return false;
		s_IsGladInitialized = true;
	}

	//Check if OpenGL Version is higher or equal to 4.6
	const bool isOpenGLCapable = GLVersion.major >= 4 && GLVersion.minor >= 6;

	//Now destroy the test window
	INTERNAL::WindowingAPI::DestroyWindow(OpenGLTestWindow);
	INTERNAL::WindowingAPI::MakeContextCurrent(nullptr);

	return isOpenGLCapable;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLContext::IsExtensionSupported(const std::string& extension)
{
	uint32_t numExtensions = 0;

	OpenGLCall(glGetIntegerv(GL_NUM_EXTENSIONS, reinterpret_cast<int32_t*>(&numExtensions)));

	for (uint32_t i = 0; i < numExtensions; i++)
	{
		OpenGLCall(const uint8_t* e = glGetStringi(GL_EXTENSIONS, i));
		if (!strcmp(reinterpret_cast<const char*>(e), extension.c_str()))
			return true;
	}

	return false;
}