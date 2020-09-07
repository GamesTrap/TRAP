#include "TRAPPCH.h"
#include "OpenGLContext.h"

#include "Window/Window.h"
#include "OpenGLCommon.h"
#include "Window/WindowingAPI.h"

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLContext::s_IsOpenGLInitialized = false;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLContext::OpenGLContext(Window* window)
{
	TP_PROFILE_FUNCTION();
	
	INTERNAL::WindowingAPI::MakeContextCurrent(static_cast<INTERNAL::WindowingAPI::InternalWindow*>(window->GetInternalWindow()));

	if (!s_IsOpenGLInitialized)
	{
		TRAP_RENDERER_ASSERT(TRAPLoadOpenGLLoader(reinterpret_cast<TRAPLoadProc>(INTERNAL::WindowingAPI::GetProcAddress)), "Could not initialize OpenGL");
		s_IsOpenGLInitialized = true;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLContext::~OpenGLContext()
{
	TP_PROFILE_FUNCTION();
	
	TP_DEBUG(Log::ContextOpenGLPrefix, "Destroying Context");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLContext::SetVSyncIntervalInternal(const uint32_t interval)
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::SwapInterval(static_cast<int32_t>(interval));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLContext::Present(const Scope<Window>& window)
{
	TP_PROFILE_FUNCTION();

	if (window)
		INTERNAL::WindowingAPI::SwapBuffers(static_cast<INTERNAL::WindowingAPI::InternalWindow*>(window->GetInternalWindow()));
	else
		TP_WARN(Log::ContextOpenGLPrefix, "Tried to pass nullptr to Present!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLContext::UseInternal(Window* window)
{
	TP_PROFILE_FUNCTION();

	if (window)
	{
		if (Window::GetActiveWindows() > 1)
			INTERNAL::WindowingAPI::MakeContextCurrent(static_cast<INTERNAL::WindowingAPI::InternalWindow*>(window->GetInternalWindow()));
	}
	else
		INTERNAL::WindowingAPI::MakeContextCurrent(nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLContext* TRAP::Graphics::API::OpenGLContext::Get()
{
	TP_PROFILE_FUNCTION();

	return dynamic_cast<OpenGLContext*>(s_Context.get());
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLContext::IsOpenGLCapable()
{
	TP_PROFILE_FUNCTION();

	//Create OpenGL 4.6 Test window
	INTERNAL::WindowingAPI::SetContextAPI(INTERNAL::WindowingAPI::ContextAPI::OpenGL);
	INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Visible, false);
	INTERNAL::WindowingAPI::WindowHint(INTERNAL::WindowingAPI::Hint::Focused, false);
	Scope<INTERNAL::WindowingAPI::InternalWindow> OpenGLTestWindow = INTERNAL::WindowingAPI::CreateWindow(800, 600, "OpenGL Version Tester", nullptr, nullptr);
	INTERNAL::WindowingAPI::DefaultWindowHints();
	if (!OpenGLTestWindow)
		return false;

	INTERNAL::WindowingAPI::MakeContextCurrent(OpenGLTestWindow.get());

	//Check if OpenGL can be loaded
	if (!s_IsOpenGLInitialized)
	{
		if (!TRAPLoadOpenGLLoader(reinterpret_cast<TRAPLoadProc>(INTERNAL::WindowingAPI::GetProcAddress)))
			return false;
		s_IsOpenGLInitialized = true;
	}

	//Check if OpenGL Version is higher or equal to 4.6
	const bool isOpenGLCapable = OpenGLVersion.Major >= 4 && OpenGLVersion.Minor >= 6;

	//Now destroy the test window
	INTERNAL::WindowingAPI::DestroyWindow(std::move(OpenGLTestWindow));
	INTERNAL::WindowingAPI::MakeContextCurrent(nullptr);

	return isOpenGLCapable;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLContext::IsExtensionSupported(const std::string_view extension)
{
	uint32_t numExtensions = 0;

	OpenGLCall(glGetIntegerv(GL_NUM_EXTENSIONS, reinterpret_cast<int32_t*>(&numExtensions)));

	for (uint32_t i = 0; i < numExtensions; i++)
	{
		OpenGLCall(const uint8_t* e = glGetStringi(GL_EXTENSIONS, i));
		if (!strcmp(reinterpret_cast<const char*>(e), extension.data()))
			return true;
	}

	return false;
}