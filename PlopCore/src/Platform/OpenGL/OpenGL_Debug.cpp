#include "OpenGL_Debug.h"


#include <GL/glew.h>
//#include <GL/GL.h>

namespace Plop
{
	GLDebugGroupScoped::GLDebugGroupScoped( const char* str )
	{
		glPushDebugGroup( GL_DEBUG_SOURCE_APPLICATION, 0, -1, str );
	}
	GLDebugGroupScoped::~GLDebugGroupScoped()
	{
		glPopDebugGroup();
	}
}