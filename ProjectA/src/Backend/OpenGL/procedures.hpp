#pragma once

namespace ProjectA {
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
	#endif

	#ifdef OPENGL_USE_SPIRV
		extern PFNGLSHADERBINARYPROC glShaderBinary;
		extern PFNGLSPECIALIZESHADERPROC glSpecializeShader;
	#else
		extern PFNGLCOMPILESHADERPROC glCompileShader;
		extern PFNGLSHADERSOURCEPROC glShaderSource;
	#endif

	#if OPENGL_VERSION >= OPENGL_VERSION_4_5
		// TODO: There is no non-DSA variant for this function. Write a custom implementation.
		extern PFNGLGETTEXTURESUBIMAGEPROC glGetTextureSubImage;
	#endif

	#if OPENGL_VERSION >= OPENGL_VERSION_4_3
		#ifdef DEBUG_ENABLE_MESSENGER
			extern PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
			extern PFNGLDEBUGMESSAGEINSERTPROC glDebugMessageInsert;
		#endif

		extern PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
		extern PFNGLDISPATCHCOMPUTEINDIRECTPROC glDispatchComputeIndirect;

		// TODO: There is no non-DSA variant for this function. Write a custom implementation.
		extern PFNGLTEXTUREVIEWPROC glTextureView;

		// TODO(review): Use DSA versions if possible.
		extern PFNGLVERTEXATTRIBBINDINGPROC glVertexAttribBinding;
		extern PFNGLVERTEXATTRIBFORMATPROC glVertexAttribFormat;
		extern PFNGLVERTEXATTRIBIFORMATPROC glVertexAttribIFormat;
		extern PFNGLVERTEXATTRIBLFORMATPROC glVertexAttribLFormat;
		extern PFNGLVERTEXBINDINGDIVISORPROC glVertexBindingDivisor;
	#endif

	#if OPENGL_VERSION >= OPENGL_VERSION_4_2
		extern PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
		extern PFNGLMEMORYBARRIERPROC glMemoryBarrier;
	#endif

	#if OPENGL_VERSION >= OPENGL_VERSION_4_0
		extern PFNGLBLENDFUNCIPROC glBlendFunci;
		extern PFNGLMINSAMPLESHADINGPROC glMinSampleShading;
	#endif

	#if OPENGL_VERSION >= OPENGL_VERSION_3_3
		extern PFNGLBINDSAMPLERPROC glBindSampler;
		extern PFNGLDELETESAMPLERSPROC glDeleteSamplers;
	#endif

	#if OPENGL_VERSION >= OPENGL_VERSION_3_2
		extern PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
		extern PFNGLDELETESYNCPROC glDeleteSync;
		extern PFNGLFENCESYNCPROC glFenceSync;
		extern PFNGLPROVOKINGVERTEXPROC glProvokingVertex;
		extern PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;
	#endif

	#if OPENGL_VERSION >= OPENGL_VERSION_3_0
		extern PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
		extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
		extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
		extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
		extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
		extern PFNGLDISABLEIPROC glDisablei;
		extern PFNGLENABLEIPROC glEnablei;
		extern PFNGLGETINTEGERI_VPROC glGetIntegeri_v;
	#endif

	#if OPENGL_VERSION >= OPENGL_VERSION_2_0
		extern PFNGLATTACHSHADERPROC glAttachShader;
		extern PFNGLBINDBUFFERPROC glBindBuffer;
		extern PFNGLBINDTEXTUREPROC glBindTexture;
		extern PFNGLCREATEPROGRAMPROC glCreateProgram;
		extern PFNGLCREATESHADERPROC glCreateShader;
		extern PFNGLCULLFACEPROC glCullFace;
		extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
		extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
		extern PFNGLDELETESHADERPROC glDeleteShader;
		extern PFNGLDELETETEXTURESPROC glDeleteTextures;
		extern PFNGLDEPTHFUNCPROC glDepthFunc;
		extern PFNGLDEPTHMASKPROC glDepthMask;
		extern PFNGLDETACHSHADERPROC glDetachShader;
		extern PFNGLDISABLEPROC glDisable;
		extern PFNGLENABLEPROC glEnable;

		// TODO(review): Use glEnableVertexArrayAttrib if newer GL version.
		extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;

		extern PFNGLFRONTFACEPROC glFrontFace;
		extern PFNGLGENTEXTURESPROC glGenTextures;
		extern PFNGLGETFLOATVPROC glGetFloatv;
		extern PFNGLGETINTEGERVPROC glGetIntegerv;
		extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
		extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
		extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
		extern PFNGLGETSHADERIVPROC glGetShaderiv;

		#if BUILD_MODE_DEBUG
			extern PFNGLGETSTRINGPROC glGetString;
		#endif

		extern PFNGLGETTEXPARAMETERIVPROC glGetTexParameteriv;
		extern PFNGLLINEWIDTHPROC glLineWidth;
		extern PFNGLLINKPROGRAMPROC glLinkProgram;
		extern PFNGLPIXELSTOREIPROC glPixelStorei;
		extern PFNGLPOLYGONMODEPROC glPolygonMode;
		extern PFNGLSCISSORPROC glScissor;
		extern PFNGLUSEPROGRAMPROC glUseProgram;
		extern PFNGLVIEWPORTPROC glViewport;
	#endif

	#ifdef BUILD_MODE_DEBUG
		#if OPENGL_VERSION >= OPENGL_VERSION_4_5
			extern PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glCheckNamedFramebufferStatus;
		#elif OPENGL_VERSION >= OPENGL_VERSION_3_0
			extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
		#endif
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