#include "Config.h"

#include "Debug.h"

#ifdef PLATFORM_WINDOWS
	#include <psapi.h> // for PROCESS_MEMORY_COUNTERS
#endif

#include <Al/al.h>
// #include <alc.h>
#include "Debug/Log.h"
#include "Debug/MemoryTrack.hpp"
#include "Utils/StringUtils.h"
#include "Utils/imgui_custom.h"

#include <GL/glew.h>
#include <map>


/*
#ifndef _MASTER
// https://stackoverflow.com/q/350323
#include <atlbase.h>
//-----------------------------------------------------------------------
// This code is blatently stolen from http://benbuck.com/archives/13
//
// This is from the blog of somebody called "BenBuck" for which there
// seems to be no information.
//-----------------------------------------------------------------------

// import EnvDTE
#pragma warning(disable : 4278)
#pragma warning(disable : 4146)
#pragma warning(disable : 4471)
#import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("8.0") lcid("0") raw_interfaces_only named_guids
#pragma warning(default : 4471)
#pragma warning(default : 4146)
#pragma warning(default : 4278)

bool visual_studio_open_file( char const* filename, unsigned int line )
{
	HRESULT result;
	CLSID clsid;
	result = ::CLSIDFromProgID( L"VisualStudio.DTE", &clsid );
	if (FAILED( result ))
		return false;

	CComPtr<IUnknown> punk;
	result = ::GetActiveObject( clsid, NULL, &punk );
	if (FAILED( result ))
		return false;

	CComPtr<EnvDTE::_DTE> DTE;
	DTE = punk;

	CComPtr<EnvDTE::ItemOperations> item_ops;
	result = DTE->get_ItemOperations( &item_ops );
	if (FAILED( result ))
		return false;

	CComBSTR bstrFileName( filename );
	CComBSTR bstrKind( EnvDTE::vsViewKindTextView );
	CComPtr<EnvDTE::Window> window;
	result = item_ops->OpenFile( bstrFileName, bstrKind, &window );
	if (FAILED( result ))
		return false;

	CComPtr<EnvDTE::Document> doc;
	result = DTE->get_ActiveDocument( &doc );
	if (FAILED( result ))
		return false;

	CComPtr<IDispatch> selection_dispatch;
	result = doc->get_Selection( &selection_dispatch );
	if (FAILED( result ))
		return false;

	CComPtr<EnvDTE::TextSelection> selection;
	result = selection_dispatch->QueryInterface( &selection );
	if (FAILED( result ))
		return false;

	result = selection->GotoLine( line, TRUE );
	if (FAILED( result ))
		return false;

	return true;
}
#else
bool visual_studio_open_file( char const* filename, unsigned int line ) { return false; }
#endif
*/

namespace Plop
{
	namespace Debug
	{
		bool bBreakOnAssert = true;
		
		void NewFrame()
		{
#ifdef ENABLE_MEMORY_TRACKING
			s_GlobalAllocation.ResetFrameStats();
#endif
		}

		void EndFrame()
		{

		}

		void Assert_GL()
		{
#ifndef _MASTER
			int err = glGetError();
			while (err != GL_NO_ERROR)
			{
				switch (err)
				{
					case GL_INVALID_ENUM:		// 0x0500 - 1280
					case GL_INVALID_VALUE:		// 0x0501 - 1281
					case GL_INVALID_OPERATION:	// 0x0502 - 1282
					case GL_STACK_OVERFLOW:		// 0x0503 - 1283
					case GL_STACK_UNDERFLOW:	// 0x0504 - 1284
					case GL_OUT_OF_MEMORY:		// 0x0505 - 1285
					default:
						BREAK();
				}
				err = glGetError();
			}
#endif
		}

		void Assert_AL()
		{
#ifndef _MASTER
			int err = alGetError();
			while (err != AL_NO_ERROR)
			{
				switch (err)
				{
					case AL_INVALID_NAME:		// 0xA001 - 40961
					case AL_INVALID_ENUM:		// 0xA002 - 40962 also AL_ILLEGAL_ENUM
					case AL_INVALID_VALUE:		// 0xA003 - 40963
					case AL_INVALID_OPERATION:	// 0xA004 - 40964 also AL_ILLEGAL_COMMAND
					case AL_OUT_OF_MEMORY:		// 0xA005 - 40965
					default:
						BREAK();
				}
				err = alGetError();
			}
#endif
		}

		void TODO( const char* _pMessage )
		{
#ifndef _MASTER
			if (_pMessage)
				Plop::Log::Info( "TODO:\t{}", _pMessage );
			static bool bIgnore = false;
			if (!bIgnore)
				BREAK();
#endif
		}


		void ShowAllocationsWindow( bool* _bOpened /*= nullptr*/ )
		{
			if (ImGui::Begin( "Allocations", _bOpened ))
			{
#ifdef ENABLE_MEMORY_TRACKING
				ASSERTM( s_GlobalAllocation.uCurrentNumberAllocation == s_GlobalAllocation.mapAllocations.size(), "size mismatch" );
				if (ImGui::Button( "Clear current tracking" ))
				{
					s_GlobalAllocation.mapAllocations.clear();
					s_GlobalAllocation.mapAllocationsCalls.clear();
					s_GlobalAllocation.mapAllocationsSizes.clear();
					s_GlobalAllocation.uCurrentAllocation = 0;
					s_GlobalAllocation.uCurrentNumberAllocation = 0;
				}


				ImGui::BeginChild( "AllocationList", ImVec2( 0, -ImGui::GetFrameHeightWithSpacing() * 9) );

				std::map<size_t, AllocationLocation, std::greater<size_t>> mapAllocSorted;
				for (auto& kv : s_GlobalAllocation.mapAllocationsSizes)
				{
					mapAllocSorted.emplace( kv.second, kv.first );
				}

				for (auto& kv : mapAllocSorted)
				{
					size_t uSize = kv.first;
					AllocationLocation location = kv.second;

					std::ostringstream ssFileLine;
					ssFileLine << location.pFile << ":" << location.uLine;
					if (ImGui::Selectable( ssFileLine.str().c_str() ))
					{
#ifdef PLATFORM_WINDOWS
						visual_studio_open_file( location.pFile, (unsigned int)location.uLine );
						//sprintf_s( buff, "/edit %s /command \"edit.goto %llu\"", location.pFile, location.uLine );
						//HINSTANCE res = ShellExecuteA( NULL, "runas", "devenv", buff, NULL, SW_SHOW );
						//ASSERT( (size_t)res > 32, "Error while executing command: <{0}> returned {1}", buff, (size_t)res );
#endif
					}
					std::string sSize = Utils::SizeToString( uSize );
					ImVec2 vTextSize = ImGui::CalcTextSize( sSize.c_str() );
					ImGui::SameLine( ImGui::GetWindowContentRegionMax().x - vTextSize.x );
					ImGui::Text( sSize.c_str() );
				}
				ImGui::EndChild();


				ImGui::LabelText( "Frame Allocation (#)", "%llu", s_GlobalAllocation.uFrameNumberAllocation );
				ImGui::LabelText( "Frame Allocation", "%s", Utils::SizeToString( s_GlobalAllocation.uFrameAllocation ).c_str() );
				ImGui::LabelText( "Current Allocation (#)", "%llu", s_GlobalAllocation.uCurrentNumberAllocation );
				ImGui::LabelText( "Current Allocation", "%s", Utils::SizeToString( s_GlobalAllocation.uCurrentAllocation ).c_str() );
				ImGui::LabelText( "Total Allocation (#)", "%llu", s_GlobalAllocation.uTotalNumberAllocation );
				ImGui::LabelText( "Total Allocation", "%s", Utils::SizeToString( s_GlobalAllocation.uTotalAllocation ).c_str() );



#else

				ImGui::Text( "Memory tracking disabled" );
#endif // ENABLE_MEMORY_TRACKING

#ifdef PLATFORM_WINDOWS
				PROCESS_MEMORY_COUNTERS ProcessMemoryCounters;
				memset( &ProcessMemoryCounters, 0, sizeof( ProcessMemoryCounters ) );
				ProcessMemoryCounters.cb = sizeof( ProcessMemoryCounters );
				if (::GetProcessMemoryInfo( GetCurrentProcess(), &ProcessMemoryCounters, sizeof( ProcessMemoryCounters ) ))
				{
					ImGui::Separator();
					ImGui::TextLabel( "OS working set", "%s", Utils::SizeToString( ProcessMemoryCounters.WorkingSetSize ).c_str() );
					ImGui::TextLabel( "OS page file usage", "%s", Utils::SizeToString(ProcessMemoryCounters.PagefileUsage ).c_str() );
					ImGui::TextLabel( "OS page fault count", "%s", Utils::SizeToString(ProcessMemoryCounters.PageFaultCount ).c_str() );
				}
#endif
			}
			ImGui::End();
		}
	}
}
