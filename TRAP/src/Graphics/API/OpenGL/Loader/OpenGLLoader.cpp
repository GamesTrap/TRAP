/*
The MIT License (MIT)

Copyright (c) 2013-2018 David Herberth

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Modified by: Jan "GamesTrap" Schuerkamp
*/

#include "TRAPPCH.h"
#include "OpenGLLoader.h"
#include "Utils/String/String.h"

static void* GetProc(const char* names);

#if defined(_WIN32) || defined(TRAP_PLATFORM_WINDOWS) || defined(__CYGWIN__)
	#ifndef _WINDOWS_
		#undef APIENTRY
	#endif
	static HMODULE libGL;

	typedef void* (APIENTRYP PFNWGLGETPROCADDRESSPROC_PRIVATE)(const char*);
	static PFNWGLGETPROCADDRESSPROC_PRIVATE TRAPGetProcAddressPtr;

	//-------------------------------------------------------------------------------------------------------------------//

	static bool OpenGL()
	{
		TP_PROFILE_FUNCTION();
		
		libGL = LoadLibraryW(L"opengl32.dll");
		if(libGL != nullptr)
		{
			void (*tmp)() = reinterpret_cast<void(*)()>(GetProcAddress(libGL, "wglGetProcAddress"));
			TRAPGetProcAddressPtr = reinterpret_cast<PFNWGLGETPROCADDRESSPROC_PRIVATE>(tmp);
			return TRAPGetProcAddressPtr != nullptr;
		}

		return false;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	static void CloseGL()
	{
		TP_PROFILE_FUNCTION();
		
		if(libGL != nullptr)
		{
			FreeLibrary(static_cast<HMODULE>(libGL));
			libGL = nullptr;
		}
	}
#else
	static void* libGL;

	#if !defined(__APPLE__) && !defined(__HAIKU__)
		typedef void* (APIENTRYP PFNGLXGETPROCADDRESSPROC_PRIVATE)(const char*);
		static PFNGLXGETPROCADDRESSPROC_PRIVATE TRAPGetProcAddressPtr;
	#endif

	//-------------------------------------------------------------------------------------------------------------------//

	static bool OpenGL()
	{
		TP_PROFILE_FUNCTION();
		
		static const std::array<std::string, 2> NAMES = { "libGL.so.1", "libGL.so" };

		uint32_t index = 0;
		for(index = 0; index < NAMES.size(); index++)
		{
			libGL = dlopen(NAMES[index].c_str(), RTLD_NOW | RTLD_GLOBAL);

			if(libGL != nullptr)
			{
				TRAPGetProcAddressPtr = (PFNGLXGETPROCADDRESSPROC_PRIVATE)dlsym(libGL, "glXGetProcAddressARB");
				return TRAPGetProcAddressPtr != nullptr;
			}
		}

		return false;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	static void CloseGL()
	{
		TP_PROFILE_FUNCTION();
		
		if(libGL != nullptr)
		{
			dlclose(libGL);
			libGL = nullptr;
		}
	}
#endif

//-------------------------------------------------------------------------------------------------------------------//

static void* GetProc(const char* names)
{
	TP_PROFILE_FUNCTION();
		
	void* result = nullptr;
	if (libGL == nullptr)
		return nullptr;

#if !defined(__APPLE__) && !defined(__HAIKU__)
	if (TRAPGetProcAddressPtr != nullptr)
		result = TRAPGetProcAddressPtr(names);
#endif
	if(result == nullptr)
	{
	#if defined(_WIN32) || defined(TRAP_PLATFORM_WINDOWS) || defined(__CYGWIN__)
		result = static_cast<void*>(GetProcAddress(static_cast<HMODULE>(libGL), names));
	#else
		result = dlsym(libGL, names);
	#endif
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAPLoadOpenGL()
{
	TP_PROFILE_FUNCTION();
		
	bool status = false;
	if(OpenGL())
	{
		status = TRAPLoadOpenGLLoader(&GetProc);
		CloseGL();
	}

	return status;
}

//-------------------------------------------------------------------------------------------------------------------//

struct TRAPOpenGLVersionStruct OpenGLVersion { 0, 0 };

static int32_t MaxLoadedMajor;
static int32_t MaxLoadedMinor;

static std::vector<std::string> ExtensionsI = {};
static const char* Extensions = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

static bool GetExtensions()
{
	TP_PROFILE_FUNCTION();
	
	if(MaxLoadedMajor < 3)
		Extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
	else
	{
		int32_t numExtensions;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

		for(uint32_t index = 0; index < static_cast<uint32_t>(numExtensions); index++)
		{
			const char* openGLStrTmp = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, index));
			ExtensionsI.emplace_back(openGLStrTmp);
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

static bool HasExtension(const std::string& extension)
{
	TP_PROFILE_FUNCTION();
	
	if(MaxLoadedMajor < 3)
	{
		const char* localExtensions = Extensions;
		if (localExtensions == nullptr)
			return false;

		while(true)
		{
			const char* loc = std::strstr(localExtensions, extension.c_str());
			if (loc == nullptr)
				return false;

			const char* terminator = loc + std::strlen(extension.c_str());
			if ((loc == localExtensions || *(loc - 1) == ' ') && (*terminator == ' ' || *terminator == '\0'))
				return true;
			localExtensions = terminator;
		}
	}
	
	if (ExtensionsI.empty())
		return false;
	for (auto& e : ExtensionsI)
		if (e == extension)
			return true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP_OpenGL_VERSION_1_0 = 0;
int32_t TRAP_OpenGL_VERSION_1_1 = 0;
int32_t TRAP_OpenGL_VERSION_1_2 = 0;
int32_t TRAP_OpenGL_VERSION_1_3 = 0;
int32_t TRAP_OpenGL_VERSION_1_4 = 0;
int32_t TRAP_OpenGL_VERSION_1_5 = 0;
int32_t TRAP_OpenGL_VERSION_2_0 = 0;
int32_t TRAP_OpenGL_VERSION_2_1 = 0;
int32_t TRAP_OpenGL_VERSION_3_0 = 0;
int32_t TRAP_OpenGL_VERSION_3_1 = 0;
int32_t TRAP_OpenGL_VERSION_3_2 = 0;
int32_t TRAP_OpenGL_VERSION_3_3 = 0;
int32_t TRAP_OpenGL_VERSION_4_0 = 0;
int32_t TRAP_OpenGL_VERSION_4_1 = 0;
int32_t TRAP_OpenGL_VERSION_4_2 = 0;
int32_t TRAP_OpenGL_VERSION_4_3 = 0;
int32_t TRAP_OpenGL_VERSION_4_4 = 0;
int32_t TRAP_OpenGL_VERSION_4_5 = 0;
int32_t TRAP_OpenGL_VERSION_4_6 = 0;
PFNGLACTIVESHADERPROGRAMPROC glActiveShaderProgram = nullptr; 
PFNGLACTIVETEXTUREPROC glActiveTexture = nullptr; 
PFNGLATTACHSHADERPROC glAttachShader = nullptr; 
PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender = nullptr; 
PFNGLBEGINQUERYPROC glBeginQuery = nullptr; 
PFNGLBEGINQUERYINDEXEDPROC glBeginQueryIndexed = nullptr; 
PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback = nullptr; 
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = nullptr; 
PFNGLBINDBUFFERPROC glBindBuffer = nullptr; 
PFNGLBINDBUFFERBASEPROC glBindBufferBase = nullptr; 
PFNGLBINDBUFFERRANGEPROC glBindBufferRange = nullptr; 
PFNGLBINDBUFFERSBASEPROC glBindBuffersBase = nullptr; 
PFNGLBINDBUFFERSRANGEPROC glBindBuffersRange = nullptr; 
PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation = nullptr; 
PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed = nullptr; 
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = nullptr; 
PFNGLBINDIMAGETEXTUREPROC glBindImageTexture = nullptr; 
PFNGLBINDIMAGETEXTURESPROC glBindImageTextures = nullptr; 
PFNGLBINDPROGRAMPIPELINEPROC glBindProgramPipeline = nullptr; 
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = nullptr; 
PFNGLBINDSAMPLERPROC glBindSampler = nullptr; 
PFNGLBINDSAMPLERSPROC glBindSamplers = nullptr; 
PFNGLBINDTEXTUREPROC glBindTexture = nullptr; 
PFNGLBINDTEXTUREUNITPROC glBindTextureUnit = nullptr; 
PFNGLBINDTEXTURESPROC glBindTextures = nullptr; 
PFNGLBINDTRANSFORMFEEDBACKPROC glBindTransformFeedback = nullptr; 
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr; 
PFNGLBLENDCOLORPROC glBlendColor = nullptr; 
PFNGLBLENDEQUATIONPROC glBlendEquation = nullptr; 
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate = nullptr; 
PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei = nullptr; 
PFNGLBLENDEQUATIONIPROC glBlendEquationi = nullptr; 
PFNGLBLENDFUNCPROC glBlendFunc = nullptr; 
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = nullptr; 
PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei = nullptr; 
PFNGLBLENDFUNCIPROC glBlendFunci = nullptr; 
PFNGLBLITNAMEDFRAMEBUFFERPROC glBlitNamedFramebuffer = nullptr; 
PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glCheckNamedFramebufferStatus = nullptr; 
PFNGLCLAMPCOLORPROC glClampColor = nullptr; 
PFNGLCLEARPROC glClear = nullptr; 
PFNGLCLEARCOLORPROC glClearColor = nullptr; 
PFNGLCLEARDEPTHPROC glClearDepth = nullptr; 
PFNGLCLEARDEPTHFPROC glClearDepthf = nullptr; 
PFNGLCLEARNAMEDBUFFERDATAPROC glClearNamedBufferData = nullptr; 
PFNGLCLEARNAMEDBUFFERSUBDATAPROC glClearNamedBufferSubData = nullptr; 
PFNGLCLEARNAMEDFRAMEBUFFERFIPROC glClearNamedFramebufferfi = nullptr; 
PFNGLCLEARNAMEDFRAMEBUFFERFVPROC glClearNamedFramebufferfv = nullptr; 
PFNGLCLEARNAMEDFRAMEBUFFERIVPROC glClearNamedFramebufferiv = nullptr; 
PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC glClearNamedFramebufferuiv = nullptr; 
PFNGLCLEARSTENCILPROC glClearStencil = nullptr; 
PFNGLCLEARTEXIMAGEPROC glClearTexImage = nullptr; 
PFNGLCLEARTEXSUBIMAGEPROC glClearTexSubImage = nullptr; 
PFNGLCLIENTWAITSYNCPROC glClientWaitSync = nullptr; 
PFNGLCLIPCONTROLPROC glClipControl = nullptr; 
PFNGLCOLORMASKPROC glColorMask = nullptr; 
PFNGLCOLORMASKIPROC glColorMaski = nullptr; 
PFNGLCOLORP3UIPROC glColorP3ui = nullptr;
PFNGLCOLORP3UIVPROC glColorP3uiv = nullptr;
PFNGLCOLORP4UIPROC glColorP4ui = nullptr;
PFNGLCOLORP4UIVPROC glColorP4uiv = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr; 
PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D = nullptr; 
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D = nullptr; 
PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D = nullptr; 
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D = nullptr; 
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D = nullptr; 
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D = nullptr; 
PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC glCompressedTextureSubImage1D = nullptr;
PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC glCompressedTextureSubImage2D = nullptr;
PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC glCompressedTextureSubImage3D = nullptr;
PFNGLCOPYIMAGESUBDATAPROC glCopyImageSubData = nullptr; 
PFNGLCOPYNAMEDBUFFERSUBDATAPROC glCopyNamedBufferSubData = nullptr; 
PFNGLCOPYTEXIMAGE1DPROC glCopyTexImage1D = nullptr; 
PFNGLCOPYTEXIMAGE2DPROC glCopyTexImage2D = nullptr; 
PFNGLCOPYTEXSUBIMAGE1DPROC glCopyTexSubImage1D = nullptr; 
PFNGLCOPYTEXSUBIMAGE2DPROC glCopyTexSubImage2D = nullptr; 
PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D = nullptr; 
PFNGLCOPYTEXTURESUBIMAGE1DPROC glCopyTextureSubImage1D = nullptr;
PFNGLCOPYTEXTURESUBIMAGE2DPROC glCopyTextureSubImage2D = nullptr;
PFNGLCOPYTEXTURESUBIMAGE3DPROC glCopyTextureSubImage3D = nullptr;
PFNGLCREATEBUFFERSPROC glCreateBuffers = nullptr; 
PFNGLCREATEFRAMEBUFFERSPROC glCreateFramebuffers = nullptr; 
PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr; 
PFNGLCREATEPROGRAMPIPELINESPROC glCreateProgramPipelines = nullptr; 
PFNGLCREATEQUERIESPROC glCreateQueries = nullptr; 
PFNGLCREATERENDERBUFFERSPROC glCreateRenderbuffers = nullptr; 
PFNGLCREATESAMPLERSPROC glCreateSamplers = nullptr; 
PFNGLCREATESHADERPROC glCreateShader = nullptr; 
PFNGLCREATESHADERPROGRAMVPROC glCreateShaderProgramv = nullptr; 
PFNGLCREATETEXTURESPROC glCreateTextures = nullptr; 
PFNGLCREATETRANSFORMFEEDBACKSPROC glCreateTransformFeedbacks = nullptr; 
PFNGLCREATEVERTEXARRAYSPROC glCreateVertexArrays = nullptr; 
PFNGLCULLFACEPROC glCullFace = nullptr; 
PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback = nullptr; 
PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl = nullptr; 
PFNGLDEBUGMESSAGEINSERTPROC glDebugMessageInsert = nullptr; 
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr; 
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = nullptr; 
PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr; 
PFNGLDELETEPROGRAMPIPELINESPROC glDeleteProgramPipelines = nullptr; 
PFNGLDELETEQUERIESPROC glDeleteQueries = nullptr; 
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = nullptr; 
PFNGLDELETESAMPLERSPROC glDeleteSamplers = nullptr; 
PFNGLDELETESHADERPROC glDeleteShader = nullptr; 
PFNGLDELETESYNCPROC glDeleteSync = nullptr; 
PFNGLDELETETEXTURESPROC glDeleteTextures = nullptr; 
PFNGLDELETETRANSFORMFEEDBACKSPROC glDeleteTransformFeedbacks = nullptr; 
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr; 
PFNGLDEPTHFUNCPROC glDepthFunc = nullptr; 
PFNGLDEPTHMASKPROC glDepthMask = nullptr; 
PFNGLDEPTHRANGEPROC glDepthRange = nullptr; 
PFNGLDEPTHRANGEARRAYVPROC glDepthRangeArrayv = nullptr; 
PFNGLDEPTHRANGEINDEXEDPROC glDepthRangeIndexed = nullptr; 
PFNGLDEPTHRANGEFPROC glDepthRangef = nullptr; 
PFNGLDETACHSHADERPROC glDetachShader = nullptr; 
PFNGLDISABLEPROC glDisable = nullptr; 
PFNGLDISABLEVERTEXARRAYATTRIBPROC glDisableVertexArrayAttrib = nullptr; 
PFNGLDISABLEIPROC glDisablei = nullptr; 
PFNGLDISPATCHCOMPUTEPROC glDispatchCompute = nullptr; 
PFNGLDISPATCHCOMPUTEINDIRECTPROC glDispatchComputeIndirect = nullptr; 
PFNGLDRAWARRAYSPROC glDrawArrays = nullptr; 
PFNGLDRAWARRAYSINDIRECTPROC glDrawArraysIndirect = nullptr; 
PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced = nullptr; 
PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC glDrawArraysInstancedBaseInstance = nullptr; 
PFNGLDRAWELEMENTSPROC glDrawElements = nullptr; 
PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex = nullptr; 
PFNGLDRAWELEMENTSINDIRECTPROC glDrawElementsIndirect = nullptr; 
PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced = nullptr; 
PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC glDrawElementsInstancedBaseInstance = nullptr; 
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex = nullptr; 
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance = nullptr; 
PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements = nullptr; 
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex = nullptr; 
PFNGLDRAWTRANSFORMFEEDBACKPROC glDrawTransformFeedback = nullptr; 
PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC glDrawTransformFeedbackInstanced = nullptr; 
PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glDrawTransformFeedbackStream = nullptr; 
PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC glDrawTransformFeedbackStreamInstanced = nullptr; 
PFNGLENABLEPROC glEnable = nullptr; 
PFNGLENABLEVERTEXARRAYATTRIBPROC glEnableVertexArrayAttrib = nullptr; 
PFNGLENABLEIPROC glEnablei = nullptr; 
PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender = nullptr; 
PFNGLENDQUERYPROC glEndQuery = nullptr; 
PFNGLENDQUERYINDEXEDPROC glEndQueryIndexed = nullptr; 
PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback = nullptr; 
PFNGLFENCESYNCPROC glFenceSync = nullptr; 
PFNGLFINISHPROC glFinish = nullptr; 
PFNGLFLUSHPROC glFlush = nullptr; 
PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC glFlushMappedNamedBufferRange = nullptr; 
PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D = nullptr; 
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = nullptr; 
PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D = nullptr; 
PFNGLFRONTFACEPROC glFrontFace = nullptr; 
PFNGLGENBUFFERSPROC glGenBuffers = nullptr; 
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = nullptr; 
PFNGLGENPROGRAMPIPELINESPROC glGenProgramPipelines = nullptr; 
PFNGLGENQUERIESPROC glGenQueries = nullptr; 
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = nullptr; 
PFNGLGENSAMPLERSPROC glGenSamplers = nullptr; 
PFNGLGENTEXTURESPROC glGenTextures = nullptr; 
PFNGLGENTRANSFORMFEEDBACKSPROC glGenTransformFeedbacks = nullptr; 
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr; 
PFNGLGENERATETEXTUREMIPMAPPROC glGenerateTextureMipmap = nullptr; 
PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC glGetActiveAtomicCounterBufferiv = nullptr; 
PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib = nullptr; 
PFNGLGETACTIVESUBROUTINENAMEPROC glGetActiveSubroutineName = nullptr; 
PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glGetActiveSubroutineUniformName = nullptr; 
PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC glGetActiveSubroutineUniformiv = nullptr; 
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform = nullptr; 
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName = nullptr;  
PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv = nullptr; 
PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName = nullptr; 
PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv = nullptr; 
PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders = nullptr; 
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = nullptr; 
PFNGLGETBOOLEANI_VPROC glGetBooleani_v = nullptr; 
PFNGLGETBOOLEANVPROC glGetBooleanv = nullptr; 
PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC glGetCompressedTextureImage = nullptr; 
PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC glGetCompressedTextureSubImage = nullptr; 
PFNGLGETDEBUGMESSAGELOGPROC glGetDebugMessageLog = nullptr; 
PFNGLGETDOUBLEI_VPROC glGetDoublei_v = nullptr; 
PFNGLGETDOUBLEVPROC glGetDoublev = nullptr; 
PFNGLGETERRORPROC glGetError = nullptr; 
PFNGLGETFLOATI_VPROC glGetFloati_v = nullptr; 
PFNGLGETFLOATVPROC glGetFloatv = nullptr; 
PFNGLGETFRAGDATAINDEXPROC glGetFragDataIndex = nullptr; 
PFNGLGETFRAGDATALOCATIONPROC glGetFragDataLocation = nullptr; 
PFNGLGETGRAPHICSRESETSTATUSPROC glGetGraphicsResetStatus = nullptr; 
PFNGLGETINTEGER64I_VPROC glGetInteger64i_v = nullptr; 
PFNGLGETINTEGER64VPROC glGetInteger64v = nullptr; 
PFNGLGETINTEGERI_VPROC glGetIntegeri_v = nullptr; 
PFNGLGETINTEGERVPROC glGetIntegerv = nullptr; 
PFNGLGETINTERNALFORMATI64VPROC glGetInternalformati64v = nullptr; 
PFNGLGETINTERNALFORMATIVPROC glGetInternalformativ = nullptr; 
PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv = nullptr; 
PFNGLGETNAMEDBUFFERPARAMETERI64VPROC glGetNamedBufferParameteri64v = nullptr; 
PFNGLGETNAMEDBUFFERPARAMETERIVPROC glGetNamedBufferParameteriv = nullptr; 
PFNGLGETNAMEDBUFFERPOINTERVPROC glGetNamedBufferPointerv = nullptr; 
PFNGLGETNAMEDBUFFERSUBDATAPROC glGetNamedBufferSubData = nullptr; 
PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetNamedFramebufferAttachmentParameteriv = nullptr; 
PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC glGetNamedFramebufferParameteriv = nullptr; 
PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC glGetNamedRenderbufferParameteriv = nullptr; 
PFNGLGETOBJECTLABELPROC glGetObjectLabel = nullptr; 
PFNGLGETOBJECTPTRLABELPROC glGetObjectPtrLabel = nullptr; 
PFNGLGETPOINTERVPROC glGetPointerv = nullptr; 
PFNGLGETPROGRAMBINARYPROC glGetProgramBinary = nullptr; 
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr; 
PFNGLGETPROGRAMINTERFACEIVPROC glGetProgramInterfaceiv = nullptr; 
PFNGLGETPROGRAMPIPELINEINFOLOGPROC glGetProgramPipelineInfoLog = nullptr; 
PFNGLGETPROGRAMPIPELINEIVPROC glGetProgramPipelineiv = nullptr; 
PFNGLGETPROGRAMRESOURCEINDEXPROC glGetProgramResourceIndex = nullptr; 
PFNGLGETPROGRAMRESOURCELOCATIONPROC glGetProgramResourceLocation = nullptr; 
PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC glGetProgramResourceLocationIndex = nullptr; 
PFNGLGETPROGRAMRESOURCENAMEPROC glGetProgramResourceName = nullptr; 
PFNGLGETPROGRAMRESOURCEIVPROC glGetProgramResourceiv = nullptr; 
PFNGLGETPROGRAMSTAGEIVPROC glGetProgramStageiv = nullptr; 
PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr; 
PFNGLGETQUERYBUFFEROBJECTI64VPROC glGetQueryBufferObjecti64v = nullptr;
PFNGLGETQUERYBUFFEROBJECTIVPROC glGetQueryBufferObjectiv = nullptr;
PFNGLGETQUERYBUFFEROBJECTUI64VPROC glGetQueryBufferObjectui64v = nullptr;
PFNGLGETQUERYBUFFEROBJECTUIVPROC glGetQueryBufferObjectuiv = nullptr;
PFNGLGETQUERYINDEXEDIVPROC glGetQueryIndexediv = nullptr; 
PFNGLGETQUERYOBJECTI64VPROC glGetQueryObjecti64v = nullptr; 
PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv = nullptr; 
PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v = nullptr; 
PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv = nullptr; 
PFNGLGETQUERYIVPROC glGetQueryiv = nullptr; 
PFNGLGETSAMPLERPARAMETERIIVPROC glGetSamplerParameterIiv = nullptr; 
PFNGLGETSAMPLERPARAMETERIUIVPROC glGetSamplerParameterIuiv = nullptr; 
PFNGLGETSAMPLERPARAMETERFVPROC glGetSamplerParameterfv = nullptr; 
PFNGLGETSAMPLERPARAMETERIVPROC glGetSamplerParameteriv = nullptr; 
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr; 
PFNGLGETSHADERPRECISIONFORMATPROC glGetShaderPrecisionFormat = nullptr; 
PFNGLGETSHADERSOURCEPROC glGetShaderSource = nullptr; 
PFNGLGETSHADERIVPROC glGetShaderiv = nullptr; 
PFNGLGETSTRINGPROC glGetString = nullptr; 
PFNGLGETSTRINGIPROC glGetStringi = nullptr; 
PFNGLGETSUBROUTINEINDEXPROC glGetSubroutineIndex = nullptr; 
PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC glGetSubroutineUniformLocation = nullptr;  
PFNGLGETSYNCIVPROC glGetSynciv = nullptr; 
PFNGLGETTEXTUREIMAGEPROC glGetTextureImage = nullptr; 
PFNGLGETTEXTURELEVELPARAMETERFVPROC glGetTextureLevelParameterfv = nullptr; 
PFNGLGETTEXTURELEVELPARAMETERIVPROC glGetTextureLevelParameteriv = nullptr; 
PFNGLGETTEXTUREPARAMETERIIVPROC glGetTextureParameterIiv = nullptr; 
PFNGLGETTEXTUREPARAMETERIUIVPROC glGetTextureParameterIuiv = nullptr; 
PFNGLGETTEXTUREPARAMETERFVPROC glGetTextureParameterfv = nullptr; 
PFNGLGETTEXTUREPARAMETERIVPROC glGetTextureParameteriv = nullptr; 
PFNGLGETTEXTURESUBIMAGEPROC glGetTextureSubImage = nullptr; 
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying = nullptr; 
PFNGLGETTRANSFORMFEEDBACKI64_VPROC glGetTransformFeedbacki64_v = nullptr; 
PFNGLGETTRANSFORMFEEDBACKI_VPROC glGetTransformFeedbacki_v = nullptr; 
PFNGLGETTRANSFORMFEEDBACKIVPROC glGetTransformFeedbackiv = nullptr; 
PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex = nullptr; 
PFNGLGETUNIFORMINDICESPROC glGetUniformIndices = nullptr; 
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr; 
PFNGLGETUNIFORMSUBROUTINEUIVPROC glGetUniformSubroutineuiv = nullptr; 
PFNGLGETUNIFORMDVPROC glGetUniformdv = nullptr; 
PFNGLGETUNIFORMFVPROC glGetUniformfv = nullptr; 
PFNGLGETUNIFORMIVPROC glGetUniformiv = nullptr; 
PFNGLGETUNIFORMUIVPROC glGetUniformuiv = nullptr; 
PFNGLGETVERTEXARRAYINDEXED64IVPROC glGetVertexArrayIndexed64iv = nullptr; 
PFNGLGETVERTEXARRAYINDEXEDIVPROC glGetVertexArrayIndexediv = nullptr; 
PFNGLGETVERTEXARRAYIVPROC glGetVertexArrayiv = nullptr; 
PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv = nullptr; 
PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv = nullptr; 
PFNGLGETVERTEXATTRIBLDVPROC glGetVertexAttribLdv = nullptr; 
PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv = nullptr; 
PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv = nullptr; 
PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv = nullptr; 
PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv = nullptr; 
PFNGLGETNCOLORTABLEPROC glGetnColorTable = nullptr;
PFNGLGETNCONVOLUTIONFILTERPROC glGetnConvolutionFilter = nullptr;
PFNGLGETNHISTOGRAMPROC glGetnHistogram = nullptr;
PFNGLGETNMAPDVPROC glGetnMapdv = nullptr;
PFNGLGETNMAPFVPROC glGetnMapfv = nullptr;
PFNGLGETNMAPIVPROC glGetnMapiv = nullptr;
PFNGLGETNMINMAXPROC glGetnMinmax = nullptr;
PFNGLGETNPIXELMAPFVPROC glGetnPixelMapfv = nullptr;
PFNGLGETNPIXELMAPUIVPROC glGetnPixelMapuiv = nullptr;
PFNGLGETNPIXELMAPUSVPROC glGetnPixelMapusv = nullptr;
PFNGLGETNPOLYGONSTIPPLEPROC glGetnPolygonStipple = nullptr;
PFNGLGETNSEPARABLEFILTERPROC glGetnSeparableFilter = nullptr;
PFNGLGETNUNIFORMDVPROC glGetnUniformdv = nullptr; 
PFNGLGETNUNIFORMFVPROC glGetnUniformfv = nullptr; 
PFNGLGETNUNIFORMIVPROC glGetnUniformiv = nullptr; 
PFNGLGETNUNIFORMUIVPROC glGetnUniformuiv = nullptr; 
PFNGLHINTPROC glHint = nullptr; 
PFNGLINVALIDATEBUFFERDATAPROC glInvalidateBufferData = nullptr; 
PFNGLINVALIDATEBUFFERSUBDATAPROC glInvalidateBufferSubData = nullptr; 
PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC glInvalidateNamedFramebufferData = nullptr; 
PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC glInvalidateNamedFramebufferSubData = nullptr; 
PFNGLINVALIDATETEXIMAGEPROC glInvalidateTexImage = nullptr; 
PFNGLINVALIDATETEXSUBIMAGEPROC glInvalidateTexSubImage = nullptr; 
PFNGLISBUFFERPROC glIsBuffer = nullptr; 
PFNGLISENABLEDPROC glIsEnabled = nullptr; 
PFNGLISENABLEDIPROC glIsEnabledi = nullptr; 
PFNGLISFRAMEBUFFERPROC glIsFramebuffer = nullptr; 
PFNGLISPROGRAMPROC glIsProgram = nullptr; 
PFNGLISPROGRAMPIPELINEPROC glIsProgramPipeline = nullptr; 
PFNGLISQUERYPROC glIsQuery = nullptr; 
PFNGLISRENDERBUFFERPROC glIsRenderbuffer = nullptr; 
PFNGLISSAMPLERPROC glIsSampler = nullptr; 
PFNGLISSHADERPROC glIsShader = nullptr; 
PFNGLISSYNCPROC glIsSync = nullptr; 
PFNGLISTEXTUREPROC glIsTexture = nullptr; 
PFNGLISTRANSFORMFEEDBACKPROC glIsTransformFeedback = nullptr; 
PFNGLISVERTEXARRAYPROC glIsVertexArray = nullptr; 
PFNGLLINEWIDTHPROC glLineWidth = nullptr; 
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr; 
PFNGLLOGICOPPROC glLogicOp = nullptr; 
PFNGLMAPNAMEDBUFFERPROC glMapNamedBuffer = nullptr; 
PFNGLMAPNAMEDBUFFERRANGEPROC glMapNamedBufferRange = nullptr; 
PFNGLMEMORYBARRIERPROC glMemoryBarrier = nullptr; 
PFNGLMEMORYBARRIERBYREGIONPROC glMemoryBarrierByRegion = nullptr; 
PFNGLMINSAMPLESHADINGPROC glMinSampleShading = nullptr; 
PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays = nullptr; 
PFNGLMULTIDRAWARRAYSINDIRECTPROC glMultiDrawArraysIndirect = nullptr; 
PFNGLMULTIDRAWARRAYSINDIRECTCOUNTPROC glMultiDrawArraysIndirectCount = nullptr;
PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements = nullptr; 
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex = nullptr; 
PFNGLMULTIDRAWELEMENTSINDIRECTPROC glMultiDrawElementsIndirect = nullptr; 
PFNGLMULTIDRAWELEMENTSINDIRECTCOUNTPROC glMultiDrawElementsIndirectCount = nullptr;
PFNGLMULTITEXCOORDP1UIPROC glMultiTexCoordP1ui = nullptr;
PFNGLMULTITEXCOORDP1UIVPROC glMultiTexCoordP1uiv = nullptr;
PFNGLMULTITEXCOORDP2UIPROC glMultiTexCoordP2ui = nullptr;
PFNGLMULTITEXCOORDP2UIVPROC glMultiTexCoordP2uiv = nullptr;
PFNGLMULTITEXCOORDP3UIPROC glMultiTexCoordP3ui = nullptr;
PFNGLMULTITEXCOORDP3UIVPROC glMultiTexCoordP3uiv = nullptr;
PFNGLMULTITEXCOORDP4UIPROC glMultiTexCoordP4ui = nullptr;
PFNGLMULTITEXCOORDP4UIVPROC glMultiTexCoordP4uiv = nullptr;
PFNGLNAMEDBUFFERDATAPROC glNamedBufferData = nullptr; 
PFNGLNAMEDBUFFERSTORAGEPROC glNamedBufferStorage = nullptr; 
PFNGLNAMEDBUFFERSUBDATAPROC glNamedBufferSubData = nullptr; 
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC glNamedFramebufferDrawBuffer = nullptr; 
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC glNamedFramebufferDrawBuffers = nullptr; 
PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC glNamedFramebufferParameteri = nullptr; 
PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC glNamedFramebufferReadBuffer = nullptr; 
PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC glNamedFramebufferRenderbuffer = nullptr; 
PFNGLNAMEDFRAMEBUFFERTEXTUREPROC glNamedFramebufferTexture = nullptr; 
PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC glNamedFramebufferTextureLayer = nullptr; 
PFNGLNAMEDRENDERBUFFERSTORAGEPROC glNamedRenderbufferStorage = nullptr; 
PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC glNamedRenderbufferStorageMultisample = nullptr; 
PFNGLNORMALP3UIPROC glNormalP3ui = nullptr;
PFNGLNORMALP3UIVPROC glNormalP3uiv = nullptr;
PFNGLOBJECTLABELPROC glObjectLabel = nullptr; 
PFNGLOBJECTPTRLABELPROC glObjectPtrLabel = nullptr; 
PFNGLPATCHPARAMETERFVPROC glPatchParameterfv = nullptr; 
PFNGLPATCHPARAMETERIPROC glPatchParameteri = nullptr; 
PFNGLPAUSETRANSFORMFEEDBACKPROC glPauseTransformFeedback = nullptr; 
PFNGLPIXELSTOREFPROC glPixelStoref = nullptr; 
PFNGLPIXELSTOREIPROC glPixelStorei = nullptr; 
PFNGLPOINTPARAMETERFPROC glPointParameterf = nullptr; 
PFNGLPOINTPARAMETERFVPROC glPointParameterfv = nullptr; 
PFNGLPOINTPARAMETERIPROC glPointParameteri = nullptr; 
PFNGLPOINTPARAMETERIVPROC glPointParameteriv = nullptr; 
PFNGLPOINTSIZEPROC glPointSize = nullptr; 
PFNGLPOLYGONMODEPROC glPolygonMode = nullptr; 
PFNGLPOLYGONOFFSETPROC glPolygonOffset = nullptr; 
PFNGLPOLYGONOFFSETCLAMPPROC glPolygonOffsetClamp = nullptr;
PFNGLPOPDEBUGGROUPPROC glPopDebugGroup = nullptr; 
PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex = nullptr; 
PFNGLPROGRAMBINARYPROC glProgramBinary = nullptr; 
PFNGLPROGRAMPARAMETERIPROC glProgramParameteri = nullptr; 
PFNGLPROGRAMUNIFORM1DPROC glProgramUniform1d = nullptr;
PFNGLPROGRAMUNIFORM1DVPROC glProgramUniform1dv = nullptr;
PFNGLPROGRAMUNIFORM1FPROC glProgramUniform1f = nullptr; 
PFNGLPROGRAMUNIFORM1FVPROC glProgramUniform1fv = nullptr; 
PFNGLPROGRAMUNIFORM1IPROC glProgramUniform1i = nullptr; 
PFNGLPROGRAMUNIFORM1IVPROC glProgramUniform1iv = nullptr; 
PFNGLPROGRAMUNIFORM1UIPROC glProgramUniform1ui = nullptr; 
PFNGLPROGRAMUNIFORM1UIVPROC glProgramUniform1uiv = nullptr; 
PFNGLPROGRAMUNIFORM2DPROC glProgramUniform2d = nullptr;
PFNGLPROGRAMUNIFORM2DVPROC glProgramUniform2dv = nullptr;
PFNGLPROGRAMUNIFORM2FPROC glProgramUniform2f = nullptr; 
PFNGLPROGRAMUNIFORM2FVPROC glProgramUniform2fv = nullptr; 
PFNGLPROGRAMUNIFORM2IPROC glProgramUniform2i = nullptr; 
PFNGLPROGRAMUNIFORM2IVPROC glProgramUniform2iv = nullptr; 
PFNGLPROGRAMUNIFORM2UIPROC glProgramUniform2ui = nullptr; 
PFNGLPROGRAMUNIFORM2UIVPROC glProgramUniform2uiv = nullptr; 
PFNGLPROGRAMUNIFORM3DPROC glProgramUniform3d = nullptr;
PFNGLPROGRAMUNIFORM3DVPROC glProgramUniform3dv = nullptr;
PFNGLPROGRAMUNIFORM3FPROC glProgramUniform3f = nullptr; 
PFNGLPROGRAMUNIFORM3FVPROC glProgramUniform3fv = nullptr; 
PFNGLPROGRAMUNIFORM3IPROC glProgramUniform3i = nullptr; 
PFNGLPROGRAMUNIFORM3IVPROC glProgramUniform3iv = nullptr; 
PFNGLPROGRAMUNIFORM3UIPROC glProgramUniform3ui = nullptr; 
PFNGLPROGRAMUNIFORM3UIVPROC glProgramUniform3uiv = nullptr; 
PFNGLPROGRAMUNIFORM4DPROC glProgramUniform4d = nullptr;
PFNGLPROGRAMUNIFORM4DVPROC glProgramUniform4dv = nullptr;
PFNGLPROGRAMUNIFORM4FPROC glProgramUniform4f = nullptr; 
PFNGLPROGRAMUNIFORM4FVPROC glProgramUniform4fv = nullptr; 
PFNGLPROGRAMUNIFORM4IPROC glProgramUniform4i = nullptr; 
PFNGLPROGRAMUNIFORM4IVPROC glProgramUniform4iv = nullptr; 
PFNGLPROGRAMUNIFORM4UIPROC glProgramUniform4ui = nullptr; 
PFNGLPROGRAMUNIFORM4UIVPROC glProgramUniform4uiv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX2DVPROC glProgramUniformMatrix2dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2FVPROC glProgramUniformMatrix2fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glProgramUniformMatrix2x3dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glProgramUniformMatrix2x3fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glProgramUniformMatrix2x4dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glProgramUniformMatrix2x4fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX3DVPROC glProgramUniformMatrix3dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3FVPROC glProgramUniformMatrix3fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glProgramUniformMatrix3x2dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glProgramUniformMatrix3x2fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glProgramUniformMatrix3x4dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glProgramUniformMatrix3x4fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX4DVPROC glProgramUniformMatrix4dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC glProgramUniformMatrix4fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glProgramUniformMatrix4x2dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glProgramUniformMatrix4x2fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glProgramUniformMatrix4x3dv = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glProgramUniformMatrix4x3fv = nullptr; 
PFNGLPROVOKINGVERTEXPROC glProvokingVertex = nullptr; 
PFNGLPUSHDEBUGGROUPPROC glPushDebugGroup = nullptr; 
PFNGLQUERYCOUNTERPROC glQueryCounter = nullptr; 
PFNGLREADPIXELSPROC glReadPixels = nullptr; 
PFNGLREADNPIXELSPROC glReadnPixels = nullptr; 
PFNGLRELEASESHADERCOMPILERPROC glReleaseShaderCompiler = nullptr; 
PFNGLRESUMETRANSFORMFEEDBACKPROC glResumeTransformFeedback = nullptr; 
PFNGLSAMPLECOVERAGEPROC glSampleCoverage = nullptr; 
PFNGLSAMPLEMASKIPROC glSampleMaski = nullptr; 
PFNGLSAMPLERPARAMETERIIVPROC glSamplerParameterIiv = nullptr; 
PFNGLSAMPLERPARAMETERIUIVPROC glSamplerParameterIuiv = nullptr; 
PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf = nullptr; 
PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv = nullptr; 
PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri = nullptr; 
PFNGLSAMPLERPARAMETERIVPROC glSamplerParameteriv = nullptr; 
PFNGLSCISSORPROC glScissor = nullptr; 
PFNGLSCISSORARRAYVPROC glScissorArrayv = nullptr; 
PFNGLSCISSORINDEXEDPROC glScissorIndexed = nullptr; 
PFNGLSCISSORINDEXEDVPROC glScissorIndexedv = nullptr; 
PFNGLSECONDARYCOLORP3UIPROC glSecondaryColorP3ui = nullptr;
PFNGLSECONDARYCOLORP3UIVPROC glSecondaryColorP3uiv = nullptr;
PFNGLSHADERBINARYPROC glShaderBinary = nullptr; 
PFNGLSHADERSOURCEPROC glShaderSource = nullptr; 
PFNGLSHADERSTORAGEBLOCKBINDINGPROC glShaderStorageBlockBinding = nullptr; 
PFNGLSPECIALIZESHADERPROC glSpecializeShader = nullptr;
PFNGLSTENCILFUNCPROC glStencilFunc = nullptr; 
PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate = nullptr; 
PFNGLSTENCILMASKPROC glStencilMask = nullptr; 
PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate = nullptr; 
PFNGLSTENCILOPPROC glStencilOp = nullptr; 
PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate = nullptr; 
PFNGLTEXCOORDP1UIPROC glTexCoordP1ui = nullptr;
PFNGLTEXCOORDP1UIVPROC glTexCoordP1uiv = nullptr;
PFNGLTEXCOORDP2UIPROC glTexCoordP2ui = nullptr;
PFNGLTEXCOORDP2UIVPROC glTexCoordP2uiv = nullptr;
PFNGLTEXCOORDP3UIPROC glTexCoordP3ui = nullptr;
PFNGLTEXCOORDP3UIVPROC glTexCoordP3uiv = nullptr;
PFNGLTEXCOORDP4UIPROC glTexCoordP4ui = nullptr;
PFNGLTEXCOORDP4UIVPROC glTexCoordP4uiv = nullptr;
PFNGLTEXIMAGE1DPROC glTexImage1D = nullptr; 
PFNGLTEXIMAGE2DPROC glTexImage2D = nullptr; 
PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample = nullptr; 
PFNGLTEXIMAGE3DPROC glTexImage3D = nullptr; 
PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample = nullptr; 
PFNGLTEXTUREBARRIERPROC glTextureBarrier = nullptr; 
PFNGLTEXTUREBUFFERPROC glTextureBuffer = nullptr; 
PFNGLTEXTUREBUFFERRANGEPROC glTextureBufferRange = nullptr; 
PFNGLTEXTUREPARAMETERIIVPROC glTextureParameterIiv = nullptr; 
PFNGLTEXTUREPARAMETERIUIVPROC glTextureParameterIuiv = nullptr; 
PFNGLTEXTUREPARAMETERFPROC glTextureParameterf = nullptr; 
PFNGLTEXTUREPARAMETERFVPROC glTextureParameterfv = nullptr; 
PFNGLTEXTUREPARAMETERIPROC glTextureParameteri = nullptr; 
PFNGLTEXTUREPARAMETERIVPROC glTextureParameteriv = nullptr; 
PFNGLTEXTURESTORAGE1DPROC glTextureStorage1D = nullptr; 
PFNGLTEXTURESTORAGE2DPROC glTextureStorage2D = nullptr; 
PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC glTextureStorage2DMultisample = nullptr; 
PFNGLTEXTURESTORAGE3DPROC glTextureStorage3D = nullptr; 
PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC glTextureStorage3DMultisample = nullptr; 
PFNGLTEXTURESUBIMAGE1DPROC glTextureSubImage1D = nullptr; 
PFNGLTEXTURESUBIMAGE2DPROC glTextureSubImage2D = nullptr;
PFNGLTEXTURESUBIMAGE3DPROC glTextureSubImage3D = nullptr;
PFNGLTEXTUREVIEWPROC glTextureView = nullptr; 
PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC glTransformFeedbackBufferBase = nullptr; 
PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC glTransformFeedbackBufferRange = nullptr; 
PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings = nullptr; 
PFNGLUNIFORM1DPROC glUniform1d = nullptr;
PFNGLUNIFORM1DVPROC glUniform1dv = nullptr;
PFNGLUNIFORM1FPROC glUniform1f = nullptr; 
PFNGLUNIFORM1FVPROC glUniform1fv = nullptr; 
PFNGLUNIFORM1IPROC glUniform1i = nullptr; 
PFNGLUNIFORM1IVPROC glUniform1iv = nullptr; 
PFNGLUNIFORM1UIPROC glUniform1ui = nullptr; 
PFNGLUNIFORM1UIVPROC glUniform1uiv = nullptr; 
PFNGLUNIFORM2DPROC glUniform2d = nullptr;
PFNGLUNIFORM2DVPROC glUniform2dv = nullptr;
PFNGLUNIFORM2FPROC glUniform2f = nullptr; 
PFNGLUNIFORM2FVPROC glUniform2fv = nullptr; 
PFNGLUNIFORM2IPROC glUniform2i = nullptr; 
PFNGLUNIFORM2IVPROC glUniform2iv = nullptr; 
PFNGLUNIFORM2UIPROC glUniform2ui = nullptr; 
PFNGLUNIFORM2UIVPROC glUniform2uiv = nullptr; 
PFNGLUNIFORM3DPROC glUniform3d = nullptr;
PFNGLUNIFORM3DVPROC glUniform3dv = nullptr;
PFNGLUNIFORM3FPROC glUniform3f = nullptr; 
PFNGLUNIFORM3FVPROC glUniform3fv = nullptr; 
PFNGLUNIFORM3IPROC glUniform3i = nullptr; 
PFNGLUNIFORM3IVPROC glUniform3iv = nullptr; 
PFNGLUNIFORM3UIPROC glUniform3ui = nullptr; 
PFNGLUNIFORM3UIVPROC glUniform3uiv = nullptr; 
PFNGLUNIFORM4DPROC glUniform4d = nullptr;
PFNGLUNIFORM4DVPROC glUniform4dv = nullptr;
PFNGLUNIFORM4FPROC glUniform4f = nullptr; 
PFNGLUNIFORM4FVPROC glUniform4fv = nullptr; 
PFNGLUNIFORM4IPROC glUniform4i = nullptr; 
PFNGLUNIFORM4IVPROC glUniform4iv = nullptr; 
PFNGLUNIFORM4UIPROC glUniform4ui = nullptr; 
PFNGLUNIFORM4UIVPROC glUniform4uiv = nullptr; 
PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding = nullptr; 
PFNGLUNIFORMMATRIX2DVPROC glUniformMatrix2dv = nullptr;
PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv = nullptr; 
PFNGLUNIFORMMATRIX2X3DVPROC glUniformMatrix2x3dv = nullptr;
PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv = nullptr; 
PFNGLUNIFORMMATRIX2X4DVPROC glUniformMatrix2x4dv = nullptr;
PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv = nullptr; 
PFNGLUNIFORMMATRIX3DVPROC glUniformMatrix3dv = nullptr;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv = nullptr; 
PFNGLUNIFORMMATRIX3X2DVPROC glUniformMatrix3x2dv = nullptr;
PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv = nullptr; 
PFNGLUNIFORMMATRIX3X4DVPROC glUniformMatrix3x4dv = nullptr;
PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv = nullptr; 
PFNGLUNIFORMMATRIX4DVPROC glUniformMatrix4dv = nullptr;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = nullptr; 
PFNGLUNIFORMMATRIX4X2DVPROC glUniformMatrix4x2dv = nullptr;
PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv = nullptr; 
PFNGLUNIFORMMATRIX4X3DVPROC glUniformMatrix4x3dv = nullptr;
PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv = nullptr; 
PFNGLUNIFORMSUBROUTINESUIVPROC glUniformSubroutinesuiv = nullptr; 
PFNGLUNMAPNAMEDBUFFERPROC glUnmapNamedBuffer = nullptr; 
PFNGLUSEPROGRAMPROC glUseProgram = nullptr; 
PFNGLUSEPROGRAMSTAGESPROC glUseProgramStages = nullptr; 
PFNGLVALIDATEPROGRAMPROC glValidateProgram = nullptr; 
PFNGLVALIDATEPROGRAMPIPELINEPROC glValidateProgramPipeline = nullptr; 
PFNGLVERTEXARRAYATTRIBBINDINGPROC glVertexArrayAttribBinding = nullptr; 
PFNGLVERTEXARRAYATTRIBFORMATPROC glVertexArrayAttribFormat = nullptr; 
PFNGLVERTEXARRAYATTRIBIFORMATPROC glVertexArrayAttribIFormat = nullptr; 
PFNGLVERTEXARRAYATTRIBLFORMATPROC glVertexArrayAttribLFormat = nullptr; 
PFNGLVERTEXARRAYBINDINGDIVISORPROC glVertexArrayBindingDivisor = nullptr; 
PFNGLVERTEXARRAYELEMENTBUFFERPROC glVertexArrayElementBuffer = nullptr; 
PFNGLVERTEXARRAYVERTEXBUFFERPROC glVertexArrayVertexBuffer = nullptr; 
PFNGLVERTEXARRAYVERTEXBUFFERSPROC glVertexArrayVertexBuffers = nullptr; 
PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d = nullptr; 
PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv = nullptr; 
PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f = nullptr; 
PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv = nullptr; 
PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s = nullptr; 
PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv = nullptr; 
PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d = nullptr; 
PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv = nullptr; 
PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f = nullptr; 
PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv = nullptr; 
PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s = nullptr; 
PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv = nullptr; 
PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d = nullptr; 
PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv = nullptr; 
PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f = nullptr; 
PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv = nullptr; 
PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s = nullptr; 
PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv = nullptr; 
PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv = nullptr; 
PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv = nullptr; 
PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv = nullptr; 
PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub = nullptr; 
PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv = nullptr; 
PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv = nullptr; 
PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv = nullptr; 
PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv = nullptr; 
PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d = nullptr; 
PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv = nullptr; 
PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f = nullptr; 
PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv = nullptr; 
PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv = nullptr; 
PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s = nullptr; 
PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv = nullptr; 
PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv = nullptr; 
PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv = nullptr; 
PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv = nullptr; 
PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor = nullptr; 
PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i = nullptr; 
PFNGLVERTEXATTRIBI1IVPROC glVertexAttribI1iv = nullptr; 
PFNGLVERTEXATTRIBI1UIPROC glVertexAttribI1ui = nullptr; 
PFNGLVERTEXATTRIBI1UIVPROC glVertexAttribI1uiv = nullptr; 
PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i = nullptr; 
PFNGLVERTEXATTRIBI2IVPROC glVertexAttribI2iv = nullptr; 
PFNGLVERTEXATTRIBI2UIPROC glVertexAttribI2ui = nullptr; 
PFNGLVERTEXATTRIBI2UIVPROC glVertexAttribI2uiv = nullptr; 
PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i = nullptr; 
PFNGLVERTEXATTRIBI3IVPROC glVertexAttribI3iv = nullptr; 
PFNGLVERTEXATTRIBI3UIPROC glVertexAttribI3ui = nullptr; 
PFNGLVERTEXATTRIBI3UIVPROC glVertexAttribI3uiv = nullptr; 
PFNGLVERTEXATTRIBI4BVPROC glVertexAttribI4bv = nullptr; 
PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i = nullptr; 
PFNGLVERTEXATTRIBI4IVPROC glVertexAttribI4iv = nullptr; 
PFNGLVERTEXATTRIBI4SVPROC glVertexAttribI4sv = nullptr; 
PFNGLVERTEXATTRIBI4UBVPROC glVertexAttribI4ubv = nullptr; 
PFNGLVERTEXATTRIBI4UIPROC glVertexAttribI4ui = nullptr; 
PFNGLVERTEXATTRIBI4UIVPROC glVertexAttribI4uiv = nullptr; 
PFNGLVERTEXATTRIBI4USVPROC glVertexAttribI4usv = nullptr; 
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer = nullptr; 
PFNGLVERTEXATTRIBL1DPROC glVertexAttribL1d = nullptr; 
PFNGLVERTEXATTRIBL1DVPROC glVertexAttribL1dv = nullptr; 
PFNGLVERTEXATTRIBL2DPROC glVertexAttribL2d = nullptr; 
PFNGLVERTEXATTRIBL2DVPROC glVertexAttribL2dv = nullptr; 
PFNGLVERTEXATTRIBL3DPROC glVertexAttribL3d = nullptr; 
PFNGLVERTEXATTRIBL3DVPROC glVertexAttribL3dv = nullptr; 
PFNGLVERTEXATTRIBL4DPROC glVertexAttribL4d = nullptr; 
PFNGLVERTEXATTRIBL4DVPROC glVertexAttribL4dv = nullptr; 
PFNGLVERTEXATTRIBLPOINTERPROC glVertexAttribLPointer = nullptr; 
PFNGLVERTEXATTRIBP1UIPROC glVertexAttribP1ui = nullptr; 
PFNGLVERTEXATTRIBP1UIVPROC glVertexAttribP1uiv = nullptr;
PFNGLVERTEXATTRIBP2UIPROC glVertexAttribP2ui = nullptr; 
PFNGLVERTEXATTRIBP2UIVPROC glVertexAttribP2uiv = nullptr;
PFNGLVERTEXATTRIBP3UIPROC glVertexAttribP3ui = nullptr; 
PFNGLVERTEXATTRIBP3UIVPROC glVertexAttribP3uiv = nullptr;
PFNGLVERTEXATTRIBP4UIPROC glVertexAttribP4ui = nullptr; 
PFNGLVERTEXATTRIBP4UIVPROC glVertexAttribP4uiv = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr; 
PFNGLVERTEXP2UIPROC glVertexP2ui = nullptr;
PFNGLVERTEXP2UIVPROC glVertexP2uiv = nullptr;
PFNGLVERTEXP3UIPROC glVertexP3ui = nullptr;
PFNGLVERTEXP3UIVPROC glVertexP3uiv = nullptr;
PFNGLVERTEXP4UIPROC glVertexP4ui = nullptr;
PFNGLVERTEXP4UIVPROC glVertexP4uiv = nullptr;
PFNGLVIEWPORTPROC glViewport = nullptr; 
PFNGLVIEWPORTARRAYVPROC glViewportArrayv = nullptr; 
PFNGLVIEWPORTINDEXEDFPROC glViewportIndexedf = nullptr; 
PFNGLVIEWPORTINDEXEDFVPROC glViewportIndexedfv = nullptr; 
PFNGLWAITSYNCPROC glWaitSync = nullptr; 

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion1_0(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();
	
	if (!TRAP_OpenGL_VERSION_1_0)
		return;

	glCullFace = reinterpret_cast<PFNGLCULLFACEPROC>(load("glCullFace")); 
	glFrontFace = reinterpret_cast<PFNGLFRONTFACEPROC>(load("glFrontFace")); 
	glHint = reinterpret_cast<PFNGLHINTPROC>(load("glHint")); 
	glLineWidth = reinterpret_cast<PFNGLLINEWIDTHPROC>(load("glLineWidth")); 
	glPointSize = reinterpret_cast<PFNGLPOINTSIZEPROC>(load("glPointSize")); 
	glPolygonMode = reinterpret_cast<PFNGLPOLYGONMODEPROC>(load("glPolygonMode")); 
	glScissor = reinterpret_cast<PFNGLSCISSORPROC>(load("glScissor")); 
	glTexImage1D = reinterpret_cast<PFNGLTEXIMAGE1DPROC>(load("glTexImage1D")); 
	glTexImage2D = reinterpret_cast<PFNGLTEXIMAGE2DPROC>(load("glTexImage2D")); 
	glClear = reinterpret_cast<PFNGLCLEARPROC>(load("glClear")); 
	glClearColor = reinterpret_cast<PFNGLCLEARCOLORPROC>(load("glClearColor")); 
	glClearStencil = reinterpret_cast<PFNGLCLEARSTENCILPROC>(load("glClearStencil")); 
	glClearDepth = reinterpret_cast<PFNGLCLEARDEPTHPROC>(load("glClearDepth")); 
	glStencilMask = reinterpret_cast<PFNGLSTENCILMASKPROC>(load("glStencilMask")); 
	glColorMask = reinterpret_cast<PFNGLCOLORMASKPROC>(load("glColorMask")); 
	glDepthMask = reinterpret_cast<PFNGLDEPTHMASKPROC>(load("glDepthMask")); 
	glDisable = reinterpret_cast<PFNGLDISABLEPROC>(load("glDisable")); 
	glEnable = reinterpret_cast<PFNGLENABLEPROC>(load("glEnable")); 
	glFinish = reinterpret_cast<PFNGLFINISHPROC>(load("glFinish")); 
	glFlush = reinterpret_cast<PFNGLFLUSHPROC>(load("glFlush")); 
	glBlendFunc = reinterpret_cast<PFNGLBLENDFUNCPROC>(load("glBlendFunc")); 
	glLogicOp = reinterpret_cast<PFNGLLOGICOPPROC>(load("glLogicOp")); 
	glStencilFunc = reinterpret_cast<PFNGLSTENCILFUNCPROC>(load("glStencilFunc")); 
	glStencilOp = reinterpret_cast<PFNGLSTENCILOPPROC>(load("glStencilOp")); 
	glDepthFunc = reinterpret_cast<PFNGLDEPTHFUNCPROC>(load("glDepthFunc")); 
	glPixelStoref = reinterpret_cast<PFNGLPIXELSTOREFPROC>(load("glPixelStoref")); 
	glPixelStorei = reinterpret_cast<PFNGLPIXELSTOREIPROC>(load("glPixelStorei")); 
	glReadPixels = reinterpret_cast<PFNGLREADPIXELSPROC>(load("glReadPixels")); 
	glGetBooleanv = reinterpret_cast<PFNGLGETBOOLEANVPROC>(load("glGetBooleanv")); 
	glGetDoublev = reinterpret_cast<PFNGLGETDOUBLEVPROC>(load("glGetDoublev")); 
	glGetError = reinterpret_cast<PFNGLGETERRORPROC>(load("glGetError")); 
	glGetFloatv = reinterpret_cast<PFNGLGETFLOATVPROC>(load("glGetFloatv")); 
	glGetIntegerv = reinterpret_cast<PFNGLGETINTEGERVPROC>(load("glGetIntegerv")); 
	glGetString = reinterpret_cast<PFNGLGETSTRINGPROC>(load("glGetString")); 
	glIsEnabled = reinterpret_cast<PFNGLISENABLEDPROC>(load("glIsEnabled")); 
	glDepthRange = reinterpret_cast<PFNGLDEPTHRANGEPROC>(load("glDepthRange")); 
	glViewport = reinterpret_cast<PFNGLVIEWPORTPROC>(load("glViewport")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion1_1(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();
	
	if (!TRAP_OpenGL_VERSION_1_1)
		return;

	glDrawArrays = reinterpret_cast<PFNGLDRAWARRAYSPROC>(load("glDrawArrays")); 
	glDrawElements = reinterpret_cast<PFNGLDRAWELEMENTSPROC>(load("glDrawElements")); 
	glPolygonOffset = reinterpret_cast<PFNGLPOLYGONOFFSETPROC>(load("glPolygonOffset")); 
	glCopyTexImage1D = reinterpret_cast<PFNGLCOPYTEXIMAGE1DPROC>(load("glCopyTexImage1D")); 
	glCopyTexImage2D = reinterpret_cast<PFNGLCOPYTEXIMAGE2DPROC>(load("glCopyTexImage2D")); 
	glCopyTexSubImage1D = reinterpret_cast<PFNGLCOPYTEXSUBIMAGE1DPROC>(load("glCopyTexSubImage1D")); 
	glCopyTexSubImage2D = reinterpret_cast<PFNGLCOPYTEXSUBIMAGE2DPROC>(load("glCopyTexSubImage2D")); 
	glBindTexture = reinterpret_cast<PFNGLBINDTEXTUREPROC>(load("glBindTexture")); 
	glDeleteTextures = reinterpret_cast<PFNGLDELETETEXTURESPROC>(load("glDeleteTextures")); 
	glGenTextures = reinterpret_cast<PFNGLGENTEXTURESPROC>(load("glGenTextures")); 
	glIsTexture = reinterpret_cast<PFNGLISTEXTUREPROC>(load("glIsTexture")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion1_2(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();
	
	if (!TRAP_OpenGL_VERSION_1_2)
		return;

	glDrawRangeElements = reinterpret_cast<PFNGLDRAWRANGEELEMENTSPROC>(load("glDrawRangeElements")); 
	glTexImage3D = reinterpret_cast<PFNGLTEXIMAGE3DPROC>(load("glTexImage3D")); 
	glCopyTexSubImage3D = reinterpret_cast<PFNGLCOPYTEXSUBIMAGE3DPROC>(load("glCopyTexSubImage3D")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion1_3(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();
	
	if (!TRAP_OpenGL_VERSION_1_3)
		return;

	glActiveTexture = reinterpret_cast<PFNGLACTIVETEXTUREPROC>(load("glActiveTexture")); 
	glSampleCoverage = reinterpret_cast<PFNGLSAMPLECOVERAGEPROC>(load("glSampleCoverage")); 
	glCompressedTexImage3D = reinterpret_cast<PFNGLCOMPRESSEDTEXIMAGE3DPROC>(load("glCompressedTexImage3D")); 
	glCompressedTexImage2D = reinterpret_cast<PFNGLCOMPRESSEDTEXIMAGE2DPROC>(load("glCompressedTexImage2D")); 
	glCompressedTexImage1D = reinterpret_cast<PFNGLCOMPRESSEDTEXIMAGE1DPROC>(load("glCompressedTexImage1D")); 
	glCompressedTexSubImage3D = reinterpret_cast<PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC>(load("glCompressedTexSubImage3D")); 
	glCompressedTexSubImage2D = reinterpret_cast<PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC>(load("glCompressedTexSubImage2D")); 
	glCompressedTexSubImage1D = reinterpret_cast<PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC>(load("glCompressedTexSubImage1D")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion1_4(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();
	
	if (!TRAP_OpenGL_VERSION_1_4)
		return;

	glBlendFuncSeparate = reinterpret_cast<PFNGLBLENDFUNCSEPARATEPROC>(load("glBlendFuncSeparate")); 
	glMultiDrawArrays = reinterpret_cast<PFNGLMULTIDRAWARRAYSPROC>(load("glMultiDrawArrays")); 
	glMultiDrawElements = reinterpret_cast<PFNGLMULTIDRAWELEMENTSPROC>(load("glMultiDrawElements")); 
	glPointParameterf = reinterpret_cast<PFNGLPOINTPARAMETERFPROC>(load("glPointParameterf")); 
	glPointParameterfv = reinterpret_cast<PFNGLPOINTPARAMETERFVPROC>(load("glPointParameterfv")); 
	glPointParameteri = reinterpret_cast<PFNGLPOINTPARAMETERIPROC>(load("glPointParameteri")); 
	glPointParameteriv = reinterpret_cast<PFNGLPOINTPARAMETERIVPROC>(load("glPointParameteriv")); 
	glBlendColor = reinterpret_cast<PFNGLBLENDCOLORPROC>(load("glBlendColor")); 
	glBlendEquation = reinterpret_cast<PFNGLBLENDEQUATIONPROC>(load("glBlendEquation")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion1_5(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();
	
	if (!TRAP_OpenGL_VERSION_1_5)
		return;

	glGenQueries = reinterpret_cast<PFNGLGENQUERIESPROC>(load("glGenQueries")); 
	glDeleteQueries = reinterpret_cast<PFNGLDELETEQUERIESPROC>(load("glDeleteQueries")); 
	glIsQuery = reinterpret_cast<PFNGLISQUERYPROC>(load("glIsQuery")); 
	glBeginQuery = reinterpret_cast<PFNGLBEGINQUERYPROC>(load("glBeginQuery")); 
	glEndQuery = reinterpret_cast<PFNGLENDQUERYPROC>(load("glEndQuery")); 
	glGetQueryiv = reinterpret_cast<PFNGLGETQUERYIVPROC>(load("glGetQueryiv")); 
	glGetQueryObjectiv = reinterpret_cast<PFNGLGETQUERYOBJECTIVPROC>(load("glGetQueryObjectiv")); 
	glGetQueryObjectuiv = reinterpret_cast<PFNGLGETQUERYOBJECTUIVPROC>(load("glGetQueryObjectuiv")); 
	glBindBuffer = reinterpret_cast<PFNGLBINDBUFFERPROC>(load("glBindBuffer")); 
	glDeleteBuffers = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(load("glDeleteBuffers")); 
	glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>(load("glGenBuffers")); 
	glIsBuffer = reinterpret_cast<PFNGLISBUFFERPROC>(load("glIsBuffer")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion2_0(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();
	
	if (!TRAP_OpenGL_VERSION_2_0)
		return;

	glBlendEquationSeparate = reinterpret_cast<PFNGLBLENDEQUATIONSEPARATEPROC>(load("glBlendEquationSeparate")); 
	glStencilOpSeparate = reinterpret_cast<PFNGLSTENCILOPSEPARATEPROC>(load("glStencilOpSeparate")); 
	glStencilFuncSeparate = reinterpret_cast<PFNGLSTENCILFUNCSEPARATEPROC>(load("glStencilFuncSeparate")); 
	glStencilMaskSeparate = reinterpret_cast<PFNGLSTENCILMASKSEPARATEPROC>(load("glStencilMaskSeparate")); 
	glAttachShader = reinterpret_cast<PFNGLATTACHSHADERPROC>(load("glAttachShader")); 
	glBindAttribLocation = reinterpret_cast<PFNGLBINDATTRIBLOCATIONPROC>(load("glBindAttribLocation")); 
	glCompileShader = reinterpret_cast<PFNGLCOMPILESHADERPROC>(load("glCompileShader")); 
	glCreateProgram = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(load("glCreateProgram")); 
	glCreateShader = reinterpret_cast<PFNGLCREATESHADERPROC>(load("glCreateShader")); 
	glDeleteProgram = reinterpret_cast<PFNGLDELETEPROGRAMPROC>(load("glDeleteProgram")); 
	glDeleteShader = reinterpret_cast<PFNGLDELETESHADERPROC>(load("glDeleteShader")); 
	glDetachShader = reinterpret_cast<PFNGLDETACHSHADERPROC>(load("glDetachShader")); 
	glGetActiveAttrib = reinterpret_cast<PFNGLGETACTIVEATTRIBPROC>(load("glGetActiveAttrib")); 
	glGetActiveUniform = reinterpret_cast<PFNGLGETACTIVEUNIFORMPROC>(load("glGetActiveUniform")); 
	glGetAttachedShaders = reinterpret_cast<PFNGLGETATTACHEDSHADERSPROC>(load("glGetAttachedShaders")); 
	glGetAttribLocation = reinterpret_cast<PFNGLGETATTRIBLOCATIONPROC>(load("glGetAttribLocation")); 
	glGetProgramiv = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(load("glGetProgramiv")); 
	glGetProgramInfoLog = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(load("glGetProgramInfoLog")); 
	glGetShaderiv = reinterpret_cast<PFNGLGETSHADERIVPROC>(load("glGetShaderiv")); 
	glGetShaderInfoLog = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(load("glGetShaderInfoLog")); 
	glGetShaderSource = reinterpret_cast<PFNGLGETSHADERSOURCEPROC>(load("glGetShaderSource")); 
	glGetUniformLocation = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(load("glGetUniformLocation")); 
	glGetUniformfv = reinterpret_cast<PFNGLGETUNIFORMFVPROC>(load("glGetUniformfv")); 
	glGetUniformiv = reinterpret_cast<PFNGLGETUNIFORMIVPROC>(load("glGetUniformiv")); 
	glGetVertexAttribdv = reinterpret_cast<PFNGLGETVERTEXATTRIBDVPROC>(load("glGetVertexAttribdv")); 
	glGetVertexAttribfv = reinterpret_cast<PFNGLGETVERTEXATTRIBFVPROC>(load("glGetVertexAttribfv")); 
	glGetVertexAttribiv = reinterpret_cast<PFNGLGETVERTEXATTRIBIVPROC>(load("glGetVertexAttribiv")); 
	glGetVertexAttribPointerv = reinterpret_cast<PFNGLGETVERTEXATTRIBPOINTERVPROC>(load("glGetVertexAttribPointerv")); 
	glIsProgram = reinterpret_cast<PFNGLISPROGRAMPROC>(load("glIsProgram")); 
	glIsShader = reinterpret_cast<PFNGLISSHADERPROC>(load("glIsShader")); 
	glLinkProgram = reinterpret_cast<PFNGLLINKPROGRAMPROC>(load("glLinkProgram")); 
	glShaderSource = reinterpret_cast<PFNGLSHADERSOURCEPROC>(load("glShaderSource")); 
	glUseProgram = reinterpret_cast<PFNGLUSEPROGRAMPROC>(load("glUseProgram")); 
	glUniform1f = reinterpret_cast<PFNGLUNIFORM1FPROC>(load("glUniform1f")); 
	glUniform2f = reinterpret_cast<PFNGLUNIFORM2FPROC>(load("glUniform2f")); 
	glUniform3f = reinterpret_cast<PFNGLUNIFORM3FPROC>(load("glUniform3f")); 
	glUniform4f = reinterpret_cast<PFNGLUNIFORM4FPROC>(load("glUniform4f")); 
	glUniform1i = reinterpret_cast<PFNGLUNIFORM1IPROC>(load("glUniform1i")); 
	glUniform2i = reinterpret_cast<PFNGLUNIFORM2IPROC>(load("glUniform2i")); 
	glUniform3i = reinterpret_cast<PFNGLUNIFORM3IPROC>(load("glUniform3i")); 
	glUniform4i = reinterpret_cast<PFNGLUNIFORM4IPROC>(load("glUniform4i")); 
	glUniform1fv = reinterpret_cast<PFNGLUNIFORM1FVPROC>(load("glUniform1fv")); 
	glUniform2fv = reinterpret_cast<PFNGLUNIFORM2FVPROC>(load("glUniform2fv")); 
	glUniform3fv = reinterpret_cast<PFNGLUNIFORM3FVPROC>(load("glUniform3fv")); 
	glUniform4fv = reinterpret_cast<PFNGLUNIFORM4FVPROC>(load("glUniform4fv")); 
	glUniform1iv = reinterpret_cast<PFNGLUNIFORM1IVPROC>(load("glUniform1iv")); 
	glUniform2iv = reinterpret_cast<PFNGLUNIFORM2IVPROC>(load("glUniform2iv")); 
	glUniform3iv = reinterpret_cast<PFNGLUNIFORM3IVPROC>(load("glUniform3iv")); 
	glUniform4iv = reinterpret_cast<PFNGLUNIFORM4IVPROC>(load("glUniform4iv")); 
	glUniformMatrix2fv = reinterpret_cast<PFNGLUNIFORMMATRIX2FVPROC>(load("glUniformMatrix2fv")); 
	glUniformMatrix3fv = reinterpret_cast<PFNGLUNIFORMMATRIX3FVPROC>(load("glUniformMatrix3fv")); 
	glUniformMatrix4fv = reinterpret_cast<PFNGLUNIFORMMATRIX4FVPROC>(load("glUniformMatrix4fv")); 
	glValidateProgram = reinterpret_cast<PFNGLVALIDATEPROGRAMPROC>(load("glValidateProgram")); 
	glVertexAttrib1d = reinterpret_cast<PFNGLVERTEXATTRIB1DPROC>(load("glVertexAttrib1d")); 
	glVertexAttrib1dv = reinterpret_cast<PFNGLVERTEXATTRIB1DVPROC>(load("glVertexAttrib1dv")); 
	glVertexAttrib1f = reinterpret_cast<PFNGLVERTEXATTRIB1FPROC>(load("glVertexAttrib1f")); 
	glVertexAttrib1fv = reinterpret_cast<PFNGLVERTEXATTRIB1FVPROC>(load("glVertexAttrib1fv")); 
	glVertexAttrib1s = reinterpret_cast<PFNGLVERTEXATTRIB1SPROC>(load("glVertexAttrib1s")); 
	glVertexAttrib1sv = reinterpret_cast<PFNGLVERTEXATTRIB1SVPROC>(load("glVertexAttrib1sv")); 
	glVertexAttrib2d = reinterpret_cast<PFNGLVERTEXATTRIB2DPROC>(load("glVertexAttrib2d")); 
	glVertexAttrib2dv = reinterpret_cast<PFNGLVERTEXATTRIB2DVPROC>(load("glVertexAttrib2dv")); 
	glVertexAttrib2f = reinterpret_cast<PFNGLVERTEXATTRIB2FPROC>(load("glVertexAttrib2f")); 
	glVertexAttrib2fv = reinterpret_cast<PFNGLVERTEXATTRIB2FVPROC>(load("glVertexAttrib2fv")); 
	glVertexAttrib2s = reinterpret_cast<PFNGLVERTEXATTRIB2SPROC>(load("glVertexAttrib2s")); 
	glVertexAttrib2sv = reinterpret_cast<PFNGLVERTEXATTRIB2SVPROC>(load("glVertexAttrib2sv")); 
	glVertexAttrib3d = reinterpret_cast<PFNGLVERTEXATTRIB3DPROC>(load("glVertexAttrib3d")); 
	glVertexAttrib3dv = reinterpret_cast<PFNGLVERTEXATTRIB3DVPROC>(load("glVertexAttrib3dv")); 
	glVertexAttrib3f = reinterpret_cast<PFNGLVERTEXATTRIB3FPROC>(load("glVertexAttrib3f")); 
	glVertexAttrib3fv = reinterpret_cast<PFNGLVERTEXATTRIB3FVPROC>(load("glVertexAttrib3fv")); 
	glVertexAttrib3s = reinterpret_cast<PFNGLVERTEXATTRIB3SPROC>(load("glVertexAttrib3s")); 
	glVertexAttrib3sv = reinterpret_cast<PFNGLVERTEXATTRIB3SVPROC>(load("glVertexAttrib3sv")); 
	glVertexAttrib4Nbv = reinterpret_cast<PFNGLVERTEXATTRIB4NBVPROC>(load("glVertexAttrib4Nbv")); 
	glVertexAttrib4Niv = reinterpret_cast<PFNGLVERTEXATTRIB4NIVPROC>(load("glVertexAttrib4Niv")); 
	glVertexAttrib4Nsv = reinterpret_cast<PFNGLVERTEXATTRIB4NSVPROC>(load("glVertexAttrib4Nsv")); 
	glVertexAttrib4Nub = reinterpret_cast<PFNGLVERTEXATTRIB4NUBPROC>(load("glVertexAttrib4Nub")); 
	glVertexAttrib4Nubv = reinterpret_cast<PFNGLVERTEXATTRIB4NUBVPROC>(load("glVertexAttrib4Nubv")); 
	glVertexAttrib4Nuiv = reinterpret_cast<PFNGLVERTEXATTRIB4NUIVPROC>(load("glVertexAttrib4Nuiv")); 
	glVertexAttrib4Nusv = reinterpret_cast<PFNGLVERTEXATTRIB4NUSVPROC>(load("glVertexAttrib4Nusv")); 
	glVertexAttrib4bv = reinterpret_cast<PFNGLVERTEXATTRIB4BVPROC>(load("glVertexAttrib4bv")); 
	glVertexAttrib4d = reinterpret_cast<PFNGLVERTEXATTRIB4DPROC>(load("glVertexAttrib4d")); 
	glVertexAttrib4dv = reinterpret_cast<PFNGLVERTEXATTRIB4DVPROC>(load("glVertexAttrib4dv")); 
	glVertexAttrib4f = reinterpret_cast<PFNGLVERTEXATTRIB4FPROC>(load("glVertexAttrib4f")); 
	glVertexAttrib4fv = reinterpret_cast<PFNGLVERTEXATTRIB4FVPROC>(load("glVertexAttrib4fv")); 
	glVertexAttrib4iv = reinterpret_cast<PFNGLVERTEXATTRIB4IVPROC>(load("glVertexAttrib4iv")); 
	glVertexAttrib4s = reinterpret_cast<PFNGLVERTEXATTRIB4SPROC>(load("glVertexAttrib4s")); 
	glVertexAttrib4sv = reinterpret_cast<PFNGLVERTEXATTRIB4SVPROC>(load("glVertexAttrib4sv")); 
	glVertexAttrib4ubv = reinterpret_cast<PFNGLVERTEXATTRIB4UBVPROC>(load("glVertexAttrib4ubv")); 
	glVertexAttrib4uiv = reinterpret_cast<PFNGLVERTEXATTRIB4UIVPROC>(load("glVertexAttrib4uiv")); 
	glVertexAttrib4usv = reinterpret_cast<PFNGLVERTEXATTRIB4USVPROC>(load("glVertexAttrib4usv")); 
	glVertexAttribPointer = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(load("glVertexAttribPointer")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion2_1(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();

	if (!TRAP_OpenGL_VERSION_2_1)
		return;

	glUniformMatrix2x3fv = reinterpret_cast<PFNGLUNIFORMMATRIX2X3FVPROC>(load("glUniformMatrix2x3fv")); 
	glUniformMatrix3x2fv = reinterpret_cast<PFNGLUNIFORMMATRIX3X2FVPROC>(load("glUniformMatrix3x2fv")); 
	glUniformMatrix2x4fv = reinterpret_cast<PFNGLUNIFORMMATRIX2X4FVPROC>(load("glUniformMatrix2x4fv")); 
	glUniformMatrix4x2fv = reinterpret_cast<PFNGLUNIFORMMATRIX4X2FVPROC>(load("glUniformMatrix4x2fv")); 
	glUniformMatrix3x4fv = reinterpret_cast<PFNGLUNIFORMMATRIX3X4FVPROC>(load("glUniformMatrix3x4fv")); 
	glUniformMatrix4x3fv = reinterpret_cast<PFNGLUNIFORMMATRIX4X3FVPROC>(load("glUniformMatrix4x3fv")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion3_0(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();

	if (!TRAP_OpenGL_VERSION_3_0)
		return;

	glColorMaski = reinterpret_cast<PFNGLCOLORMASKIPROC>(load("glColorMaski")); 
	glGetBooleani_v = reinterpret_cast<PFNGLGETBOOLEANI_VPROC>(load("glGetBooleani_v")); 
	glEnablei = reinterpret_cast<PFNGLENABLEIPROC>(load("glEnablei")); 
	glDisablei = reinterpret_cast<PFNGLDISABLEIPROC>(load("glDisablei")); 
	glIsEnabledi = reinterpret_cast<PFNGLISENABLEDIPROC>(load("glIsEnabledi")); 
	glBeginTransformFeedback = reinterpret_cast<PFNGLBEGINTRANSFORMFEEDBACKPROC>(load("glBeginTransformFeedback")); 
	glEndTransformFeedback = reinterpret_cast<PFNGLENDTRANSFORMFEEDBACKPROC>(load("glEndTransformFeedback")); 
	glBindBufferRange = reinterpret_cast<PFNGLBINDBUFFERRANGEPROC>(load("glBindBufferRange")); 
	glBindBufferBase = reinterpret_cast<PFNGLBINDBUFFERBASEPROC>(load("glBindBufferBase")); 
	glTransformFeedbackVaryings = reinterpret_cast<PFNGLTRANSFORMFEEDBACKVARYINGSPROC>(load("glTransformFeedbackVaryings")); 
	glGetTransformFeedbackVarying = reinterpret_cast<PFNGLGETTRANSFORMFEEDBACKVARYINGPROC>(load("glGetTransformFeedbackVarying")); 
	glClampColor = reinterpret_cast<PFNGLCLAMPCOLORPROC>(load("glClampColor")); 
	glBeginConditionalRender = reinterpret_cast<PFNGLBEGINCONDITIONALRENDERPROC>(load("glBeginConditionalRender")); 
	glEndConditionalRender = reinterpret_cast<PFNGLENDCONDITIONALRENDERPROC>(load("glEndConditionalRender")); 
	glVertexAttribIPointer = reinterpret_cast<PFNGLVERTEXATTRIBIPOINTERPROC>(load("glVertexAttribIPointer")); 
	glGetVertexAttribIiv = reinterpret_cast<PFNGLGETVERTEXATTRIBIIVPROC>(load("glGetVertexAttribIiv")); 
	glGetVertexAttribIuiv = reinterpret_cast<PFNGLGETVERTEXATTRIBIUIVPROC>(load("glGetVertexAttribIuiv")); 
	glVertexAttribI1i = reinterpret_cast<PFNGLVERTEXATTRIBI1IPROC>(load("glVertexAttribI1i")); 
	glVertexAttribI2i = reinterpret_cast<PFNGLVERTEXATTRIBI2IPROC>(load("glVertexAttribI2i")); 
	glVertexAttribI3i = reinterpret_cast<PFNGLVERTEXATTRIBI3IPROC>(load("glVertexAttribI3i")); 
	glVertexAttribI4i = reinterpret_cast<PFNGLVERTEXATTRIBI4IPROC>(load("glVertexAttribI4i")); 
	glVertexAttribI1ui = reinterpret_cast<PFNGLVERTEXATTRIBI1UIPROC>(load("glVertexAttribI1ui")); 
	glVertexAttribI2ui = reinterpret_cast<PFNGLVERTEXATTRIBI2UIPROC>(load("glVertexAttribI2ui")); 
	glVertexAttribI3ui = reinterpret_cast<PFNGLVERTEXATTRIBI3UIPROC>(load("glVertexAttribI3ui")); 
	glVertexAttribI4ui = reinterpret_cast<PFNGLVERTEXATTRIBI4UIPROC>(load("glVertexAttribI4ui")); 
	glVertexAttribI1iv = reinterpret_cast<PFNGLVERTEXATTRIBI1IVPROC>(load("glVertexAttribI1iv")); 
	glVertexAttribI2iv = reinterpret_cast<PFNGLVERTEXATTRIBI2IVPROC>(load("glVertexAttribI2iv")); 
	glVertexAttribI3iv = reinterpret_cast<PFNGLVERTEXATTRIBI3IVPROC>(load("glVertexAttribI3iv")); 
	glVertexAttribI4iv = reinterpret_cast<PFNGLVERTEXATTRIBI4IVPROC>(load("glVertexAttribI4iv")); 
	glVertexAttribI1uiv = reinterpret_cast<PFNGLVERTEXATTRIBI1UIVPROC>(load("glVertexAttribI1uiv")); 
	glVertexAttribI2uiv = reinterpret_cast<PFNGLVERTEXATTRIBI2UIVPROC>(load("glVertexAttribI2uiv")); 
	glVertexAttribI3uiv = reinterpret_cast<PFNGLVERTEXATTRIBI3UIVPROC>(load("glVertexAttribI3uiv")); 
	glVertexAttribI4uiv = reinterpret_cast<PFNGLVERTEXATTRIBI4UIVPROC>(load("glVertexAttribI4uiv")); 
	glVertexAttribI4bv = reinterpret_cast<PFNGLVERTEXATTRIBI4BVPROC>(load("glVertexAttribI4bv")); 
	glVertexAttribI4sv = reinterpret_cast<PFNGLVERTEXATTRIBI4SVPROC>(load("glVertexAttribI4sv")); 
	glVertexAttribI4ubv = reinterpret_cast<PFNGLVERTEXATTRIBI4UBVPROC>(load("glVertexAttribI4ubv")); 
	glVertexAttribI4usv = reinterpret_cast<PFNGLVERTEXATTRIBI4USVPROC>(load("glVertexAttribI4usv")); 
	glGetUniformuiv = reinterpret_cast<PFNGLGETUNIFORMUIVPROC>(load("glGetUniformuiv")); 
	glBindFragDataLocation = reinterpret_cast<PFNGLBINDFRAGDATALOCATIONPROC>(load("glBindFragDataLocation")); 
	glGetFragDataLocation = reinterpret_cast<PFNGLGETFRAGDATALOCATIONPROC>(load("glGetFragDataLocation")); 
	glUniform1ui = reinterpret_cast<PFNGLUNIFORM1UIPROC>(load("glUniform1ui")); 
	glUniform2ui = reinterpret_cast<PFNGLUNIFORM2UIPROC>(load("glUniform2ui")); 
	glUniform3ui = reinterpret_cast<PFNGLUNIFORM3UIPROC>(load("glUniform3ui")); 
	glUniform4ui = reinterpret_cast<PFNGLUNIFORM4UIPROC>(load("glUniform4ui")); 
	glUniform1uiv = reinterpret_cast<PFNGLUNIFORM1UIVPROC>(load("glUniform1uiv")); 
	glUniform2uiv = reinterpret_cast<PFNGLUNIFORM2UIVPROC>(load("glUniform2uiv")); 
	glUniform3uiv = reinterpret_cast<PFNGLUNIFORM3UIVPROC>(load("glUniform3uiv")); 
	glUniform4uiv = reinterpret_cast<PFNGLUNIFORM4UIVPROC>(load("glUniform4uiv")); 
	glGetStringi = reinterpret_cast<PFNGLGETSTRINGIPROC>(load("glGetStringi")); 
	glIsRenderbuffer = reinterpret_cast<PFNGLISRENDERBUFFERPROC>(load("glIsRenderbuffer")); 
	glBindRenderbuffer = reinterpret_cast<PFNGLBINDRENDERBUFFERPROC>(load("glBindRenderbuffer")); 
	glDeleteRenderbuffers = reinterpret_cast<PFNGLDELETERENDERBUFFERSPROC>(load("glDeleteRenderbuffers")); 
	glGenRenderbuffers = reinterpret_cast<PFNGLGENRENDERBUFFERSPROC>(load("glGenRenderbuffers")); 
	glIsFramebuffer = reinterpret_cast<PFNGLISFRAMEBUFFERPROC>(load("glIsFramebuffer")); 
	glBindFramebuffer = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(load("glBindFramebuffer")); 
	glDeleteFramebuffers = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(load("glDeleteFramebuffers")); 
	glGenFramebuffers = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(load("glGenFramebuffers")); 
	glFramebufferTexture1D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE1DPROC>(load("glFramebufferTexture1D")); 
	glFramebufferTexture2D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(load("glFramebufferTexture2D")); 
	glFramebufferTexture3D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE3DPROC>(load("glFramebufferTexture3D")); 
	glBindVertexArray = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(load("glBindVertexArray")); 
	glDeleteVertexArrays = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(load("glDeleteVertexArrays")); 
	glGenVertexArrays = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(load("glGenVertexArrays")); 
	glIsVertexArray = reinterpret_cast<PFNGLISVERTEXARRAYPROC>(load("glIsVertexArray")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion3_1(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();

	if (!TRAP_OpenGL_VERSION_3_1)
		return;

	glDrawArraysInstanced = reinterpret_cast<PFNGLDRAWARRAYSINSTANCEDPROC>(load("glDrawArraysInstanced")); 
	glDrawElementsInstanced = reinterpret_cast<PFNGLDRAWELEMENTSINSTANCEDPROC>(load("glDrawElementsInstanced")); 
	glPrimitiveRestartIndex = reinterpret_cast<PFNGLPRIMITIVERESTARTINDEXPROC>(load("glPrimitiveRestartIndex")); 
	glGetUniformIndices = reinterpret_cast<PFNGLGETUNIFORMINDICESPROC>(load("glGetUniformIndices")); 
	glGetActiveUniformsiv = reinterpret_cast<PFNGLGETACTIVEUNIFORMSIVPROC>(load("glGetActiveUniformsiv")); 
	glGetActiveUniformName = reinterpret_cast<PFNGLGETACTIVEUNIFORMNAMEPROC>(load("glGetActiveUniformName")); 
	glGetUniformBlockIndex = reinterpret_cast<PFNGLGETUNIFORMBLOCKINDEXPROC>(load("glGetUniformBlockIndex")); 
	glGetActiveUniformBlockiv = reinterpret_cast<PFNGLGETACTIVEUNIFORMBLOCKIVPROC>(load("glGetActiveUniformBlockiv")); 
	glGetActiveUniformBlockName = reinterpret_cast<PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC>(load("glGetActiveUniformBlockName"));  
	glUniformBlockBinding = reinterpret_cast<PFNGLUNIFORMBLOCKBINDINGPROC>(load("glUniformBlockBinding")); 
	glGetIntegeri_v = reinterpret_cast<PFNGLGETINTEGERI_VPROC>(load("glGetIntegeri_v")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion3_2(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();

	if (!TRAP_OpenGL_VERSION_3_2)
		return;

	glDrawElementsBaseVertex = reinterpret_cast<PFNGLDRAWELEMENTSBASEVERTEXPROC>(load("glDrawElementsBaseVertex")); 
	glDrawRangeElementsBaseVertex = reinterpret_cast<PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC>(load("glDrawRangeElementsBaseVertex")); 
	glDrawElementsInstancedBaseVertex = reinterpret_cast<PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC>(load("glDrawElementsInstancedBaseVertex")); 
	glMultiDrawElementsBaseVertex = reinterpret_cast<PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC>(load("glMultiDrawElementsBaseVertex")); 
	glProvokingVertex = reinterpret_cast<PFNGLPROVOKINGVERTEXPROC>(load("glProvokingVertex")); 
	glFenceSync = reinterpret_cast<PFNGLFENCESYNCPROC>(load("glFenceSync")); 
	glIsSync = reinterpret_cast<PFNGLISSYNCPROC>(load("glIsSync")); 
	glDeleteSync = reinterpret_cast<PFNGLDELETESYNCPROC>(load("glDeleteSync")); 
	glClientWaitSync = reinterpret_cast<PFNGLCLIENTWAITSYNCPROC>(load("glClientWaitSync")); 
	glWaitSync = reinterpret_cast<PFNGLWAITSYNCPROC>(load("glWaitSync")); 
	glGetInteger64v = reinterpret_cast<PFNGLGETINTEGER64VPROC>(load("glGetInteger64v")); 
	glGetSynciv = reinterpret_cast<PFNGLGETSYNCIVPROC>(load("glGetSynciv")); 
	glGetInteger64i_v = reinterpret_cast<PFNGLGETINTEGER64I_VPROC>(load("glGetInteger64i_v")); 
	glTexImage2DMultisample = reinterpret_cast<PFNGLTEXIMAGE2DMULTISAMPLEPROC>(load("glTexImage2DMultisample")); 
	glTexImage3DMultisample = reinterpret_cast<PFNGLTEXIMAGE3DMULTISAMPLEPROC>(load("glTexImage3DMultisample")); 
	glGetMultisamplefv = reinterpret_cast<PFNGLGETMULTISAMPLEFVPROC>(load("glGetMultisamplefv")); 
	glSampleMaski = reinterpret_cast<PFNGLSAMPLEMASKIPROC>(load("glSampleMaski")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion3_3(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();

	if (!TRAP_OpenGL_VERSION_3_3)
		return;

	glBindFragDataLocationIndexed = reinterpret_cast<PFNGLBINDFRAGDATALOCATIONINDEXEDPROC>(load("glBindFragDataLocationIndexed")); 
	glGetFragDataIndex = reinterpret_cast<PFNGLGETFRAGDATAINDEXPROC>(load("glGetFragDataIndex")); 
	glGenSamplers = reinterpret_cast<PFNGLGENSAMPLERSPROC>(load("glGenSamplers")); 
	glDeleteSamplers = reinterpret_cast<PFNGLDELETESAMPLERSPROC>(load("glDeleteSamplers")); 
	glIsSampler = reinterpret_cast<PFNGLISSAMPLERPROC>(load("glIsSampler")); 
	glBindSampler = reinterpret_cast<PFNGLBINDSAMPLERPROC>(load("glBindSampler")); 
	glSamplerParameteri = reinterpret_cast<PFNGLSAMPLERPARAMETERIPROC>(load("glSamplerParameteri")); 
	glSamplerParameteriv = reinterpret_cast<PFNGLSAMPLERPARAMETERIVPROC>(load("glSamplerParameteriv")); 
	glSamplerParameterf = reinterpret_cast<PFNGLSAMPLERPARAMETERFPROC>(load("glSamplerParameterf")); 
	glSamplerParameterfv = reinterpret_cast<PFNGLSAMPLERPARAMETERFVPROC>(load("glSamplerParameterfv")); 
	glSamplerParameterIiv = reinterpret_cast<PFNGLSAMPLERPARAMETERIIVPROC>(load("glSamplerParameterIiv")); 
	glSamplerParameterIuiv = reinterpret_cast<PFNGLSAMPLERPARAMETERIUIVPROC>(load("glSamplerParameterIuiv")); 
	glGetSamplerParameteriv = reinterpret_cast<PFNGLGETSAMPLERPARAMETERIVPROC>(load("glGetSamplerParameteriv")); 
	glGetSamplerParameterIiv = reinterpret_cast<PFNGLGETSAMPLERPARAMETERIIVPROC>(load("glGetSamplerParameterIiv")); 
	glGetSamplerParameterfv = reinterpret_cast<PFNGLGETSAMPLERPARAMETERFVPROC>(load("glGetSamplerParameterfv")); 
	glGetSamplerParameterIuiv = reinterpret_cast<PFNGLGETSAMPLERPARAMETERIUIVPROC>(load("glGetSamplerParameterIuiv")); 
	glQueryCounter = reinterpret_cast<PFNGLQUERYCOUNTERPROC>(load("glQueryCounter")); 
	glGetQueryObjecti64v = reinterpret_cast<PFNGLGETQUERYOBJECTI64VPROC>(load("glGetQueryObjecti64v")); 
	glGetQueryObjectui64v = reinterpret_cast<PFNGLGETQUERYOBJECTUI64VPROC>(load("glGetQueryObjectui64v")); 
	glVertexAttribDivisor = reinterpret_cast<PFNGLVERTEXATTRIBDIVISORPROC>(load("glVertexAttribDivisor")); 
	glVertexAttribP1ui = reinterpret_cast<PFNGLVERTEXATTRIBP1UIPROC>(load("glVertexAttribP1ui")); 
	glVertexAttribP1uiv = reinterpret_cast<PFNGLVERTEXATTRIBP1UIVPROC>(load("glVertexAttribP1uiv"));
	glVertexAttribP2ui = reinterpret_cast<PFNGLVERTEXATTRIBP2UIPROC>(load("glVertexAttribP2ui")); 
	glVertexAttribP2uiv = reinterpret_cast<PFNGLVERTEXATTRIBP2UIVPROC>(load("glVertexAttribP2uiv"));
	glVertexAttribP3ui = reinterpret_cast<PFNGLVERTEXATTRIBP3UIPROC>(load("glVertexAttribP3ui")); 
	glVertexAttribP3uiv = reinterpret_cast<PFNGLVERTEXATTRIBP3UIVPROC>(load("glVertexAttribP3uiv"));
	glVertexAttribP4ui = reinterpret_cast<PFNGLVERTEXATTRIBP4UIPROC>(load("glVertexAttribP4ui")); 
	glVertexAttribP4uiv = reinterpret_cast<PFNGLVERTEXATTRIBP4UIVPROC>(load("glVertexAttribP4uiv"));
	glVertexP2ui = reinterpret_cast<PFNGLVERTEXP2UIPROC>(load("glVertexP2ui"));
	glVertexP2uiv = reinterpret_cast<PFNGLVERTEXP2UIVPROC>(load("glVertexP2uiv"));
	glVertexP3ui = reinterpret_cast<PFNGLVERTEXP3UIPROC>(load("glVertexP3ui"));
	glVertexP3uiv = reinterpret_cast<PFNGLVERTEXP3UIVPROC>(load("glVertexP3uiv"));
	glVertexP4ui = reinterpret_cast<PFNGLVERTEXP4UIPROC>(load("glVertexP4ui"));
	glVertexP4uiv = reinterpret_cast<PFNGLVERTEXP4UIVPROC>(load("glVertexP4uiv"));
	glTexCoordP1ui = reinterpret_cast<PFNGLTEXCOORDP1UIPROC>(load("glTexCoordP1ui"));
	glTexCoordP1uiv = reinterpret_cast<PFNGLTEXCOORDP1UIVPROC>(load("glTexCoordP1uiv"));
	glTexCoordP2ui = reinterpret_cast<PFNGLTEXCOORDP2UIPROC>(load("glTexCoordP2ui"));
	glTexCoordP2uiv = reinterpret_cast<PFNGLTEXCOORDP2UIVPROC>(load("glTexCoordP2uiv"));
	glTexCoordP3ui = reinterpret_cast<PFNGLTEXCOORDP3UIPROC>(load("glTexCoordP3ui"));
	glTexCoordP3uiv = reinterpret_cast<PFNGLTEXCOORDP3UIVPROC>(load("glTexCoordP3uiv"));
	glTexCoordP4ui = reinterpret_cast<PFNGLTEXCOORDP4UIPROC>(load("glTexCoordP4ui"));
	glTexCoordP4uiv = reinterpret_cast<PFNGLTEXCOORDP4UIVPROC>(load("glTexCoordP4uiv"));
	glMultiTexCoordP1ui = reinterpret_cast<PFNGLMULTITEXCOORDP1UIPROC>(load("glMultiTexCoordP1ui"));
	glMultiTexCoordP1uiv = reinterpret_cast<PFNGLMULTITEXCOORDP1UIVPROC>(load("glMultiTexCoordP1uiv"));
	glMultiTexCoordP2ui = reinterpret_cast<PFNGLMULTITEXCOORDP2UIPROC>(load("glMultiTexCoordP2ui"));
	glMultiTexCoordP2uiv = reinterpret_cast<PFNGLMULTITEXCOORDP2UIVPROC>(load("glMultiTexCoordP2uiv"));
	glMultiTexCoordP3ui = reinterpret_cast<PFNGLMULTITEXCOORDP3UIPROC>(load("glMultiTexCoordP3ui"));
	glMultiTexCoordP3uiv = reinterpret_cast<PFNGLMULTITEXCOORDP3UIVPROC>(load("glMultiTexCoordP3uiv"));
	glMultiTexCoordP4ui = reinterpret_cast<PFNGLMULTITEXCOORDP4UIPROC>(load("glMultiTexCoordP4ui"));
	glMultiTexCoordP4uiv = reinterpret_cast<PFNGLMULTITEXCOORDP4UIVPROC>(load("glMultiTexCoordP4uiv"));
	glNormalP3ui = reinterpret_cast<PFNGLNORMALP3UIPROC>(load("glNormalP3ui"));
	glNormalP3uiv = reinterpret_cast<PFNGLNORMALP3UIVPROC>(load("glNormalP3uiv"));
	glColorP3ui = reinterpret_cast<PFNGLCOLORP3UIPROC>(load("glColorP3ui"));
	glColorP3uiv = reinterpret_cast<PFNGLCOLORP3UIVPROC>(load("glColorP3uiv"));
	glColorP4ui = reinterpret_cast<PFNGLCOLORP4UIPROC>(load("glColorP4ui"));
	glColorP4uiv = reinterpret_cast<PFNGLCOLORP4UIVPROC>(load("glColorP4uiv"));
	glSecondaryColorP3ui = reinterpret_cast<PFNGLSECONDARYCOLORP3UIPROC>(load("glSecondaryColorP3ui"));
	glSecondaryColorP3uiv = reinterpret_cast<PFNGLSECONDARYCOLORP3UIVPROC>(load("glSecondaryColorP3uiv"));
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion4_0(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();

	if (!TRAP_OpenGL_VERSION_4_0)
		return;

	glMinSampleShading = reinterpret_cast<PFNGLMINSAMPLESHADINGPROC>(load("glMinSampleShading")); 
	glBlendEquationi = reinterpret_cast<PFNGLBLENDEQUATIONIPROC>(load("glBlendEquationi")); 
	glBlendEquationSeparatei = reinterpret_cast<PFNGLBLENDEQUATIONSEPARATEIPROC>(load("glBlendEquationSeparatei")); 
	glBlendFunci = reinterpret_cast<PFNGLBLENDFUNCIPROC>(load("glBlendFunci")); 
	glBlendFuncSeparatei = reinterpret_cast<PFNGLBLENDFUNCSEPARATEIPROC>(load("glBlendFuncSeparatei")); 
	glDrawArraysIndirect = reinterpret_cast<PFNGLDRAWARRAYSINDIRECTPROC>(load("glDrawArraysIndirect")); 
	glDrawElementsIndirect = reinterpret_cast<PFNGLDRAWELEMENTSINDIRECTPROC>(load("glDrawElementsIndirect")); 
	glUniform1d = reinterpret_cast<PFNGLUNIFORM1DPROC>(load("glUniform1d"));
	glUniform2d = reinterpret_cast<PFNGLUNIFORM2DPROC>(load("glUniform2d"));
	glUniform3d = reinterpret_cast<PFNGLUNIFORM3DPROC>(load("glUniform3d"));
	glUniform4d = reinterpret_cast<PFNGLUNIFORM4DPROC>(load("glUniform4d"));
	glUniform1dv = reinterpret_cast<PFNGLUNIFORM1DVPROC>(load("glUniform1dv"));
	glUniform2dv = reinterpret_cast<PFNGLUNIFORM2DVPROC>(load("glUniform2dv"));
	glUniform3dv = reinterpret_cast<PFNGLUNIFORM3DVPROC>(load("glUniform3dv"));
	glUniform4dv = reinterpret_cast<PFNGLUNIFORM4DVPROC>(load("glUniform4dv"));
	glUniformMatrix2dv = reinterpret_cast<PFNGLUNIFORMMATRIX2DVPROC>(load("glUniformMatrix2dv"));
	glUniformMatrix3dv = reinterpret_cast<PFNGLUNIFORMMATRIX3DVPROC>(load("glUniformMatrix3dv"));
	glUniformMatrix4dv = reinterpret_cast<PFNGLUNIFORMMATRIX4DVPROC>(load("glUniformMatrix4dv"));
	glUniformMatrix2x3dv = reinterpret_cast<PFNGLUNIFORMMATRIX2X3DVPROC>(load("glUniformMatrix2x3dv"));
	glUniformMatrix2x4dv = reinterpret_cast<PFNGLUNIFORMMATRIX2X4DVPROC>(load("glUniformMatrix2x4dv"));
	glUniformMatrix3x2dv = reinterpret_cast<PFNGLUNIFORMMATRIX3X2DVPROC>(load("glUniformMatrix3x2dv"));
	glUniformMatrix3x4dv = reinterpret_cast<PFNGLUNIFORMMATRIX3X4DVPROC>(load("glUniformMatrix3x4dv"));
	glUniformMatrix4x2dv = reinterpret_cast<PFNGLUNIFORMMATRIX4X2DVPROC>(load("glUniformMatrix4x2dv"));
	glUniformMatrix4x3dv = reinterpret_cast<PFNGLUNIFORMMATRIX4X3DVPROC>(load("glUniformMatrix4x3dv"));
	glGetUniformdv = reinterpret_cast<PFNGLGETUNIFORMDVPROC>(load("glGetUniformdv")); 
	glGetSubroutineUniformLocation = reinterpret_cast<PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC>(load("glGetSubroutineUniformLocation"));  
	glGetSubroutineIndex = reinterpret_cast<PFNGLGETSUBROUTINEINDEXPROC>(load("glGetSubroutineIndex")); 
	glGetActiveSubroutineUniformiv = reinterpret_cast<PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC>(load("glGetActiveSubroutineUniformiv")); 
	glGetActiveSubroutineUniformName = reinterpret_cast<PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC>(load("glGetActiveSubroutineUniformName")); 
	glGetActiveSubroutineName = reinterpret_cast<PFNGLGETACTIVESUBROUTINENAMEPROC>(load("glGetActiveSubroutineName")); 
	glUniformSubroutinesuiv = reinterpret_cast<PFNGLUNIFORMSUBROUTINESUIVPROC>(load("glUniformSubroutinesuiv")); 
	glGetUniformSubroutineuiv = reinterpret_cast<PFNGLGETUNIFORMSUBROUTINEUIVPROC>(load("glGetUniformSubroutineuiv")); 
	glGetProgramStageiv = reinterpret_cast<PFNGLGETPROGRAMSTAGEIVPROC>(load("glGetProgramStageiv")); 
	glPatchParameteri = reinterpret_cast<PFNGLPATCHPARAMETERIPROC>(load("glPatchParameteri")); 
	glPatchParameterfv = reinterpret_cast<PFNGLPATCHPARAMETERFVPROC>(load("glPatchParameterfv")); 
	glBindTransformFeedback = reinterpret_cast<PFNGLBINDTRANSFORMFEEDBACKPROC>(load("glBindTransformFeedback")); 
	glDeleteTransformFeedbacks = reinterpret_cast<PFNGLDELETETRANSFORMFEEDBACKSPROC>(load("glDeleteTransformFeedbacks")); 
	glGenTransformFeedbacks = reinterpret_cast<PFNGLGENTRANSFORMFEEDBACKSPROC>(load("glGenTransformFeedbacks")); 
	glIsTransformFeedback = reinterpret_cast<PFNGLISTRANSFORMFEEDBACKPROC>(load("glIsTransformFeedback")); 
	glPauseTransformFeedback = reinterpret_cast<PFNGLPAUSETRANSFORMFEEDBACKPROC>(load("glPauseTransformFeedback")); 
	glResumeTransformFeedback = reinterpret_cast<PFNGLRESUMETRANSFORMFEEDBACKPROC>(load("glResumeTransformFeedback")); 
	glDrawTransformFeedback = reinterpret_cast<PFNGLDRAWTRANSFORMFEEDBACKPROC>(load("glDrawTransformFeedback")); 
	glDrawTransformFeedbackStream = reinterpret_cast<PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC>(load("glDrawTransformFeedbackStream")); 
	glBeginQueryIndexed = reinterpret_cast<PFNGLBEGINQUERYINDEXEDPROC>(load("glBeginQueryIndexed")); 
	glEndQueryIndexed = reinterpret_cast<PFNGLENDQUERYINDEXEDPROC>(load("glEndQueryIndexed")); 
	glGetQueryIndexediv = reinterpret_cast<PFNGLGETQUERYINDEXEDIVPROC>(load("glGetQueryIndexediv")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion4_1(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();

	if (!TRAP_OpenGL_VERSION_4_1)
		return;

	glReleaseShaderCompiler = reinterpret_cast<PFNGLRELEASESHADERCOMPILERPROC>(load("glReleaseShaderCompiler")); 
	glShaderBinary = reinterpret_cast<PFNGLSHADERBINARYPROC>(load("glShaderBinary")); 
	glGetShaderPrecisionFormat = reinterpret_cast<PFNGLGETSHADERPRECISIONFORMATPROC>(load("glGetShaderPrecisionFormat")); 
	glDepthRangef = reinterpret_cast<PFNGLDEPTHRANGEFPROC>(load("glDepthRangef")); 
	glClearDepthf = reinterpret_cast<PFNGLCLEARDEPTHFPROC>(load("glClearDepthf")); 
	glGetProgramBinary = reinterpret_cast<PFNGLGETPROGRAMBINARYPROC>(load("glGetProgramBinary")); 
	glProgramBinary = reinterpret_cast<PFNGLPROGRAMBINARYPROC>(load("glProgramBinary")); 
	glProgramParameteri = reinterpret_cast<PFNGLPROGRAMPARAMETERIPROC>(load("glProgramParameteri")); 
	glUseProgramStages = reinterpret_cast<PFNGLUSEPROGRAMSTAGESPROC>(load("glUseProgramStages")); 
	glActiveShaderProgram = reinterpret_cast<PFNGLACTIVESHADERPROGRAMPROC>(load("glActiveShaderProgram")); 
	glCreateShaderProgramv = reinterpret_cast<PFNGLCREATESHADERPROGRAMVPROC>(load("glCreateShaderProgramv")); 
	glBindProgramPipeline = reinterpret_cast<PFNGLBINDPROGRAMPIPELINEPROC>(load("glBindProgramPipeline")); 
	glDeleteProgramPipelines = reinterpret_cast<PFNGLDELETEPROGRAMPIPELINESPROC>(load("glDeleteProgramPipelines")); 
	glGenProgramPipelines = reinterpret_cast<PFNGLGENPROGRAMPIPELINESPROC>(load("glGenProgramPipelines")); 
	glIsProgramPipeline = reinterpret_cast<PFNGLISPROGRAMPIPELINEPROC>(load("glIsProgramPipeline")); 
	glGetProgramPipelineiv = reinterpret_cast<PFNGLGETPROGRAMPIPELINEIVPROC>(load("glGetProgramPipelineiv")); 
	glProgramUniform1i = reinterpret_cast<PFNGLPROGRAMUNIFORM1IPROC>(load("glProgramUniform1i")); 
	glProgramUniform1iv = reinterpret_cast<PFNGLPROGRAMUNIFORM1IVPROC>(load("glProgramUniform1iv")); 
	glProgramUniform1f = reinterpret_cast<PFNGLPROGRAMUNIFORM1FPROC>(load("glProgramUniform1f")); 
	glProgramUniform1fv = reinterpret_cast<PFNGLPROGRAMUNIFORM1FVPROC>(load("glProgramUniform1fv")); 
	glProgramUniform1d = reinterpret_cast<PFNGLPROGRAMUNIFORM1DPROC>(load("glProgramUniform1d"));
	glProgramUniform1dv = reinterpret_cast<PFNGLPROGRAMUNIFORM1DVPROC>(load("glProgramUniform1dv"));
	glProgramUniform1ui = reinterpret_cast<PFNGLPROGRAMUNIFORM1UIPROC>(load("glProgramUniform1ui")); 
	glProgramUniform1uiv = reinterpret_cast<PFNGLPROGRAMUNIFORM1UIVPROC>(load("glProgramUniform1uiv")); 
	glProgramUniform2i = reinterpret_cast<PFNGLPROGRAMUNIFORM2IPROC>(load("glProgramUniform2i")); 
	glProgramUniform2iv = reinterpret_cast<PFNGLPROGRAMUNIFORM2IVPROC>(load("glProgramUniform2iv")); 
	glProgramUniform2f = reinterpret_cast<PFNGLPROGRAMUNIFORM2FPROC>(load("glProgramUniform2f")); 
	glProgramUniform2fv = reinterpret_cast<PFNGLPROGRAMUNIFORM2FVPROC>(load("glProgramUniform2fv")); 
	glProgramUniform2d = reinterpret_cast<PFNGLPROGRAMUNIFORM2DPROC>(load("glProgramUniform2d"));
	glProgramUniform2dv = reinterpret_cast<PFNGLPROGRAMUNIFORM2DVPROC>(load("glProgramUniform2dv"));
	glProgramUniform2ui = reinterpret_cast<PFNGLPROGRAMUNIFORM2UIPROC>(load("glProgramUniform2ui")); 
	glProgramUniform2uiv = reinterpret_cast<PFNGLPROGRAMUNIFORM2UIVPROC>(load("glProgramUniform2uiv")); 
	glProgramUniform3i = reinterpret_cast<PFNGLPROGRAMUNIFORM3IPROC>(load("glProgramUniform3i")); 
	glProgramUniform3iv = reinterpret_cast<PFNGLPROGRAMUNIFORM3IVPROC>(load("glProgramUniform3iv")); 
	glProgramUniform3f = reinterpret_cast<PFNGLPROGRAMUNIFORM3FPROC>(load("glProgramUniform3f")); 
	glProgramUniform3fv = reinterpret_cast<PFNGLPROGRAMUNIFORM3FVPROC>(load("glProgramUniform3fv")); 
	glProgramUniform3d = reinterpret_cast<PFNGLPROGRAMUNIFORM3DPROC>(load("glProgramUniform3d"));
	glProgramUniform3dv = reinterpret_cast<PFNGLPROGRAMUNIFORM3DVPROC>(load("glProgramUniform3dv"));
	glProgramUniform3ui = reinterpret_cast<PFNGLPROGRAMUNIFORM3UIPROC>(load("glProgramUniform3ui")); 
	glProgramUniform3uiv = reinterpret_cast<PFNGLPROGRAMUNIFORM3UIVPROC>(load("glProgramUniform3uiv")); 
	glProgramUniform4i = reinterpret_cast<PFNGLPROGRAMUNIFORM4IPROC>(load("glProgramUniform4i")); 
	glProgramUniform4iv = reinterpret_cast<PFNGLPROGRAMUNIFORM4IVPROC>(load("glProgramUniform4iv")); 
	glProgramUniform4f = reinterpret_cast<PFNGLPROGRAMUNIFORM4FPROC>(load("glProgramUniform4f")); 
	glProgramUniform4fv = reinterpret_cast<PFNGLPROGRAMUNIFORM4FVPROC>(load("glProgramUniform4fv")); 
	glProgramUniform4d = reinterpret_cast<PFNGLPROGRAMUNIFORM4DPROC>(load("glProgramUniform4d"));
	glProgramUniform4dv = reinterpret_cast<PFNGLPROGRAMUNIFORM4DVPROC>(load("glProgramUniform4dv"));
	glProgramUniform4ui = reinterpret_cast<PFNGLPROGRAMUNIFORM4UIPROC>(load("glProgramUniform4ui")); 
	glProgramUniform4uiv = reinterpret_cast<PFNGLPROGRAMUNIFORM4UIVPROC>(load("glProgramUniform4uiv")); 
	glProgramUniformMatrix2fv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX2FVPROC>(load("glProgramUniformMatrix2fv")); 
	glProgramUniformMatrix3fv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX3FVPROC>(load("glProgramUniformMatrix3fv")); 
	glProgramUniformMatrix4fv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX4FVPROC>(load("glProgramUniformMatrix4fv")); 
	glProgramUniformMatrix2dv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX2DVPROC>(load("glProgramUniformMatrix2dv"));
	glProgramUniformMatrix3dv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX3DVPROC>(load("glProgramUniformMatrix3dv"));
	glProgramUniformMatrix4dv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX4DVPROC>(load("glProgramUniformMatrix4dv"));
	glProgramUniformMatrix2x3fv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC>(load("glProgramUniformMatrix2x3fv")); 
	glProgramUniformMatrix3x2fv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC>(load("glProgramUniformMatrix3x2fv")); 
	glProgramUniformMatrix2x4fv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC>(load("glProgramUniformMatrix2x4fv")); 
	glProgramUniformMatrix4x2fv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC>(load("glProgramUniformMatrix4x2fv")); 
	glProgramUniformMatrix3x4fv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC>(load("glProgramUniformMatrix3x4fv")); 
	glProgramUniformMatrix4x3fv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC>(load("glProgramUniformMatrix4x3fv")); 
	glProgramUniformMatrix2x3dv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC>(load("glProgramUniformMatrix2x3dv"));
	glProgramUniformMatrix3x2dv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC>(load("glProgramUniformMatrix3x2dv"));
	glProgramUniformMatrix2x4dv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC>(load("glProgramUniformMatrix2x4dv"));
	glProgramUniformMatrix4x2dv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC>(load("glProgramUniformMatrix4x2dv"));
	glProgramUniformMatrix3x4dv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC>(load("glProgramUniformMatrix3x4dv"));
	glProgramUniformMatrix4x3dv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC>(load("glProgramUniformMatrix4x3dv"));
	glValidateProgramPipeline = reinterpret_cast<PFNGLVALIDATEPROGRAMPIPELINEPROC>(load("glValidateProgramPipeline")); 
	glGetProgramPipelineInfoLog = reinterpret_cast<PFNGLGETPROGRAMPIPELINEINFOLOGPROC>(load("glGetProgramPipelineInfoLog")); 
	glVertexAttribL1d = reinterpret_cast<PFNGLVERTEXATTRIBL1DPROC>(load("glVertexAttribL1d")); 
	glVertexAttribL2d = reinterpret_cast<PFNGLVERTEXATTRIBL2DPROC>(load("glVertexAttribL2d")); 
	glVertexAttribL3d = reinterpret_cast<PFNGLVERTEXATTRIBL3DPROC>(load("glVertexAttribL3d")); 
	glVertexAttribL4d = reinterpret_cast<PFNGLVERTEXATTRIBL4DPROC>(load("glVertexAttribL4d")); 
	glVertexAttribL1dv = reinterpret_cast<PFNGLVERTEXATTRIBL1DVPROC>(load("glVertexAttribL1dv")); 
	glVertexAttribL2dv = reinterpret_cast<PFNGLVERTEXATTRIBL2DVPROC>(load("glVertexAttribL2dv")); 
	glVertexAttribL3dv = reinterpret_cast<PFNGLVERTEXATTRIBL3DVPROC>(load("glVertexAttribL3dv")); 
	glVertexAttribL4dv = reinterpret_cast<PFNGLVERTEXATTRIBL4DVPROC>(load("glVertexAttribL4dv")); 
	glVertexAttribLPointer = reinterpret_cast<PFNGLVERTEXATTRIBLPOINTERPROC>(load("glVertexAttribLPointer")); 
	glGetVertexAttribLdv = reinterpret_cast<PFNGLGETVERTEXATTRIBLDVPROC>(load("glGetVertexAttribLdv")); 
	glViewportArrayv = reinterpret_cast<PFNGLVIEWPORTARRAYVPROC>(load("glViewportArrayv")); 
	glViewportIndexedf = reinterpret_cast<PFNGLVIEWPORTINDEXEDFPROC>(load("glViewportIndexedf")); 
	glViewportIndexedfv = reinterpret_cast<PFNGLVIEWPORTINDEXEDFVPROC>(load("glViewportIndexedfv")); 
	glScissorArrayv = reinterpret_cast<PFNGLSCISSORARRAYVPROC>(load("glScissorArrayv")); 
	glScissorIndexed = reinterpret_cast<PFNGLSCISSORINDEXEDPROC>(load("glScissorIndexed")); 
	glScissorIndexedv = reinterpret_cast<PFNGLSCISSORINDEXEDVPROC>(load("glScissorIndexedv")); 
	glDepthRangeArrayv = reinterpret_cast<PFNGLDEPTHRANGEARRAYVPROC>(load("glDepthRangeArrayv")); 
	glDepthRangeIndexed = reinterpret_cast<PFNGLDEPTHRANGEINDEXEDPROC>(load("glDepthRangeIndexed")); 
	glGetFloati_v = reinterpret_cast<PFNGLGETFLOATI_VPROC>(load("glGetFloati_v")); 
	glGetDoublei_v = reinterpret_cast<PFNGLGETDOUBLEI_VPROC>(load("glGetDoublei_v")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion4_2(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();

	if (!TRAP_OpenGL_VERSION_4_2)
		return;

	glDrawArraysInstancedBaseInstance = reinterpret_cast<PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC>(load("glDrawArraysInstancedBaseInstance")); 
	glDrawElementsInstancedBaseInstance = reinterpret_cast<PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC>(load("glDrawElementsInstancedBaseInstance")); 
	glDrawElementsInstancedBaseVertexBaseInstance = reinterpret_cast<PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC>(load("glDrawElementsInstancedBaseVertexBaseInstance")); 
	glGetInternalformativ = reinterpret_cast<PFNGLGETINTERNALFORMATIVPROC>(load("glGetInternalformativ")); 
	glGetActiveAtomicCounterBufferiv = reinterpret_cast<PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC>(load("glGetActiveAtomicCounterBufferiv")); 
	glBindImageTexture = reinterpret_cast<PFNGLBINDIMAGETEXTUREPROC>(load("glBindImageTexture")); 
	glMemoryBarrier = reinterpret_cast<PFNGLMEMORYBARRIERPROC>(load("glMemoryBarrier")); 
	glDrawTransformFeedbackInstanced = reinterpret_cast<PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC>(load("glDrawTransformFeedbackInstanced")); 
	glDrawTransformFeedbackStreamInstanced = reinterpret_cast<PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC>(load("glDrawTransformFeedbackStreamInstanced")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion4_3(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();

	if (!TRAP_OpenGL_VERSION_4_3)
		return;

	glDispatchCompute = reinterpret_cast<PFNGLDISPATCHCOMPUTEPROC>(load("glDispatchCompute")); 
	glDispatchComputeIndirect = reinterpret_cast<PFNGLDISPATCHCOMPUTEINDIRECTPROC>(load("glDispatchComputeIndirect")); 
	glCopyImageSubData = reinterpret_cast<PFNGLCOPYIMAGESUBDATAPROC>(load("glCopyImageSubData")); 
	glGetInternalformati64v = reinterpret_cast<PFNGLGETINTERNALFORMATI64VPROC>(load("glGetInternalformati64v")); 
	glInvalidateTexSubImage = reinterpret_cast<PFNGLINVALIDATETEXSUBIMAGEPROC>(load("glInvalidateTexSubImage")); 
	glInvalidateTexImage = reinterpret_cast<PFNGLINVALIDATETEXIMAGEPROC>(load("glInvalidateTexImage")); 
	glInvalidateBufferSubData = reinterpret_cast<PFNGLINVALIDATEBUFFERSUBDATAPROC>(load("glInvalidateBufferSubData")); 
	glInvalidateBufferData = reinterpret_cast<PFNGLINVALIDATEBUFFERDATAPROC>(load("glInvalidateBufferData")); 
	glMultiDrawArraysIndirect = reinterpret_cast<PFNGLMULTIDRAWARRAYSINDIRECTPROC>(load("glMultiDrawArraysIndirect")); 
	glMultiDrawElementsIndirect = reinterpret_cast<PFNGLMULTIDRAWELEMENTSINDIRECTPROC>(load("glMultiDrawElementsIndirect")); 
	glGetProgramInterfaceiv = reinterpret_cast<PFNGLGETPROGRAMINTERFACEIVPROC>(load("glGetProgramInterfaceiv")); 
	glGetProgramResourceIndex = reinterpret_cast<PFNGLGETPROGRAMRESOURCEINDEXPROC>(load("glGetProgramResourceIndex")); 
	glGetProgramResourceName = reinterpret_cast<PFNGLGETPROGRAMRESOURCENAMEPROC>(load("glGetProgramResourceName")); 
	glGetProgramResourceiv = reinterpret_cast<PFNGLGETPROGRAMRESOURCEIVPROC>(load("glGetProgramResourceiv")); 
	glGetProgramResourceLocation = reinterpret_cast<PFNGLGETPROGRAMRESOURCELOCATIONPROC>(load("glGetProgramResourceLocation")); 
	glGetProgramResourceLocationIndex = reinterpret_cast<PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC>(load("glGetProgramResourceLocationIndex")); 
	glShaderStorageBlockBinding = reinterpret_cast<PFNGLSHADERSTORAGEBLOCKBINDINGPROC>(load("glShaderStorageBlockBinding")); 
	glTextureView = reinterpret_cast<PFNGLTEXTUREVIEWPROC>(load("glTextureView")); 
	glDebugMessageControl = reinterpret_cast<PFNGLDEBUGMESSAGECONTROLPROC>(load("glDebugMessageControl")); 
	glDebugMessageInsert = reinterpret_cast<PFNGLDEBUGMESSAGEINSERTPROC>(load("glDebugMessageInsert")); 
	glDebugMessageCallback = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKPROC>(load("glDebugMessageCallback")); 
	glGetDebugMessageLog = reinterpret_cast<PFNGLGETDEBUGMESSAGELOGPROC>(load("glGetDebugMessageLog")); 
	glPushDebugGroup = reinterpret_cast<PFNGLPUSHDEBUGGROUPPROC>(load("glPushDebugGroup")); 
	glPopDebugGroup = reinterpret_cast<PFNGLPOPDEBUGGROUPPROC>(load("glPopDebugGroup")); 
	glObjectLabel = reinterpret_cast<PFNGLOBJECTLABELPROC>(load("glObjectLabel")); 
	glGetObjectLabel = reinterpret_cast<PFNGLGETOBJECTLABELPROC>(load("glGetObjectLabel")); 
	glObjectPtrLabel = reinterpret_cast<PFNGLOBJECTPTRLABELPROC>(load("glObjectPtrLabel")); 
	glGetObjectPtrLabel = reinterpret_cast<PFNGLGETOBJECTPTRLABELPROC>(load("glGetObjectPtrLabel")); 
	glGetPointerv = reinterpret_cast<PFNGLGETPOINTERVPROC>(load("glGetPointerv")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion4_4(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();

	if (!TRAP_OpenGL_VERSION_4_4)
		return;

	glClearTexImage = reinterpret_cast<PFNGLCLEARTEXIMAGEPROC>(load("glClearTexImage")); 
	glClearTexSubImage = reinterpret_cast<PFNGLCLEARTEXSUBIMAGEPROC>(load("glClearTexSubImage")); 
	glBindBuffersBase = reinterpret_cast<PFNGLBINDBUFFERSBASEPROC>(load("glBindBuffersBase")); 
	glBindBuffersRange = reinterpret_cast<PFNGLBINDBUFFERSRANGEPROC>(load("glBindBuffersRange")); 
	glBindTextures = reinterpret_cast<PFNGLBINDTEXTURESPROC>(load("glBindTextures")); 
	glBindSamplers = reinterpret_cast<PFNGLBINDSAMPLERSPROC>(load("glBindSamplers")); 
	glBindImageTextures = reinterpret_cast<PFNGLBINDIMAGETEXTURESPROC>(load("glBindImageTextures")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion4_5(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();

	if (!TRAP_OpenGL_VERSION_4_5)
		return;

	glClipControl = reinterpret_cast<PFNGLCLIPCONTROLPROC>(load("glClipControl")); 
	glCreateTransformFeedbacks = reinterpret_cast<PFNGLCREATETRANSFORMFEEDBACKSPROC>(load("glCreateTransformFeedbacks")); 
	glTransformFeedbackBufferBase = reinterpret_cast<PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC>(load("glTransformFeedbackBufferBase")); 
	glTransformFeedbackBufferRange = reinterpret_cast<PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC>(load("glTransformFeedbackBufferRange")); 
	glGetTransformFeedbackiv = reinterpret_cast<PFNGLGETTRANSFORMFEEDBACKIVPROC>(load("glGetTransformFeedbackiv")); 
	glGetTransformFeedbacki_v = reinterpret_cast<PFNGLGETTRANSFORMFEEDBACKI_VPROC>(load("glGetTransformFeedbacki_v")); 
	glGetTransformFeedbacki64_v = reinterpret_cast<PFNGLGETTRANSFORMFEEDBACKI64_VPROC>(load("glGetTransformFeedbacki64_v")); 
	glCreateBuffers = reinterpret_cast<PFNGLCREATEBUFFERSPROC>(load("glCreateBuffers")); 
	glNamedBufferStorage = reinterpret_cast<PFNGLNAMEDBUFFERSTORAGEPROC>(load("glNamedBufferStorage")); 
	glNamedBufferData = reinterpret_cast<PFNGLNAMEDBUFFERDATAPROC>(load("glNamedBufferData")); 
	glNamedBufferSubData = reinterpret_cast<PFNGLNAMEDBUFFERSUBDATAPROC>(load("glNamedBufferSubData")); 
	glCopyNamedBufferSubData = reinterpret_cast<PFNGLCOPYNAMEDBUFFERSUBDATAPROC>(load("glCopyNamedBufferSubData")); 
	glClearNamedBufferData = reinterpret_cast<PFNGLCLEARNAMEDBUFFERDATAPROC>(load("glClearNamedBufferData")); 
	glClearNamedBufferSubData = reinterpret_cast<PFNGLCLEARNAMEDBUFFERSUBDATAPROC>(load("glClearNamedBufferSubData")); 
	glMapNamedBuffer = reinterpret_cast<PFNGLMAPNAMEDBUFFERPROC>(load("glMapNamedBuffer")); 
	glMapNamedBufferRange = reinterpret_cast<PFNGLMAPNAMEDBUFFERRANGEPROC>(load("glMapNamedBufferRange")); 
	glUnmapNamedBuffer = reinterpret_cast<PFNGLUNMAPNAMEDBUFFERPROC>(load("glUnmapNamedBuffer")); 
	glFlushMappedNamedBufferRange = reinterpret_cast<PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC>(load("glFlushMappedNamedBufferRange")); 
	glGetNamedBufferParameteriv = reinterpret_cast<PFNGLGETNAMEDBUFFERPARAMETERIVPROC>(load("glGetNamedBufferParameteriv")); 
	glGetNamedBufferParameteri64v = reinterpret_cast<PFNGLGETNAMEDBUFFERPARAMETERI64VPROC>(load("glGetNamedBufferParameteri64v")); 
	glGetNamedBufferPointerv = reinterpret_cast<PFNGLGETNAMEDBUFFERPOINTERVPROC>(load("glGetNamedBufferPointerv")); 
	glGetNamedBufferSubData = reinterpret_cast<PFNGLGETNAMEDBUFFERSUBDATAPROC>(load("glGetNamedBufferSubData")); 
	glCreateFramebuffers = reinterpret_cast<PFNGLCREATEFRAMEBUFFERSPROC>(load("glCreateFramebuffers")); 
	glNamedFramebufferRenderbuffer = reinterpret_cast<PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC>(load("glNamedFramebufferRenderbuffer")); 
	glNamedFramebufferParameteri = reinterpret_cast<PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC>(load("glNamedFramebufferParameteri")); 
	glNamedFramebufferTexture = reinterpret_cast<PFNGLNAMEDFRAMEBUFFERTEXTUREPROC>(load("glNamedFramebufferTexture")); 
	glNamedFramebufferTextureLayer = reinterpret_cast<PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC>(load("glNamedFramebufferTextureLayer")); 
	glNamedFramebufferDrawBuffer = reinterpret_cast<PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC>(load("glNamedFramebufferDrawBuffer")); 
	glNamedFramebufferDrawBuffers = reinterpret_cast<PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC>(load("glNamedFramebufferDrawBuffers")); 
	glNamedFramebufferReadBuffer = reinterpret_cast<PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC>(load("glNamedFramebufferReadBuffer")); 
	glInvalidateNamedFramebufferData = reinterpret_cast<PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC>(load("glInvalidateNamedFramebufferData")); 
	glInvalidateNamedFramebufferSubData = reinterpret_cast<PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC>(load("glInvalidateNamedFramebufferSubData")); 
	glClearNamedFramebufferiv = reinterpret_cast<PFNGLCLEARNAMEDFRAMEBUFFERIVPROC>(load("glClearNamedFramebufferiv")); 
	glClearNamedFramebufferuiv = reinterpret_cast<PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC>(load("glClearNamedFramebufferuiv")); 
	glClearNamedFramebufferfv = reinterpret_cast<PFNGLCLEARNAMEDFRAMEBUFFERFVPROC>(load("glClearNamedFramebufferfv")); 
	glClearNamedFramebufferfi = reinterpret_cast<PFNGLCLEARNAMEDFRAMEBUFFERFIPROC>(load("glClearNamedFramebufferfi")); 
	glBlitNamedFramebuffer = reinterpret_cast<PFNGLBLITNAMEDFRAMEBUFFERPROC>(load("glBlitNamedFramebuffer")); 
	glCheckNamedFramebufferStatus = reinterpret_cast<PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC>(load("glCheckNamedFramebufferStatus")); 
	glGetNamedFramebufferParameteriv = reinterpret_cast<PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC>(load("glGetNamedFramebufferParameteriv")); 
	glGetNamedFramebufferAttachmentParameteriv = reinterpret_cast<PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC>(load("glGetNamedFramebufferAttachmentParameteriv")); 
	glCreateRenderbuffers = reinterpret_cast<PFNGLCREATERENDERBUFFERSPROC>(load("glCreateRenderbuffers")); 
	glNamedRenderbufferStorage = reinterpret_cast<PFNGLNAMEDRENDERBUFFERSTORAGEPROC>(load("glNamedRenderbufferStorage")); 
	glNamedRenderbufferStorageMultisample = reinterpret_cast<PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC>(load("glNamedRenderbufferStorageMultisample")); 
	glGetNamedRenderbufferParameteriv = reinterpret_cast<PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC>(load("glGetNamedRenderbufferParameteriv")); 
	glCreateTextures = reinterpret_cast<PFNGLCREATETEXTURESPROC>(load("glCreateTextures")); 
	glTextureBuffer = reinterpret_cast<PFNGLTEXTUREBUFFERPROC>(load("glTextureBuffer")); 
	glTextureBufferRange = reinterpret_cast<PFNGLTEXTUREBUFFERRANGEPROC>(load("glTextureBufferRange")); 
	glTextureStorage1D = reinterpret_cast<PFNGLTEXTURESTORAGE1DPROC>(load("glTextureStorage1D")); 
	glTextureStorage2D = reinterpret_cast<PFNGLTEXTURESTORAGE2DPROC>(load("glTextureStorage2D")); 
	glTextureStorage3D = reinterpret_cast<PFNGLTEXTURESTORAGE3DPROC>(load("glTextureStorage3D")); 
	glTextureStorage2DMultisample = reinterpret_cast<PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC>(load("glTextureStorage2DMultisample")); 
	glTextureStorage3DMultisample = reinterpret_cast<PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC>(load("glTextureStorage3DMultisample")); 
	glTextureSubImage1D = reinterpret_cast<PFNGLTEXTURESUBIMAGE1DPROC>(load("glTextureSubImage1D")); 
	glTextureSubImage2D = reinterpret_cast<PFNGLTEXTURESUBIMAGE2DPROC>(load("glTextureSubImage2D"));
	glTextureSubImage3D = reinterpret_cast<PFNGLTEXTURESUBIMAGE3DPROC>(load("glTextureSubImage3D"));
	glCompressedTextureSubImage1D = reinterpret_cast<PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC>(load("glCompressedTextureSubImage1D"));
	glCompressedTextureSubImage2D = reinterpret_cast<PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC>(load("glCompressedTextureSubImage2D"));
	glCompressedTextureSubImage3D = reinterpret_cast<PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC>(load("glCompressedTextureSubImage3D"));
	glCopyTextureSubImage1D = reinterpret_cast<PFNGLCOPYTEXTURESUBIMAGE1DPROC>(load("glCopyTextureSubImage1D"));
	glCopyTextureSubImage2D = reinterpret_cast<PFNGLCOPYTEXTURESUBIMAGE2DPROC>(load("glCopyTextureSubImage2D"));
	glCopyTextureSubImage3D = reinterpret_cast<PFNGLCOPYTEXTURESUBIMAGE3DPROC>(load("glCopyTextureSubImage3D"));
	glTextureParameterf = reinterpret_cast<PFNGLTEXTUREPARAMETERFPROC>(load("glTextureParameterf")); 
	glTextureParameterfv = reinterpret_cast<PFNGLTEXTUREPARAMETERFVPROC>(load("glTextureParameterfv")); 
	glTextureParameteri = reinterpret_cast<PFNGLTEXTUREPARAMETERIPROC>(load("glTextureParameteri")); 
	glTextureParameterIiv = reinterpret_cast<PFNGLTEXTUREPARAMETERIIVPROC>(load("glTextureParameterIiv")); 
	glTextureParameterIuiv = reinterpret_cast<PFNGLTEXTUREPARAMETERIUIVPROC>(load("glTextureParameterIuiv")); 
	glTextureParameteriv = reinterpret_cast<PFNGLTEXTUREPARAMETERIVPROC>(load("glTextureParameteriv")); 
	glGenerateTextureMipmap = reinterpret_cast<PFNGLGENERATETEXTUREMIPMAPPROC>(load("glGenerateTextureMipmap")); 
	glBindTextureUnit = reinterpret_cast<PFNGLBINDTEXTUREUNITPROC>(load("glBindTextureUnit")); 
	glGetTextureImage = reinterpret_cast<PFNGLGETTEXTUREIMAGEPROC>(load("glGetTextureImage")); 
	glGetCompressedTextureImage = reinterpret_cast<PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC>(load("glGetCompressedTextureImage")); 
	glGetTextureLevelParameterfv = reinterpret_cast<PFNGLGETTEXTURELEVELPARAMETERFVPROC>(load("glGetTextureLevelParameterfv")); 
	glGetTextureLevelParameteriv = reinterpret_cast<PFNGLGETTEXTURELEVELPARAMETERIVPROC>(load("glGetTextureLevelParameteriv")); 
	glGetTextureParameterfv = reinterpret_cast<PFNGLGETTEXTUREPARAMETERFVPROC>(load("glGetTextureParameterfv")); 
	glGetTextureParameterIiv = reinterpret_cast<PFNGLGETTEXTUREPARAMETERIIVPROC>(load("glGetTextureParameterIiv")); 
	glGetTextureParameterIuiv = reinterpret_cast<PFNGLGETTEXTUREPARAMETERIUIVPROC>(load("glGetTextureParameterIuiv")); 
	glGetTextureParameteriv = reinterpret_cast<PFNGLGETTEXTUREPARAMETERIVPROC>(load("glGetTextureParameteriv")); 
	glCreateVertexArrays = reinterpret_cast<PFNGLCREATEVERTEXARRAYSPROC>(load("glCreateVertexArrays")); 
	glDisableVertexArrayAttrib = reinterpret_cast<PFNGLDISABLEVERTEXARRAYATTRIBPROC>(load("glDisableVertexArrayAttrib")); 
	glEnableVertexArrayAttrib = reinterpret_cast<PFNGLENABLEVERTEXARRAYATTRIBPROC>(load("glEnableVertexArrayAttrib")); 
	glVertexArrayElementBuffer = reinterpret_cast<PFNGLVERTEXARRAYELEMENTBUFFERPROC>(load("glVertexArrayElementBuffer")); 
	glVertexArrayVertexBuffer = reinterpret_cast<PFNGLVERTEXARRAYVERTEXBUFFERPROC>(load("glVertexArrayVertexBuffer")); 
	glVertexArrayVertexBuffers = reinterpret_cast<PFNGLVERTEXARRAYVERTEXBUFFERSPROC>(load("glVertexArrayVertexBuffers")); 
	glVertexArrayAttribBinding = reinterpret_cast<PFNGLVERTEXARRAYATTRIBBINDINGPROC>(load("glVertexArrayAttribBinding")); 
	glVertexArrayAttribFormat = reinterpret_cast<PFNGLVERTEXARRAYATTRIBFORMATPROC>(load("glVertexArrayAttribFormat")); 
	glVertexArrayAttribIFormat = reinterpret_cast<PFNGLVERTEXARRAYATTRIBIFORMATPROC>(load("glVertexArrayAttribIFormat")); 
	glVertexArrayAttribLFormat = reinterpret_cast<PFNGLVERTEXARRAYATTRIBLFORMATPROC>(load("glVertexArrayAttribLFormat")); 
	glVertexArrayBindingDivisor = reinterpret_cast<PFNGLVERTEXARRAYBINDINGDIVISORPROC>(load("glVertexArrayBindingDivisor")); 
	glGetVertexArrayiv = reinterpret_cast<PFNGLGETVERTEXARRAYIVPROC>(load("glGetVertexArrayiv")); 
	glGetVertexArrayIndexediv = reinterpret_cast<PFNGLGETVERTEXARRAYINDEXEDIVPROC>(load("glGetVertexArrayIndexediv")); 
	glGetVertexArrayIndexed64iv = reinterpret_cast<PFNGLGETVERTEXARRAYINDEXED64IVPROC>(load("glGetVertexArrayIndexed64iv")); 
	glCreateSamplers = reinterpret_cast<PFNGLCREATESAMPLERSPROC>(load("glCreateSamplers")); 
	glCreateProgramPipelines = reinterpret_cast<PFNGLCREATEPROGRAMPIPELINESPROC>(load("glCreateProgramPipelines")); 
	glCreateQueries = reinterpret_cast<PFNGLCREATEQUERIESPROC>(load("glCreateQueries")); 
	glGetQueryBufferObjecti64v = reinterpret_cast<PFNGLGETQUERYBUFFEROBJECTI64VPROC>(load("glGetQueryBufferObjecti64v"));
	glGetQueryBufferObjectiv = reinterpret_cast<PFNGLGETQUERYBUFFEROBJECTIVPROC>(load("glGetQueryBufferObjectiv"));
	glGetQueryBufferObjectui64v = reinterpret_cast<PFNGLGETQUERYBUFFEROBJECTUI64VPROC>(load("glGetQueryBufferObjectui64v"));
	glGetQueryBufferObjectuiv = reinterpret_cast<PFNGLGETQUERYBUFFEROBJECTUIVPROC>(load("glGetQueryBufferObjectuiv"));
	glMemoryBarrierByRegion = reinterpret_cast<PFNGLMEMORYBARRIERBYREGIONPROC>(load("glMemoryBarrierByRegion")); 
	glGetTextureSubImage = reinterpret_cast<PFNGLGETTEXTURESUBIMAGEPROC>(load("glGetTextureSubImage")); 
	glGetCompressedTextureSubImage = reinterpret_cast<PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC>(load("glGetCompressedTextureSubImage")); 
	glGetGraphicsResetStatus = reinterpret_cast<PFNGLGETGRAPHICSRESETSTATUSPROC>(load("glGetGraphicsResetStatus")); 
	glGetnUniformdv = reinterpret_cast<PFNGLGETNUNIFORMDVPROC>(load("glGetnUniformdv")); 
	glGetnUniformfv = reinterpret_cast<PFNGLGETNUNIFORMFVPROC>(load("glGetnUniformfv")); 
	glGetnUniformiv = reinterpret_cast<PFNGLGETNUNIFORMIVPROC>(load("glGetnUniformiv")); 
	glGetnUniformuiv = reinterpret_cast<PFNGLGETNUNIFORMUIVPROC>(load("glGetnUniformuiv")); 
	glReadnPixels = reinterpret_cast<PFNGLREADNPIXELSPROC>(load("glReadnPixels")); 
	glGetnMapdv = reinterpret_cast<PFNGLGETNMAPDVPROC>(load("glGetnMapdv"));
	glGetnMapfv = reinterpret_cast<PFNGLGETNMAPFVPROC>(load("glGetnMapfv"));
	glGetnMapiv = reinterpret_cast<PFNGLGETNMAPIVPROC>(load("glGetnMapiv"));
	glGetnPixelMapfv = reinterpret_cast<PFNGLGETNPIXELMAPFVPROC>(load("glGetnPixelMapfv"));
	glGetnPixelMapuiv = reinterpret_cast<PFNGLGETNPIXELMAPUIVPROC>(load("glGetnPixelMapuiv"));
	glGetnPixelMapusv = reinterpret_cast<PFNGLGETNPIXELMAPUSVPROC>(load("glGetnPixelMapusv"));
	glGetnPolygonStipple = reinterpret_cast<PFNGLGETNPOLYGONSTIPPLEPROC>(load("glGetnPolygonStipple"));
	glGetnColorTable = reinterpret_cast<PFNGLGETNCOLORTABLEPROC>(load("glGetnColorTable"));
	glGetnConvolutionFilter = reinterpret_cast<PFNGLGETNCONVOLUTIONFILTERPROC>(load("glGetnConvolutionFilter"));
	glGetnSeparableFilter = reinterpret_cast<PFNGLGETNSEPARABLEFILTERPROC>(load("glGetnSeparableFilter"));
	glGetnHistogram = reinterpret_cast<PFNGLGETNHISTOGRAMPROC>(load("glGetnHistogram"));
	glGetnMinmax = reinterpret_cast<PFNGLGETNMINMAXPROC>(load("glGetnMinmax"));
	glTextureBarrier = reinterpret_cast<PFNGLTEXTUREBARRIERPROC>(load("glTextureBarrier")); 
}

//-------------------------------------------------------------------------------------------------------------------//

static void LoadOpenGLVersion4_6(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();

	if (!TRAP_OpenGL_VERSION_4_6)
		return;

	glSpecializeShader = reinterpret_cast<PFNGLSPECIALIZESHADERPROC>(load("glSpecializeShader"));
	glMultiDrawArraysIndirectCount = reinterpret_cast<PFNGLMULTIDRAWARRAYSINDIRECTCOUNTPROC>(load("glMultiDrawArraysIndirectCount"));
	glMultiDrawElementsIndirectCount = reinterpret_cast<PFNGLMULTIDRAWELEMENTSINDIRECTCOUNTPROC>(load("glMultiDrawElementsIndirectCount"));
	glPolygonOffsetClamp = reinterpret_cast<PFNGLPOLYGONOFFSETCLAMPPROC>(load("glPolygonOffsetClamp"));
}

//-------------------------------------------------------------------------------------------------------------------//

static bool FindExtensionsOpenGL()
{
	TP_PROFILE_FUNCTION();

	if (!GetExtensions())
		return false;
	(void)&HasExtension;
	ExtensionsI.clear();
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

static void FindCoreOpenGL()
{
	TP_PROFILE_FUNCTION();

	const std::string version(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	if (version.empty())
		return;

	std::vector<std::string> splittedVersion = TRAP::Utils::String::SplitString(version, '.');

	if (splittedVersion.empty() || splittedVersion.size() < 2)
	{
		TP_ERROR(TRAP::Log::ContextOpenGLPrefix, "Failed to split version string!");
		return;
	}

	const int32_t major = std::stoi(splittedVersion[0]);
	const int32_t minor = std::stoi(splittedVersion[1]);

	OpenGLVersion.Major = major;
	OpenGLVersion.Minor = minor;
	MaxLoadedMajor = major;
	MaxLoadedMinor = minor;
	
	TRAP_OpenGL_VERSION_1_0 = (major == 1 && minor >= 0) || major > 1;
	TRAP_OpenGL_VERSION_1_1 = (major == 1 && minor >= 1) || major > 1;
	TRAP_OpenGL_VERSION_1_2 = (major == 1 && minor >= 2) || major > 1;
	TRAP_OpenGL_VERSION_1_3 = (major == 1 && minor >= 3) || major > 1;
	TRAP_OpenGL_VERSION_1_4 = (major == 1 && minor >= 4) || major > 1;
	TRAP_OpenGL_VERSION_1_5 = (major == 1 && minor >= 5) || major > 1;
	TRAP_OpenGL_VERSION_2_0 = (major == 2 && minor >= 0) || major > 2;
	TRAP_OpenGL_VERSION_2_1 = (major == 2 && minor >= 1) || major > 2;
	TRAP_OpenGL_VERSION_3_0 = (major == 3 && minor >= 0) || major > 3;
	TRAP_OpenGL_VERSION_3_1 = (major == 3 && minor >= 1) || major > 3;
	TRAP_OpenGL_VERSION_3_2 = (major == 3 && minor >= 2) || major > 3;
	TRAP_OpenGL_VERSION_3_3 = (major == 3 && minor >= 3) || major > 3;
	TRAP_OpenGL_VERSION_4_0 = (major == 4 && minor >= 0) || major > 4;
	TRAP_OpenGL_VERSION_4_1 = (major == 4 && minor >= 1) || major > 4;
	TRAP_OpenGL_VERSION_4_2 = (major == 4 && minor >= 2) || major > 4;
	TRAP_OpenGL_VERSION_4_3 = (major == 4 && minor >= 3) || major > 4;
	TRAP_OpenGL_VERSION_4_4 = (major == 4 && minor >= 4) || major > 4;
	TRAP_OpenGL_VERSION_4_5 = (major == 4 && minor >= 5) || major > 4;
	TRAP_OpenGL_VERSION_4_6 = (major == 4 && minor >= 6) || major > 4;

	if(OpenGLVersion.Major > 4 || (OpenGLVersion.Major >= 4 && OpenGLVersion.Minor >= 6))
	{
		MaxLoadedMajor = 4;
		MaxLoadedMinor = 6;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAPLoadOpenGLLoader(const TRAPLoadProc load)
{
	TP_PROFILE_FUNCTION();

	OpenGLVersion.Major = 0;
	OpenGLVersion.Minor = 0;
	glGetString = reinterpret_cast<PFNGLGETSTRINGPROC>(load("glGetString"));
	if (glGetString == nullptr)
		return false;
	if (glGetString(GL_VERSION) == nullptr)
		return false;

	FindCoreOpenGL();

	LoadOpenGLVersion1_0(load);
	LoadOpenGLVersion1_1(load);
	LoadOpenGLVersion1_2(load);
	LoadOpenGLVersion1_3(load);
	LoadOpenGLVersion1_4(load);
	LoadOpenGLVersion1_5(load);
	LoadOpenGLVersion2_0(load);
	LoadOpenGLVersion2_1(load);
	LoadOpenGLVersion3_0(load);
	LoadOpenGLVersion3_1(load);
	LoadOpenGLVersion3_2(load);
	LoadOpenGLVersion3_3(load);
	LoadOpenGLVersion4_0(load);
	LoadOpenGLVersion4_1(load);
	LoadOpenGLVersion4_2(load);
	LoadOpenGLVersion4_3(load);
	LoadOpenGLVersion4_4(load);
	LoadOpenGLVersion4_5(load);
	LoadOpenGLVersion4_6(load);

	if (!FindExtensionsOpenGL())
		return false;

	return OpenGLVersion.Major != 0 || OpenGLVersion.Minor != 0;
}