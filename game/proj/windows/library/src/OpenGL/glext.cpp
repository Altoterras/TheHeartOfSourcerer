#include <windows.h>
#include <gl/GL.h>
#include <tchar.h>

#include "OpenGL/glext.h"

////////////////////////////////////////////////////////////////////////////
// グローバル変数

PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLISBUFFERPROC glIsBuffer;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLMAPBUFFERPROC glMapBuffer;
PFNGLUNMAPBUFFERPROC glUnmapBuffer;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBLENDEQUATIONPROC glBlendEquation;

////////////////////////////////////////////////////////////////////////////
// グローバル関数

void initGlext()
{
	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	if(glGenBuffers == 0L)	{	::MessageBox(NULL, _T("glGenBuffers is null."), _T("initGlext"), MB_ICONERROR);	}

	glIsBuffer = (PFNGLISBUFFERPROC)wglGetProcAddress("glIsBuffer");
	if(glGenBuffers == 0L)	{	::MessageBox(NULL, _T("glIsBuffer is null."), _T("initGlext"), MB_ICONERROR);	}

	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	if(glGenBuffers == 0L)	{	::MessageBox(NULL, _T("glBindBuffer is null."), _T("initGlext"), MB_ICONERROR);	}

	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	if(glGenBuffers == 0L)	{	::MessageBox(NULL, _T("glBufferData is null."), _T("initGlext"), MB_ICONERROR);	}

	glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
	if(glGenBuffers == 0L)	{	::MessageBox(NULL, _T("glBufferSubData is null."), _T("initGlext"), MB_ICONERROR);	}

	glMapBuffer = (PFNGLMAPBUFFERPROC)wglGetProcAddress("glMapBuffer");
	if(glGenBuffers == 0L)	{	::MessageBox(NULL, _T("glMapBuffer is null."), _T("initGlext"), MB_ICONERROR);	}

	glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress("glUnmapBuffer");
	if(glGenBuffers == 0L)	{	::MessageBox(NULL, _T("glUnmapBuffer is null."), _T("initGlext"), MB_ICONERROR);	}

	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	if(glGenBuffers == 0L)	{	::MessageBox(NULL, _T("glDeleteBuffers is null."), _T("initGlext"), MB_ICONERROR);	}

	glBlendEquation = (PFNGLBLENDEQUATIONPROC)wglGetProcAddress("glBlendEquation");
	if(glGenBuffers == 0L)	{	::MessageBox(NULL, _T("glBlendEquation is null."), _T("initGlext"), MB_ICONERROR);	}
}
