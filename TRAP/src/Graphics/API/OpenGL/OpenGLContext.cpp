#include "TRAPPCH.h"
#include "OpenGLContext.h"

#include "Window/Window.h"
#include "OpenGLCommon.h"

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLContext::s_IsGladInitialized = false;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLContext::OpenGLContext(Window* window)
{
	glfwMakeContextCurrent(static_cast<GLFWwindow*>(window->GetNativeWindow()));

	if (!s_IsGladInitialized)
	{
		TRAP_RENDERER_ASSERT(gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)), "Could not initialize GLAD");
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
	glfwSwapInterval(static_cast<int32_t>(interval));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLContext::Present(const std::unique_ptr<Window>& window)
{
	glfwSwapBuffers(static_cast<GLFWwindow*>(window->GetNativeWindow()));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLContext::UseInternal(const std::unique_ptr<Window>& window)
{
	if (Window::GetActiveWindows() > 1)
		glfwMakeContextCurrent(static_cast<GLFWwindow*>(window->GetNativeWindow()));
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

	//Check if OpenGL can be loaded
	if (!s_IsGladInitialized)
	{
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
			return false;
		s_IsGladInitialized = true;
	}

	//Check if OpenGL Version is higher or equal to 4.6
	const bool isOpenGLCapable = GLVersion.major >= 4 && GLVersion.minor >= 6;

	//Now destroy the test window
	glfwDestroyWindow(OpenGLTestWindow);
	glfwMakeContextCurrent(nullptr);

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