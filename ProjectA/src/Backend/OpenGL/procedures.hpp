#pragma once

#include "Engine/config.hpp"

namespace ProjectA {
	extern PFNGLATTACHSHADERPROC glAttachShader;
	extern PFNGLBINDBUFFERPROC glBindBuffer;
	extern PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
	extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
	extern PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
	extern PFNGLBINDSAMPLERPROC glBindSampler;
	extern PFNGLBINDTEXTUREUNITPROC glBindTextureUnit;
	extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
	extern PFNGLBINDVERTEXBUFFERPROC glBindVertexBuffer;
	extern PFNGLBLENDFUNCIPROC glBlendFunci;
	extern PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;

	#ifdef BUILD_MODE_DEBUG
		extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
	#endif

	extern PFNGLCLEARBUFFERFIPROC glClearBufferfi;
	extern PFNGLCLEARBUFFERFVPROC glClearBufferfv;
	extern PFNGLCLIENTWAITSYNCPROC glClientWaitSync;

	#ifndef OPENGL_USE_SPIRV
		extern PFNGLCOMPILESHADERPROC glCompileShader;
	#endif

	#ifdef OPENGL_USE_DSA
		extern PFNGLCOPYNAMEDBUFFERSUBDATAPROC glCopyNamedBufferSubData;
	#else
		extern PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData;
	#endif

	extern PFNGLCREATEBUFFERSPROC glCreateBuffers;
	extern PFNGLCREATEFRAMEBUFFERSPROC glCreateFramebuffers;
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
	extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
	extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
	extern PFNGLDELETESAMPLERSPROC glDeleteSamplers;
	extern PFNGLDELETESHADERPROC glDeleteShader;
	extern PFNGLDELETESYNCPROC glDeleteSync;
	extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
	extern PFNGLDETACHSHADERPROC glDetachShader;
	extern PFNGLENABLEIPROC glDisablei;
	extern PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
	extern PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
	extern PFNGLDRAWBUFFERSPROC glDrawBuffers;
	extern PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
	extern PFNGLENABLEIPROC glEnablei;
	extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
	extern PFNGLFENCESYNCPROC glFenceSync;
	extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
	extern PFNGLGETINTEGERI_VPROC glGetIntegeri_v;
	extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
	extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
	extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
	extern PFNGLGETSHADERIVPROC glGetShaderiv;
	extern PFNGLLINKPROGRAMPROC glLinkProgram;
	extern PFNGLMAPNAMEDBUFFERRANGEPROC glMapNamedBufferRange;
	extern PFNGLMEMORYBARRIERPROC glMemoryBarrier;
	extern PFNGLMINSAMPLESHADINGPROC glMinSampleShading;
	extern PFNGLNAMEDBUFFERSTORAGEPROC glNamedBufferStorage;
	extern PFNGLPROVOKINGVERTEXPROC glProvokingVertex;
	extern PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;

	#ifdef OPENGL_USE_SPIRV
		extern PFNGLSHADERBINARYPROC glShaderBinary;
	#endif

	#ifndef OPENGL_USE_SPIRV
		extern PFNGLSHADERSOURCEPROC glShaderSource;
	#endif

	#ifdef OPENGL_USE_SPIRV
		extern PFNGLSPECIALIZESHADERPROC glSpecializeShader;
	#endif

	extern PFNGLTEXTURESTORAGE2DPROC glTextureStorage2D;
	extern PFNGLTEXTURESTORAGE3DPROC glTextureStorage3D;
	extern PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC glTextureStorage2DMultisample;
	extern PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC glTextureStorage3DMultisample;
	extern PFNGLTEXTURESUBIMAGE1DPROC glTextureSubImage1D;
	extern PFNGLTEXTURESUBIMAGE2DPROC glTextureSubImage2D;
	extern PFNGLTEXTURESUBIMAGE3DPROC glTextureSubImage3D;
	extern PFNGLTEXTUREVIEWPROC glTextureView;
	extern PFNGLUNMAPNAMEDBUFFERPROC glUnmapNamedBuffer;
	extern PFNGLUSEPROGRAMPROC glUseProgram;
	extern PFNGLVERTEXATTRIBBINDINGPROC glVertexAttribBinding;
	extern PFNGLVERTEXATTRIBFORMATPROC glVertexAttribFormat;
	extern PFNGLVERTEXATTRIBIFORMATPROC glVertexAttribIFormat;
	extern PFNGLVERTEXATTRIBLFORMATPROC glVertexAttribLFormat;
	extern PFNGLVERTEXBINDINGDIVISORPROC glVertexBindingDivisor;

	extern PFNGLCLEARTEXSUBIMAGEPROC glClearTexSubImage;
}