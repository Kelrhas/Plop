#include "Plop_pch.h"
#include "Console.h"

namespace Plop
{
	const uint32_t		COLOR_COMMAND = 0x008400FF;
	char				Console::s_pInputBuffer[256];
	Console::CommandMap	Console::s_mapCommands;
	LogEntry			Console::s_ConsoleBuffer[MAX_ENTRY];
	uint32_t			Console::s_uEntryIndex = 0;
	ImGuiTextFilter		Console::s_Filter;


	void Console::Init()
	{
		memset( s_pInputBuffer, 0, sizeof( s_pInputBuffer ) );

		RegisterCommand( "Test", []( const String& _args ) { Console::AddOutput( "Command Test with args" ); } );
	}

	void Console::AddOutput( const String& _str, uint32_t _uColorRGBA /*= 0xFFFFFFFF*/, LogEntry::Type _eType /*= LogEntry::Type::Info*/ )
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

		s_ConsoleBuffer[s_uEntryIndex] = { _eType, pBuffer, _uColorRGBA | 0x000000FF };
		s_uEntryIndex = (++s_uEntryIndex) % MAX_ENTRY;
	}

	void Console::RegisterCommand( const char* _pCommand, std::function<void( const String& )> _callback )
	{
		String sCommand = _pCommand;
		std::transform( sCommand.begin(), sCommand.end(), sCommand.begin(), ::toupper );

		ASSERT( s_mapCommands.find( sCommand ) == s_mapCommands.end(), "The command is already registered, overwriting the callback..." );
		s_mapCommands[sCommand] = _callback;
	}

	bool Console::ExecCommand( const char* _pText )
	{
		String sText = _pText;
		size_t argsPos = sText.find_first_of( ' ' );
		String sArgs = argsPos != String::npos ? sText.c_str() + argsPos + 1 : "";

		String sCommand = argsPos != String::npos ? sText.substr(0, argsPos ) : sText;
		std::transform( sCommand.begin(), sCommand.end(), sCommand.begin(), ::toupper );

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
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration;// | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
		if (ImGui::Begin( "Console", nullptr, windowFlags ))
		{
			s_Filter.Draw( "Filter (\"incl,-excl\") (\"error\")", 180 );


			const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
			ImGui::BeginChild( "ScrollingRegion", ImVec2( 0, -footer_height_to_reserve ), false, ImGuiWindowFlags_HorizontalScrollbar );
			ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 4, 1 ) );

			for (int i = 0; i < (int)s_uEntryIndex; i++)
			{
				const LogEntry& entry = s_ConsoleBuffer[i];
				const char* pText = entry.sLog.c_str();

				if (!s_Filter.PassFilter( pText ))
					continue;

				ImU32 uColor = IM_COL32( (entry.uColor & 0xFF000000) >> 24, (entry.uColor & 0x00FF0000) >> 16, (entry.uColor & 0x0000FF00) >> 8, (entry.uColor & 0x000000FF) );
				ImGui::PushStyleColor( ImGuiCol_Text, uColor );
				ImGui::Text( pText );
				//ImGui::TextUnformatted( pText );
				ImGui::PopStyleColor();
			}

			if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY( 1.0f );

			ImGui::PopStyleVar();
			ImGui::EndChild();
			ImGui::Separator();

			bool bReclaimFocus = false;
			ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
			if (ImGui::InputText( "Input", s_pInputBuffer, IM_ARRAYSIZE( s_pInputBuffer ), textFlags, &Console::InputCallback, nullptr ))
			{
				if (s_pInputBuffer[0])
				{
					if (ExecCommand( s_pInputBuffer ) == false)
					{
						String str = fmt::format( "Unknown command: {}", s_pInputBuffer );
						AddOutput( str, COLOR_COMMAND, LogEntry::Type::Command );
					}
				}
				strcpy( s_pInputBuffer, "" );
				bReclaimFocus = true;
			}

			ImGui::SetItemDefaultFocus();
			if(bReclaimFocus)
				ImGui::SetKeyboardFocusHere( -1 );
		}
		ImGui::End();
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
