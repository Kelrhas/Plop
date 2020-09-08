#include "Plop_pch.h"
#include "Debug.h"

#include <map>

#include <GL/glew.h>
#include <Debug/Log.h>

#include <Debug/MemoryTrack.hpp>

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
				if (ImGui::Button( "Clear current allocations" ))
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

					static char buff[256]{ 0 };
					sprintf_s( buff, "%s:%llu", location.pFile, location.uLine );
					if (ImGui::Selectable( buff ))
					{
#ifdef PLATFORM_WINDOWS
						sprintf_s( buff, "/edit %s /command \"edit.goto %llu\"", location.pFile, location.uLine );
						HINSTANCE res = ShellExecuteA( NULL, "runas", "devenv", buff, NULL, SW_SHOW );
						ASSERT( (size_t)res > 32, "Error while executing command: <{0}> returned {1}", buff, (size_t)res );
#endif
					}
					ImGui::SameLine( ImGui::GetWindowContentRegionMax().x - 100 );
					ImGui::Text( "%llu bytes", uSize );
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
