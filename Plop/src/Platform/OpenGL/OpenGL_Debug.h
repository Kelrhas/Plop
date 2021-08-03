#pragma once



namespace Plop
{

	class GLDebugGroupScoped
	{
	public:
		GLDebugGroupScoped( const char* str );
		~GLDebugGroupScoped();
	};

}

#define GL_DEBUG_GROUP_SCOPED(str) Plop::GLDebugGroupScoped oDebugGroup(str)
#define GL_DEBUG_MARKER(str) glDebugMessageInsert( GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, -1, str );