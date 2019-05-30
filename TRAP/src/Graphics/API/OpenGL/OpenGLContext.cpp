#include "TRAPPCH.h"
#include "OpenGLContext.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLContext::OpenGLContext(Window* window)
{
	glfwMakeContextCurrent(static_cast<GLFWwindow*>(window->GetNativeWindow()));

	TP_RENDERER_ASSERT(gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)), "Could not initialize GLAD");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLContext::~OpenGLContext()
{
	TP_DEBUG("[Context][OpenGL] Destroying Context");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLContext::SetVSyncIntervalInternal(const unsigned int interval)
{
	glfwSwapInterval(static_cast<int>(interval));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLContext::Present(Window* window)
{
	glfwSwapBuffers(static_cast<GLFWwindow*>(window->GetNativeWindow()));
}