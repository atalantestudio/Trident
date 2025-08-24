#pragma once

namespace ProjectA {
	extern PFNGLATTACHSHADERPROC glAttachShader;
	extern PFNGLBINDBUFFERPROC glBindBuffer;
	extern PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
	extern PFNGLBINDSAMPLERPROC glBindSampler;
	extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
	extern PFNGLBLENDFUNCIPROC glBlendFunci;
	extern PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
	extern PFNGLCREATEPROGRAMPROC glCreateProgram;
	extern PFNGLCREATESHADERPROC glCreateShader;
	extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
	extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
	extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
	extern PFNGLDELETESAMPLERSPROC glDeleteSamplers;
	extern PFNGLDELETESHADERPROC glDeleteShader;
	extern PFNGLDELETESYNCPROC glDeleteSync;
	extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
	extern PFNGLDETACHSHADERPROC glDetachShader;
	extern PFNGLENABLEIPROC glDisablei;
	extern PFNGLENABLEIPROC glEnablei;
	extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
	extern PFNGLFENCESYNCPROC glFenceSync;
	extern PFNGLGETINTEGERI_VPROC glGetIntegeri_v;
	extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
	extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
	extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
	extern PFNGLGETSHADERIVPROC glGetShaderiv;

	// TODO(review): There is no non-DSA variant for this function.
	//       Write a custom implementation.
	extern PFNGLGETTEXTURESUBIMAGEPROC glGetTextureSubImage;

	extern PFNGLLINKPROGRAMPROC glLinkProgram;
	extern PFNGLMEMORYBARRIERPROC glMemoryBarrier;
	extern PFNGLMINSAMPLESHADINGPROC glMinSampleShading;
	extern PFNGLPROVOKINGVERTEXPROC glProvokingVertex;
	extern PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;

	// TODO(review): There is no non-DSA variant for this function.
	//       Write a custom implementation.
	extern PFNGLTEXTUREVIEWPROC glTextureView;

	extern PFNGLUSEPROGRAMPROC glUseProgram;
	extern PFNGLVERTEXATTRIBBINDINGPROC glVertexAttribBinding;
	extern PFNGLVERTEXATTRIBFORMATPROC glVertexAttribFormat;
	extern PFNGLVERTEXATTRIBIFORMATPROC glVertexAttribIFormat;
	extern PFNGLVERTEXATTRIBLFORMATPROC glVertexAttribLFormat;
	extern PFNGLVERTEXBINDINGDIVISORPROC glVertexBindingDivisor;

	#ifdef DEBUG_ENABLE_MESSENGER
		extern PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
		extern PFNGLDEBUGMESSAGEINSERTPROC glDebugMessageInsert;
	#endif

	#ifdef OPENGL_USE_DSA
		extern PFNGLBINDTEXTUREUNITPROC glBindTextureUnit;
		extern PFNGLBLITNAMEDFRAMEBUFFERPROC glBlitNamedFramebuffer;
		extern PFNGLCLEARNAMEDFRAMEBUFFERFIPROC glClearNamedFramebufferfi;
		extern PFNGLCLEARNAMEDFRAMEBUFFERFVPROC glClearNamedFramebufferfv;
		extern PFNGLCOPYNAMEDBUFFERSUBDATAPROC glCopyNamedBufferSubData;
		extern PFNGLCREATEBUFFERSPROC glCreateBuffers;
		extern PFNGLCREATEFRAMEBUFFERSPROC glCreateFramebuffers;
		extern PFNGLCREATESAMPLERSPROC glCreateSamplers;
		extern PFNGLCREATETEXTURESPROC glCreateTextures;
		extern PFNGLCREATEVERTEXARRAYSPROC glCreateVertexArrays;
		extern PFNGLMAPNAMEDBUFFERRANGEPROC glMapNamedBufferRange;
		extern PFNGLNAMEDBUFFERSTORAGEPROC glNamedBufferStorage;
		extern PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC glNamedFramebufferDrawBuffers;
		extern PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC glNamedFramebufferReadBuffer;
		extern PFNGLNAMEDFRAMEBUFFERTEXTUREPROC glNamedFramebufferTexture;
		extern PFNGLTEXTURESTORAGE2DPROC glTextureStorage2D;
		extern PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC glTextureStorage2DMultisample;
		extern PFNGLTEXTURESTORAGE3DPROC glTextureStorage3D;
		extern PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC glTextureStorage3DMultisample;
		extern PFNGLTEXTURESUBIMAGE1DPROC glTextureSubImage1D;
		extern PFNGLTEXTURESUBIMAGE2DPROC glTextureSubImage2D;
		extern PFNGLTEXTURESUBIMAGE3DPROC glTextureSubImage3D;
		extern PFNGLUNMAPNAMEDBUFFERPROC glUnmapNamedBuffer;
		extern PFNGLVERTEXARRAYELEMENTBUFFERPROC glVertexArrayElementBuffer;
		extern PFNGLVERTEXARRAYVERTEXBUFFERPROC glVertexArrayVertexBuffer;

		#ifdef BUILD_MODE_DEBUG
			extern PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glCheckNamedFramebufferStatus;
		#endif
	#else
		extern PFNGLACTIVETEXTUREPROC glActiveTexture;
		extern PFNGLBINDVERTEXBUFFERPROC glBindVertexBuffer;
		extern PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
		extern PFNGLBUFFERSTORAGEPROC glBufferStorage;
		extern PFNGLCLEARBUFFERFIPROC glClearBufferfi;
		extern PFNGLCLEARBUFFERFVPROC glClearBufferfv;
		extern PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData;
		extern PFNGLDRAWBUFFERSPROC glDrawBuffers;
		extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
		extern PFNGLGENBUFFERSPROC glGenBuffers;
		extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
		extern PFNGLGENSAMPLERSPROC glGenSamplers;
		extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
		extern PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
		extern PFNGLTEXSTORAGE2DPROC glTexStorage2D;
		extern PFNGLTEXSTORAGE2DMULTISAMPLEPROC glTexStorage2DMultisample;
		extern PFNGLTEXSTORAGE3DPROC glTexStorage3D;
		extern PFNGLTEXSTORAGE3DMULTISAMPLEPROC glTexStorage3DMultisample;
		extern PFNGLTEXSUBIMAGE1DPROC glTexSubImage1D;
		extern PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D;
		extern PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
		extern PFNGLUNMAPBUFFERPROC glUnmapBuffer;

		#ifdef BUILD_MODE_DEBUG
			extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
		#endif
	#endif

	#ifdef OPENGL_USE_SPIRV
		extern PFNGLSHADERBINARYPROC glShaderBinary;
		extern PFNGLSPECIALIZESHADERPROC glSpecializeShader;
	#else
		extern PFNGLCOMPILESHADERPROC glCompileShader;
		extern PFNGLSHADERSOURCEPROC glShaderSource;
	#endif

	#if OPENGL_VERSION >= OPENGL_VERSION_4_3
		extern PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
		extern PFNGLDISPATCHCOMPUTEINDIRECTPROC glDispatchComputeIndirect;
	#endif

	#if OPENGL_VERSION >= OPENGL_VERSION_3_0
		extern PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
		extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
	#endif

	#if OPENGL_VERSION >= OPENGL_VERSION_4_2
		extern PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC glDrawArraysInstancedBaseInstance;
		extern PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance;
	#elif OPENGL_VERSION >= OPENGL_VERSION_3_2
		extern PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
	#elif OPENGL_VERSION >= OPENGL_VERSION_3_1
		extern PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
		extern PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
	#elif OPENGL_VERSION >= OPENGL_VERSION_2_0
		extern PFNGLDRAWARRAYSPROC glDrawArrays;
		extern PFNGLDRAWELEMENTSPROC glDrawElements;
	#endif

	#if OPENGL_VERSION >= OPENGL_VERSION_4_3
		extern PFNGLMULTIDRAWARRAYSINDIRECTPROC glMultiDrawArraysIndirect;
		extern PFNGLMULTIDRAWELEMENTSINDIRECTPROC glMultiDrawElementsIndirect;
	#elif OPENGL_VERSION >= OPENGL_VERSION_4_0
		extern PFNGLDRAWARRAYSINDIRECTPROC glDrawArraysIndirect;
		extern PFNGLDRAWELEMENTSINDIRECTPROC glDrawElementsIndirect;
	#endif
}