#include "Plop_pch.h"
#include "Console.h"

#include <misc/cpp/imgui_stdlib.h>
#include "Application.h"
#include "Input/Input.h"
#include "Utils/StringUtils.h"

namespace Plop
{
	// ImGui colors are 0xAABBGGRR
	const ImU32 s_BufferTypeColors[] = {
		 0xFFFFFFFF, 0xFFA000A0, 0xFF00A5FF, 0xFF0000FF, 0xFF008400, // text color
		 0xFFD8D8D8, 0xFF700070, 0xFF0092CC, 0xFF0000CC, 0xFF006200, // hovered color
		 0xFFA0A0A0, 0xFF450045, 0xFF005580, 0xFF000080, 0xFF004000, // fade color
	};
	static_assert(sizeof( s_BufferTypeColors ) / sizeof( s_BufferTypeColors[0] ) == (int)LogEntry::Type::Count * 3);
	const char* s_BufferTypeNames[] = {
		"Info",
		"Assert",
		"Warning",
		"Error",
		"Command"
	};
	static_assert(sizeof( s_BufferTypeNames ) / sizeof( s_BufferTypeNames[0] ) == (int)LogEntry::Type::Count);
	int	 s_BufferCountPerType[(int)LogEntry::Type::Count]{ 0 };
	bool s_BufferTypeDisplayed[(int)LogEntry::Type::Count]{ true };

	bool				Console::s_bDrawConsole = false;
	String				Console::s_sInputBuffer;
	Console::CommandMap	Console::s_mapCommands;
	LogEntry			Console::s_ConsoleBuffer[MAX_ENTRY];
	uint32_t			Console::s_uEntryIndex = 0;
	ImGuiTextFilter		Console::s_Filter;
	bool				Console::s_bNeedAutoScroll = true;


	void Console::Init()
	{
		memset( s_BufferCountPerType, 0, sizeof( s_BufferCountPerType ) );
		memset( s_BufferTypeDisplayed, true, sizeof( s_BufferTypeDisplayed ) );

		RegisterCommand( "Test", []( const String& _args ) { Console::AddOutput( "Command Test with args" ); } );
		RegisterCommand( "Close", []( const String& _args ) { Application::Get()->Close(); } );
		RegisterCommand( "Exit", []( const String& _args ) { Application::Get()->Close(); } );
	}

	void Console::AddOutput( const String& _str, LogEntry::Type _eType /*= LogEntry::Type::Info*/ )
	{
		char pBuffer[4096];

		// spacify tabulation because ImGui does not handle the 4-space column layout
		const char* pStart = _str.c_str();
		int iCharCount = 0;
		while (pStart && *pStart)
		{
			if (*pStart == '\t')
			{
				while (iCharCount % 4 != 0)
				{
					pBuffer[iCharCount++] = ' ';
				}
			}
			else
				pBuffer[iCharCount++] = *pStart;

			++pStart;
		}
		pBuffer[iCharCount] = 0;

		s_BufferCountPerType[(int)_eType]++;
		s_ConsoleBuffer[s_uEntryIndex] = { _eType, pBuffer };
		s_uEntryIndex = (++s_uEntryIndex) % MAX_ENTRY;
	}

	void Console::RegisterCommand( const char* _pCommand, std::function<void( const String& )> _callback )
	{
		String sCommand = _pCommand;
		std::transform( sCommand.begin(), sCommand.end(), sCommand.begin(), ::toupper );

		ASSERT( s_mapCommands.find( sCommand ) == s_mapCommands.end(), "The command is already registered, overwriting the callback..." );
		s_mapCommands[sCommand] = _callback;
	}

	bool Console::ExecCommand( const String& _sText )
	{
		size_t argsPos = _sText.find_first_of( ' ' );
		String sArgs = argsPos != String::npos ? _sText.c_str() + argsPos + 1 : "";

		String sCommand = argsPos != String::npos ? _sText.substr(0, argsPos ) : _sText;
		std::transform( sCommand.begin(), sCommand.end(), sCommand.begin(), ::toupper );

		s_bNeedAutoScroll = true;

		auto& it = s_mapCommands.find( sCommand );
		if(it != s_mapCommands.end())
		{
			it->second( sArgs );

			return true;
		}

		return false;
	}

	void Console::Draw()
	{
		if (Input::IsKeyPressed( KeyCode::KEY_OEM7 ))
		{
			s_bDrawConsole = !s_bDrawConsole;
			if (s_bDrawConsole)
			{
				ImGui::SetNextWindowFocus();
				ImGui::SetNextWindowViewport( ImGui::GetMainViewport()->ID );
			}
		}

		if (s_bDrawConsole)
		{
			const ImVec2 consoleDefaultSize( Application::Get()->GetWindow().GetWidth() * 0.8f, Application::Get()->GetWindow().GetHeight() * 0.3f );
			ImGui::SetNextWindowSize( consoleDefaultSize, ImGuiCond_Appearing );

			const ImVec2 consoleDefaultPos = (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0 ? ImVec2( ImGui::GetMainViewport()->Pos.x + Application::Get()->GetWindow().GetWidth() * 0.5f, ImGui::GetMainViewport()->Pos.y + Application::Get()->GetWindow().GetHeight() * 1.f )
											: ImVec2( Application::Get()->GetWindow().GetWidth() * 0.5f, Application::Get()->GetWindow().GetHeight() * 1.f );
			ImGui::SetNextWindowPos( consoleDefaultPos, ImGuiCond_Appearing, ImVec2( 0.5f, 1.f ) );


			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration;// | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
			if (ImGui::Begin( "Console", nullptr, windowFlags ))
			{
				s_Filter.Draw( "Filter", 180 );
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Checkbox( "Autoscroll", &s_bNeedAutoScroll );


				for (int i = 0; i < (int)LogEntry::Type::Count; ++i)
				{
					if (s_BufferCountPerType[i] == 0)
						continue;


					ImGui::SameLine();
					int nb = s_BufferCountPerType[i];
					ImGui::PushID( i );

					if (s_BufferTypeDisplayed[i])
					{
						ImGui::PushStyleColor( ImGuiCol_Border, s_BufferTypeColors[i] );
						ImGui::PushStyleColor( ImGuiCol_Button, s_BufferTypeColors[i + (int)LogEntry::Type::Count * 2] );
					}
					else
					{
						ImGui::PushStyleColor( ImGuiCol_Border, s_BufferTypeColors[i + (int)LogEntry::Type::Count * 2] );
						ImGui::PushStyleColor( ImGuiCol_Button, 0x00000000 ); // transparent
					}
					ImGui::PushStyleColor( ImGuiCol_Text, s_BufferTypeColors[i] );
					ImGui::PushStyleColor( ImGuiCol_ButtonHovered, s_BufferTypeColors[i + (int)LogEntry::Type::Count * 1] );
					ImGui::PushStyleColor( ImGuiCol_ButtonActive, s_BufferTypeColors[i] );

					ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1.0f );

					static char pBuffer[128]{ 0 };
					sprintf_s( pBuffer, "%s: %d", s_BufferTypeNames[i], s_BufferCountPerType[i] );

					if (ImGui::Button( pBuffer ))
						s_BufferTypeDisplayed[i] = !s_BufferTypeDisplayed[i];
					ImGui::PopStyleVar();
					ImGui::PopStyleColor( 5 );
					ImGui::PopID();
				}


				const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
				ImGui::BeginChild( "ScrollingRegion", ImVec2( 0, -footer_height_to_reserve ), false, ImGuiWindowFlags_HorizontalScrollbar );
				ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 4, 1 ) );

				for (int i = 0; i < (int)s_uEntryIndex; i++)
				{
					const LogEntry& entry = s_ConsoleBuffer[i];
					const char* pText = entry.sLog.c_str();

					// filter type
					if (!s_BufferTypeDisplayed[(int)entry.eType])
						continue;

					// filter text
					if (!s_Filter.PassFilter( pText ))
						continue;

					ImGui::PushStyleColor( ImGuiCol_Text, s_BufferTypeColors[(int)entry.eType] );
					//ImGui::Text( pText );
					ImGui::TextUnformatted( pText );
					ImGui::PopStyleColor();
				}

				ImGuiIO& io = ImGui::GetIO();

				if (s_bNeedAutoScroll)
					ImGui::SetScrollHereY( 1.0f );

				ImGui::PopStyleVar();
				ImGui::EndChild();

				// deactive auto scroll
				if (io.MouseWheel && ImGui::IsItemHovered())
					s_bNeedAutoScroll = false;


				ImGui::Separator();

				bool bReclaimFocus = false;
				ImGuiInputTextFlags textFlags = /*ImGuiInputTextFlags_CharsNoBlank |*/ ImGuiInputTextFlags_EnterReturnsTrue |
					ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory |
					ImGuiInputTextFlags_NoHorizontalScroll;
				ImGui::SetNextItemWidth( -1.f );
				if (ImGui::InputText( "##Input", &s_sInputBuffer, textFlags, &Console::InputCallback, nullptr ))
				{
					Utils::TrimSpaces( s_sInputBuffer );
					if (s_sInputBuffer.empty() == false)
					{
						if (ExecCommand( s_sInputBuffer ) == false)
						{
							String str = fmt::format( "Unknown command: {}", s_sInputBuffer );
							AddOutput( str, LogEntry::Type::Command );
						}
					}
					s_sInputBuffer.clear();
					bReclaimFocus = true;
				}

				ImGui::SetItemDefaultFocus();
				if (bReclaimFocus || ImGui::IsWindowAppearing())
					ImGui::SetKeyboardFocusHere( -1 );
			}
			ImGui::End();
		}
	}

	int Console::InputCallback( ImGuiInputTextCallbackData* _pData )
	{
		switch (_pData->EventFlag)
		{
			case ImGuiInputTextFlags_CallbackCompletion:
			{


				break;
			}
			case ImGuiInputTextFlags_CallbackHistory:
			{

				break;
			}
		}

		return 0;
	}

}
