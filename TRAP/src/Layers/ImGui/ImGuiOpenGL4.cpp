/*The MIT License(MIT)

Copyright(c) 2014 - 2020 Omar Cornut

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Modified by: Jan "GamesTrap" Schuerkamp
*/

#include "TRAPPCH.h"
#include "ImGuiOpenGL4.h"

//OpenGL Data
static GLuint g_FontTexture = 0;
static GLuint g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
static int32_t g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0; //Uniforms location
static int32_t g_AttribLocationVtxPos = 0, g_AttribLocationVtxUV = 0, g_AttribLocationVtxColor = 0; //Vertex attributes location
static uint32_t g_VboHandle = 0, g_ElementsHandle = 0;

//Forward Declarations
static void ImGuiOpenGL4InitPlatformInterface();
static void ImGuiOpenGL4ShutdownPlatformInterface();

//Functions
bool ImGuiOpenGL4Init()
{
	TP_PROFILE_FUNCTION();

	//Setup back-end capabilities flags
	ImGuiIO& io = ImGui::GetIO();
	io.BackendRendererName = "TRAP_OpenGL4";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset; //We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports; //We can create multi-viewports on the Renderer side (optional)

	//Make a dummy OpenGL call (we don't actually need the result)
	//IF YOU GET A CRASH HERE: it probably means that you haven't initialized the OpenGL function loader used by this code.
	//Desktop OpenGL 3/4 needs a function loader.
	GLint currentTexture = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		ImGuiOpenGL4InitPlatformInterface();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGuiOpenGL4Shutdown()
{
	TP_PROFILE_FUNCTION();

	ImGuiOpenGL4ShutdownPlatformInterface();
	ImGuiOpenGL4DestroyDeviceObjects();
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGuiOpenGL4NewFrame()
{
	TP_PROFILE_FUNCTION();

	if (!g_ShaderHandle)
		ImGuiOpenGL4CreateDeviceObjects();
}

//-------------------------------------------------------------------------------------------------------------------//

static void ImGuiOpenGL4SetupRenderState(ImDrawData* drawData, const int32_t fbWidth, const int32_t fbHeight, GLuint vertexArrayObject)
{
	TP_PROFILE_FUNCTION();

	//Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Setup viewport, orthographic projection matrix
	//Our visible imgui space lies from drawData->DisplayPos (top left) to drawData->DisplayPos + drawData->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
	glViewport(0, 0, static_cast<GLsizei>(fbWidth), static_cast<GLsizei>(fbHeight));
	const float L = drawData->DisplayPos.x;
	const float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
	const float T = drawData->DisplayPos.y;
	const float B = drawData->DisplayPos.y + drawData->DisplaySize.y;
	const std::array<std::array<float, 4>, 4> orthoProjection =
	{
		{
			{ 2.0f / (R - L), 0.0f, 0.0f, 0.0f },
			{ 0.0f, 2.0f / (T - B), 0.0f, 0.0f },
			{ 0.0f, 0.0f, -1.0f, 0.0f },
			{ (R + L) / (L - R), (T + B) / (B - T), 0.0f, 1.0f }
		}
	};
	glUseProgram(g_ShaderHandle);
	glUniform1i(g_AttribLocationTex, 0);
	glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &orthoProjection[0][0]);
	glBindSampler(0, 0); //We use combined texture/sampler state. Applications using OpenGL 3.3 may set that otherwise.

	(void)vertexArrayObject;

	//Bind vertex/index buffers and setup attributes for ImDrawVert
	glVertexArrayVertexBuffer(vertexArrayObject, 0, g_VboHandle, 0, sizeof(ImDrawVert));
	
	glEnableVertexArrayAttrib(vertexArrayObject, g_AttribLocationVtxPos);
	glVertexArrayAttribFormat(vertexArrayObject, g_AttribLocationVtxPos, 2, GL_FLOAT, GL_FALSE, static_cast<GLintptr>(IM_OFFSETOF(ImDrawVert, pos)));
	glVertexArrayAttribBinding(vertexArrayObject, g_AttribLocationVtxPos, 0);
	
	glEnableVertexArrayAttrib(vertexArrayObject, g_AttribLocationVtxUV);
	glVertexArrayAttribFormat(vertexArrayObject, g_AttribLocationVtxUV, 2, GL_FLOAT, GL_FALSE, static_cast<GLintptr>(IM_OFFSETOF(ImDrawVert, uv)));
	glVertexArrayAttribBinding(vertexArrayObject, g_AttribLocationVtxUV, 0);
	
	glEnableVertexArrayAttrib(vertexArrayObject, g_AttribLocationVtxColor);
	glVertexArrayAttribFormat(vertexArrayObject, g_AttribLocationVtxColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, static_cast<GLintptr>(IM_OFFSETOF(ImDrawVert, col)));
	glVertexArrayAttribBinding(vertexArrayObject, g_AttribLocationVtxColor, 0);
	
	glVertexArrayElementBuffer(vertexArrayObject, g_ElementsHandle);
}

//-------------------------------------------------------------------------------------------------------------------//

//OpenGL 3 Render function.
//(this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call this directly from your main loop)
//Note that this implementation is little overcomplicated because we are saving/setting up/restoring every OpenGL state explicitly, in order to be able to run within any OpnGL engine that doesn't do so.
void ImGuiOpenGL4RenderDrawData(ImDrawData* drawData)
{
	TP_PROFILE_FUNCTION();

	//Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	const int32_t fbWidth = static_cast<int32_t>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
	const int32_t fbHeight = static_cast<int32_t>(drawData->DisplaySize.y * drawData->FramebufferScale.y);
	if (fbWidth <= 0 || fbHeight <= 0)
		return;

	//Backup OpenGL state
	GLenum lastActiveTexture = 0; glGetIntegerv(GL_ACTIVE_TEXTURE, reinterpret_cast<GLint*>(&lastActiveTexture));
	glActiveTexture(GL_TEXTURE0);
	GLint lastProgram = 0; glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);
	GLint lastTexture = 0; glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
	GLint lastSampler = 0; glGetIntegerv(GL_SAMPLER_BINDING, &lastSampler);
	GLint lastArrayBuffer = 0; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &lastArrayBuffer);
	GLint lastVertexArrayObject = 0; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &lastVertexArrayObject);
	std::array<GLint, 2> lastPolygonMode{0, 0}; glGetIntegerv(GL_POLYGON_MODE, lastPolygonMode.data());
	std::array<GLint, 4> lastViewport{ 0, 0, 0, 0 }; glGetIntegerv(GL_VIEWPORT, lastViewport.data());
	std::array<GLint, 4> lastScissorBox{ 0, 0, 0, 0 }; glGetIntegerv(GL_SCISSOR_BOX, lastScissorBox.data());
	GLenum lastBlendSRCRGB = 0; glGetIntegerv(GL_BLEND_SRC_RGB, reinterpret_cast<GLint*>(&lastBlendSRCRGB));
	GLenum lastBlendDSTRGB = 0; glGetIntegerv(GL_BLEND_DST_RGB, reinterpret_cast<GLint*>(&lastBlendDSTRGB));
	GLenum lastBlendSRCAlpha = 0; glGetIntegerv(GL_BLEND_SRC_ALPHA, reinterpret_cast<GLint*>(&lastBlendSRCAlpha));
	GLenum lastBlendDSTAlpha = 0; glGetIntegerv(GL_BLEND_DST_ALPHA, reinterpret_cast<GLint*>(&lastBlendDSTAlpha));
	GLenum lastBlendEquationRGB = 0; glGetIntegerv(GL_BLEND_EQUATION_RGB, reinterpret_cast<GLint*>(&lastBlendEquationRGB));
	GLenum lastBlendEquationAlpha = 0; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, reinterpret_cast<GLint*>(&lastBlendEquationAlpha));
	const GLboolean lastEnableBlend = glIsEnabled(GL_BLEND);
	const GLboolean lastEnableCullFace = glIsEnabled(GL_CULL_FACE);
	const GLboolean lastEnableDepthTest = glIsEnabled(GL_DEPTH_TEST);
	const GLboolean lastEnableScissorTest = glIsEnabled(GL_SCISSOR_TEST);
	bool clipOriginLowerLeft = true;
	GLenum lastClipOrigin = 0; glGetIntegerv(GL_CLIP_ORIGIN, reinterpret_cast<GLint*>(&lastClipOrigin)); //Suppor for OpenGL 4.5's glClipControl(GL_UPPER_LEFT)
	if (lastClipOrigin == GL_UPPER_LEFT)
		clipOriginLowerLeft = false;

	//Setup desired OpenGL state
	//Recreate the VAO every time (this is to easily allow multiple OpenGL contexts to be rendered to. VAO are not shared among OpenGL contexts)
	//The renderer would actually work without any VAO bound, but then our VertexAttrib calls would overwrite the default one currently bound.
	GLuint vertexArrayObject = 0;
	glCreateVertexArrays(1, &vertexArrayObject);
	ImGuiOpenGL4SetupRenderState(drawData, fbWidth, fbHeight, vertexArrayObject);

	//Will project scissor/clipping rectangles into framebuffer space
	const ImVec2 clipOff = drawData->DisplayPos; //(0,0) unless using multi-viewports
	const ImVec2 clipScale = drawData->FramebufferScale; //(1,1) unless using retina display which are often (2,2)

	//Render command lists
	for(int32_t n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmdList = drawData->CmdLists[n];

		//Upload vertex/index buffers
		glNamedBufferData(g_VboHandle, static_cast<GLsizeiptr>(cmdList->VtxBuffer.Size) * sizeof(ImDrawVert), static_cast<const GLvoid*>(cmdList->VtxBuffer.Data), GL_STREAM_DRAW);
		glNamedBufferData(g_ElementsHandle, static_cast<GLsizeiptr>(cmdList->IdxBuffer.Size) * sizeof(ImDrawIdx), static_cast<const GLvoid*>(cmdList->IdxBuffer.Data), GL_STREAM_DRAW);

		for(int32_t cmdI = 0; cmdI < cmdList->CmdBuffer.Size; cmdI++)
		{
			const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmdI];
			if(pcmd->UserCallback != nullptr)
			{
				//User callback, registered via ImDrawList::AddCallback()
				//(ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
				if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
					ImGuiOpenGL4SetupRenderState(drawData, fbWidth, fbHeight, vertexArrayObject);
				else
					pcmd->UserCallback(cmdList, pcmd);
			}
			else
			{
				//Project scissor/clipping rectangles into framebuffer space
				ImVec4 clipRect;
				clipRect.x = (pcmd->ClipRect.x - clipOff.x) * clipScale.x;
				clipRect.y = (pcmd->ClipRect.y - clipOff.y) * clipScale.y;
				clipRect.z = (pcmd->ClipRect.z - clipOff.x) * clipScale.x;
				clipRect.w = (pcmd->ClipRect.w - clipOff.y) * clipScale.y;

				if(clipRect.x < fbWidth && clipRect.y < fbHeight && clipRect.z >= 0.0f && clipRect.w >= 0.0f)
				{
					//Apply scissor/clipping rectangle
					if (clipOriginLowerLeft)
						glScissor(static_cast<int32_t>(clipRect.x), static_cast<int32_t>(fbHeight - clipRect.w), static_cast<int32_t>(clipRect.z - clipRect.x), static_cast<int32_t>(clipRect.w - clipRect.y));
					else
						glScissor(static_cast<int32_t>(clipRect.x), static_cast<int32_t>(clipRect.y), static_cast<int32_t>(clipRect.z), static_cast<int32_t>(clipRect.w)); //Support for OpenGL 4.5 rarely used glClipControl(GL_UPPER_LEFT);

					//Bind VAO, texture, Draw
					glBindVertexArray(vertexArrayObject);
					glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(reinterpret_cast<intptr_t>(pcmd->TextureId)));
					glDrawElementsBaseVertex(GL_TRIANGLES,
					                         static_cast<GLsizei>(pcmd->ElemCount),
					                         sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
					                         reinterpret_cast<void*>(static_cast<intptr_t>(pcmd->IdxOffset * sizeof(
						                         ImDrawIdx))),
					                         static_cast<GLint>(pcmd->VtxOffset));
				}
			}
		}
	}

	//Destroy the temporary VAO
	glDeleteVertexArrays(1, &vertexArrayObject);

	//Restore modified OpenGL state
	glUseProgram(lastProgram);
	glBindTexture(GL_TEXTURE_2D, lastTexture);
	glBindSampler(0, lastSampler);
	glActiveTexture(lastActiveTexture);
	glBindVertexArray(lastVertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, lastArrayBuffer);
	glBlendEquationSeparate(lastBlendEquationRGB, lastBlendEquationAlpha);
	glBlendFuncSeparate(lastBlendSRCRGB, lastBlendDSTRGB, lastBlendSRCAlpha, lastBlendDSTAlpha);
	if (lastEnableBlend)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
	if (lastEnableCullFace)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	if (lastEnableDepthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	if (lastEnableScissorTest)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(lastPolygonMode[0]));
	glViewport(lastViewport[0], lastViewport[1], static_cast<GLsizei>(lastViewport[2]), static_cast<GLsizei>(lastViewport[3]));
	glScissor(lastScissorBox[0], lastScissorBox[1], static_cast<GLsizei>(lastScissorBox[2]), static_cast<GLsizei>(lastScissorBox[3]));
}

//-------------------------------------------------------------------------------------------------------------------//

bool ImGuiOpenGL4CreateFontsTexture()
{
	TP_PROFILE_FUNCTION();

	//Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	uint8_t* pixels = nullptr;
	int32_t width = 0, height = 0;
	//Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders.
	//If your ImTextureId represent a higher-level concept than just an OpenGL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	//Upload texture to graphics system
	GLint lastTexture = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
	glCreateTextures(GL_TEXTURE_2D, 1, &g_FontTexture);
	glTextureParameteri(g_FontTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(g_FontTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTextureStorage2D(g_FontTexture, 1, GL_RGBA8, width, height);
	glTextureSubImage2D(g_FontTexture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	//Store our identifier
	io.Fonts->TexID = reinterpret_cast<ImTextureID>(static_cast<intptr_t>(g_FontTexture));

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGuiOpenGL4DestroyFontsTexture()
{
	TP_PROFILE_FUNCTION();

	if(g_FontTexture)
	{
		ImGuiIO& io = ImGui::GetIO();
		glDeleteTextures(1, &g_FontTexture);
		io.Fonts->TexID = nullptr;
		g_FontTexture = 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

static bool CheckShader(const GLuint handle, const char* desc)
{
	TP_PROFILE_FUNCTION();

	GLint status = 0, logLength = 0;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLength);
	if (static_cast<GLboolean>(status) == GL_FALSE)
		TP_ERROR(TRAP::Log::ImGuiOpenGLPrefix, "Failed to compile shader: ", desc);
	if(logLength > 1)
	{
		ImVector<char> buf;
		buf.resize(static_cast<int32_t>(logLength + 1));
		glGetShaderInfoLog(handle, logLength, nullptr, static_cast<GLchar*>(buf.begin()));
		TP_ERROR(buf.begin());
	}

	return static_cast<GLboolean>(status) == GL_TRUE;
}

//-------------------------------------------------------------------------------------------------------------------//

static bool CheckProgram(const GLuint handle, const char* desc)
{
	TP_PROFILE_FUNCTION();

	GLint status = 0, logLength = 0;
	glGetProgramiv(handle, GL_LINK_STATUS, &status);
	glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &logLength);
	if (static_cast<GLboolean>(status) == GL_FALSE)
		TP_ERROR(TRAP::Log::ImGuiOpenGLPrefix, "Failed to link program: ", desc);
	if (logLength > 1)
	{
		ImVector<char> buf;
		buf.resize(static_cast<int32_t>(logLength + 1));
		glGetProgramInfoLog(handle, logLength, nullptr, static_cast<GLchar*>(buf.begin()));
		TP_ERROR(buf.begin());
	}

	return static_cast<GLboolean>(status) == GL_TRUE;
}

//-------------------------------------------------------------------------------------------------------------------//

bool ImGuiOpenGL4CreateDeviceObjects()
{
	TP_PROFILE_FUNCTION();

	GLint lastTexture = 0;
	GLint lastArrayBuffer = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &lastArrayBuffer);
	GLint lastVertexArray = 0;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &lastVertexArray);

	const GLchar* vertexShader = "#version 460 core\n"
								 "layout (location = 0) in vec2 Position;\n"
								 "layout (location = 1) in vec2 UV;\n"
								 "layout (location = 2) in vec4 Color;\n"
								 "uniform mat4 ProjMtx;\n"
								 "out vec2 Frag_UV;\n"
								 "out vec4 Frag_Color;\n"
								 "void main()\n"
								 "{\n"
								 "	Frag_UV = UV;\n"
								 "	Frag_Color = Color;\n"
								 "	gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
								 "}\n";
	const GLchar* fragmentShader = "#version 460 core\n"
								   "in vec2 Frag_UV;\n"
								   "in vec4 Frag_Color;\n"
								   "uniform sampler2D Texture;\n"
								   "layout (location = 0) out vec4 Out_Color;\n"
								   "void main()\n"
								   "{\n"
								   "	Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
								   "}\n";

	//Create shaders
	g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(g_VertHandle, 1, &vertexShader, nullptr);
	glCompileShader(g_VertHandle);
	CheckShader(g_VertHandle, "Vertex Shader");

	g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(g_FragHandle, 1, &fragmentShader, nullptr);
	glCompileShader(g_FragHandle);
	CheckShader(g_FragHandle, "Fragment Shader");

	g_ShaderHandle = glCreateProgram();
	glAttachShader(g_ShaderHandle, g_VertHandle);
	glAttachShader(g_ShaderHandle, g_FragHandle);
	glLinkProgram(g_ShaderHandle);
	CheckProgram(g_ShaderHandle, "Shader Program");

	g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
	g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
	g_AttribLocationVtxPos = glGetAttribLocation(g_ShaderHandle, "Position");
	g_AttribLocationVtxUV = glGetAttribLocation(g_ShaderHandle, "UV");
	g_AttribLocationVtxColor = glGetAttribLocation(g_ShaderHandle, "Color");

	//Create buffers
	glCreateBuffers(1, &g_VboHandle);
	glCreateBuffers(1, &g_ElementsHandle);

	ImGuiOpenGL4CreateFontsTexture();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGuiOpenGL4DestroyDeviceObjects()
{
	TP_PROFILE_FUNCTION();

	if(g_VboHandle)
	{
		glDeleteBuffers(1, &g_VboHandle);
		g_VboHandle = 0;
	}
	if(g_ElementsHandle)
	{
		glDeleteBuffers(1, &g_ElementsHandle);
		g_ElementsHandle = 0;
	}
	if(g_ShaderHandle && g_VertHandle)
		glDetachShader(g_ShaderHandle, g_VertHandle);
	if(g_ShaderHandle && g_FragHandle)		
		glDetachShader(g_ShaderHandle, g_FragHandle);
	if(g_VertHandle)
	{
		glDeleteShader(g_VertHandle);
		g_VertHandle = 0;
	}
	if(g_FragHandle)
	{
		glDeleteShader(g_FragHandle);
		g_FragHandle = 0;
	}
	if(g_ShaderHandle)
	{
		glDeleteProgram(g_ShaderHandle);
		g_ShaderHandle = 0;
	}

	ImGuiOpenGL4DestroyFontsTexture();
}

//-------------------------------------------------------------------------------------------------------------------//
//MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
//This is a _advanced_ and _optional_ feature, allowing the back-end to create and handle multiple viewports simultaneously.
//If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first...
//-------------------------------------------------------------------------------------------------------------------//

static void ImGuiOpenGL4RenderWindow(ImGuiViewport* viewport, void*)
{
	TP_PROFILE_FUNCTION();

	if(!(viewport->Flags & ImGuiViewportFlags_NoRendererClear))
	{
		const ImVec4 clearColor = ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f };
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	
	ImGuiOpenGL4RenderDrawData(viewport->DrawData);
}

//-------------------------------------------------------------------------------------------------------------------//

static void ImGuiOpenGL4InitPlatformInterface()
{
	TP_PROFILE_FUNCTION();

	ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
	platformIO.Renderer_RenderWindow = ImGuiOpenGL4RenderWindow;
}

//-------------------------------------------------------------------------------------------------------------------//

static void ImGuiOpenGL4ShutdownPlatformInterface()
{
	TP_PROFILE_FUNCTION();

	ImGui::DestroyPlatformWindows();
}