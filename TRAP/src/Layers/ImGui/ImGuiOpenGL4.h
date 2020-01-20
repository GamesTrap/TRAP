#ifndef _TRAP_IMGUI_OPENGL3_H_
#define _TRAP_IMGUI_OPENGL3_H_

//Backend API
IMGUI_IMPL_API bool ImGuiOpenGL4Init();
IMGUI_IMPL_API void ImGuiOpenGL4Shutdown();
IMGUI_IMPL_API void ImGuiOpenGL4NewFrame();
IMGUI_IMPL_API void ImGuiOpenGL4RenderDrawData(ImDrawData* drawData);

//(Optional) Called by Init/NewFrame/Shutdown
IMGUI_IMPL_API bool ImGuiOpenGL4CreateFontsTexture();
IMGUI_IMPL_API void ImGuiOpenGL4DestroyFontsTexture();
IMGUI_IMPL_API bool ImGuiOpenGL4CreateDeviceObjects();
IMGUI_IMPL_API void ImGuiOpenGL4DestroyDeviceObjects();

#endif /*_TRAP_IMGUI_OPENGL3_H_*/