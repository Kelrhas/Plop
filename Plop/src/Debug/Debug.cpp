#include "Plop_pch.h"
#include "Debug.h"

#include <map>

#include <GL/glew.h>
#include <Debug/Log.h>

#include <Debug/MemoryTrack.hpp>



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
#import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("8.0") lcid("0") raw_interfaces_only named_guids
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
			int iGLError = glGetError();
			while (iGLError != GL_NO_ERROR)
			{
				switch (iGLError)
				{
					case GL_INVALID_ENUM:		// 1280
					case GL_INVALID_VALUE:		// 1281
					case GL_INVALID_OPERATION:	// 1282
					default:
						BREAK();
				}
				iGLError = glGetError();
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
				ASSERT( s_GlobalAllocation.uCurrentNumberAllocation == s_GlobalAllocation.mapAllocations.size(), "size mismatch" );
				if (ImGui::Button( "Clear current tracking" ))
				{
					s_GlobalAllocation.mapAllocations.clear();
					s_GlobalAllocation.mapAllocationsCalls.clear();
					s_GlobalAllocation.mapAllocationsSizes.clear();
					s_GlobalAllocation.uCurrentAllocation = 0;
					s_GlobalAllocation.uCurrentNumberAllocation = 0;
				}


				ImGui::BeginChild( "AllocationList", ImVec2( 0, -ImGui::GetFrameHeightWithSpacing() * 6) );

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
					std::ostringstream ssBytes;
					ssBytes << uSize << " bytes";
					ImVec2 vTextSize = ImGui::CalcTextSize( ssBytes.str().c_str() );
					ImGui::SameLine( ImGui::GetWindowContentRegionMax().x - vTextSize.x );
					ImGui::Text( ssBytes.str().c_str() );
				}
				ImGui::EndChild();


				ImGui::LabelText( "Frame Allocation (#)", "%llu", s_GlobalAllocation.uFrameNumberAllocation );
				ImGui::LabelText( "Frame Allocation (bytes)", "%llu", s_GlobalAllocation.uFrameAllocation );
				ImGui::LabelText( "Current Allocation (#)", "%llu", s_GlobalAllocation.uCurrentNumberAllocation );
				ImGui::LabelText( "Current Allocation (bytes)", "%llu", s_GlobalAllocation.uCurrentAllocation );
				ImGui::LabelText( "Total Allocation (#)", "%llu", s_GlobalAllocation.uTotalNumberAllocation );
				ImGui::LabelText( "Total Allocation (bytes)", "%llu", s_GlobalAllocation.uTotalAllocation );
#else

				ImGui::Text( "Memory tracking disabled" );
#endif // ENABLE_MEMORY_TRACKING
			}
			ImGui::End();
		}
	}
}
