#include "Plop_pch.h"
#include "Debug.h"

#include <GL/glew.h>
#include <Debug/Log.h>

namespace Plop
{
	namespace Debug
	{
		void Assert_GL()
		{
#ifndef _MASTER
			int iGLError = glGetError();
			while (iGLError != GL_NO_ERROR)
			{
				switch (iGLError)
				{
					case GL_INVALID_ENUM:		// 1280
					case GL_INVALID_VALUE:		// 1281
					case GL_INVALID_OPERATION:	// 1282
					default:
						__debugbreak();
				}
				iGLError = glGetError();
			}
#endif
		}

		void TODO( const char* _pMessage )
		{
#ifndef _MASTER
			if (_pMessage)
				Plop::Log::Info( "TODO:\t{}\n", _pMessage );
			static bool bIgnore = false;
			if (!bIgnore)
				__debugbreak();
#endif
		}
	}
}
