#pragma once

namespace ProjectA {
	// The following methods are part of the OpenGL 1.1 API
	// and do not need to be linked manually.
	// - glCullFace
	// - glDisable
	// - glEnable
	// - glFrontFace

	// TODO: Remove 1.1 functions from this list.
	extern PFNGLATTACHSHADERPROC glAttachShader;
	extern PFNGLBINDBUFFERPROC glBindBuffer;
	extern PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
	extern PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
	extern PFNGLBINDSAMPLERPROC glBindSampler;
	extern PFNGLBINDTEXTUREUNITPROC glBindTextureUnit;
	extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
	extern PFNGLBINDVERTEXBUFFERPROC glBindVertexBuffer;
	extern PFNGLBLENDFUNCPROC glBlendFunc;
	extern PFNGLCLEARPROC glClear;
	extern PFNGLCLEARCOLORPROC glClearColor;
	extern PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
	extern PFNGLCOPYNAMEDBUFFERSUBDATAPROC glCopyNamedBufferSubData;
	extern PFNGLCREATEBUFFERSPROC glCreateBuffers;
	extern PFNGLCREATEPROGRAMPROC glCreateProgram;
	extern PFNGLCREATESAMPLERSPROC glCreateSamplers;
	extern PFNGLCREATESHADERPROC glCreateShader;
	extern PFNGLCREATETEXTURESPROC glCreateTextures;
	extern PFNGLCREATEVERTEXARRAYSPROC glCreateVertexArrays;

	#ifdef DEBUG_ENABLE_MESSENGER
		extern PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
		extern PFNGLDEBUGMESSAGEINSERTPROC glDebugMessageInsert;
	#endif

	extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
	extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
	extern PFNGLDELETESAMPLERSPROC glDeleteSamplers;
	extern PFNGLDELETESHADERPROC glDeleteShader;
	extern PFNGLDELETESYNCPROC glDeleteSync;
	extern PFNGLDELETETEXTURESPROC glDeleteTextures;
	extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
	extern PFNGLDETACHSHADERPROC glDetachShader;
	extern PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
	extern PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
	extern PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
	extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
	extern PFNGLFENCESYNCPROC glFenceSync;
	extern PFNGLGENTEXTURESPROC glGenTextures;
	extern PFNGLGETINTEGERI_VPROC glGetIntegeri_v;
	extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
	extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
	extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
	extern PFNGLGETSHADERIVPROC glGetShaderiv;
	extern PFNGLLINKPROGRAMPROC glLinkProgram;
	extern PFNGLMAPNAMEDBUFFERRANGEPROC glMapNamedBufferRange;
	extern PFNGLMEMORYBARRIERPROC glMemoryBarrier;
	extern PFNGLNAMEDBUFFERSTORAGEPROC glNamedBufferStorage;
	extern PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;
	extern PFNGLSHADERBINARYPROC glShaderBinary;
	extern PFNGLSPECIALIZESHADERPROC glSpecializeShader;
	extern PFNGLTEXTURESTORAGE2DPROC glTextureStorage2D;
	extern PFNGLTEXTURESUBIMAGE2DPROC glTextureSubImage2D;
	extern PFNGLTEXTUREVIEWPROC glTextureView;
	extern PFNGLUNMAPNAMEDBUFFERPROC glUnmapNamedBuffer;
	extern PFNGLUSEPROGRAMPROC glUseProgram;
	extern PFNGLVERTEXATTRIBBINDINGPROC glVertexAttribBinding;
	extern PFNGLVERTEXATTRIBFORMATPROC glVertexAttribFormat;
	extern PFNGLVERTEXATTRIBIFORMATPROC glVertexAttribIFormat;
	extern PFNGLVERTEXATTRIBLFORMATPROC glVertexAttribLFormat;
	extern PFNGLVERTEXBINDINGDIVISORPROC glVertexBindingDivisor;
	extern PFNGLVIEWPORTPROC glViewport;
}