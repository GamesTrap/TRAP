#include "TRAPPCH.h"
#include "OpenGLContext.h"

#include "Window/Window.h"

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLContext::s_IsGladInitialized = false;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLContext::OpenGLContext(Window* window)
{
	glfwMakeContextCurrent(static_cast<GLFWwindow*>(window->GetNativeWindow()));

	if(!s_IsGladInitialized)
	{
		TP_RENDERER_ASSERT(gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)), "Could not initialize GLAD");
		s_IsGladInitialized = true;
	}
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

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLContext::IsOpenGLCapable()
{
	//Create OpenGL 4.6 Test window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, false);
	glfwWindowHint(GLFW_FOCUSED, false);
	glfwWindowHint(GLFW_FOCUS_ON_SHOW, false);
	GLFWwindow* OpenGLTestWindow = glfwCreateWindow(800, 600, "OpenGL Version Tester", nullptr, nullptr);
	glfwDefaultWindowHints();
	if (!OpenGLTestWindow)
		return false;

	glfwMakeContextCurrent(OpenGLTestWindow);

	//CHeck if OpenGL can be loaded
	if (!s_IsGladInitialized)
	{
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
			return false;
		s_IsGladInitialized = true;
	}

	//Check if OpenGL Version is higher or equal to 4.6
	const bool isOpenGLCapable = GLVersion.major >= 4 && GLVersion.minor >= 6;
	glfwDestroyWindow(OpenGLTestWindow);
	glfwMakeContextCurrent(nullptr);

	return isOpenGLCapable;
}