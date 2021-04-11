#include "Plop_pch.h"
#include "SoundLoader.h"


#include <entt/entt.hpp>

#include "Application.h"
#include "Debug/Log.h"
#include "Utils/Utils.h"
#include "Utils/OSDialogs.h"

namespace Plop::AssetLoader
{
	/// <summary>
	/// Resource loader for the class Sound, takes a file path
	/// </summary>
	struct SoundLoader : entt::resource_loader<SoundLoader, Sound>
	{
		SoundPtr load( const StringPath& _path ) const
		{
			StringPath sAssetpath = Application::Get()->GetRootDirectory() / _path;

			SoundPtr xSnd;
			if (_path.extension() == ".wav")
				xSnd = LoadWAV( sAssetpath );
			else if (_path.extension() == ".mp3")
				xSnd = LoadMP3( sAssetpath );
			else
				Debug::TODO();

			if (xSnd)
				xSnd->m_sFilePath = _path;

			return xSnd;
		}


		SoundPtr LoadWAV( const StringPath& _path ) const
		{
			//https://fr.wikipedia.org/wiki/Waveform_Audio_File_Format
			// LITTLE ENDIAN

			std::ifstream stream( _path, std::ifstream::binary | std::ifstream::in );
			ASSERT( stream.good() );
			if (!stream.good())
				return nullptr;

			SoundPtr xSnd = std::make_shared<Sound>();

			uint8_t SizeTemp[4];

			uint8_t FileTypeBlocID[4];
			uint32_t FileSize;
			uint8_t FileFormatID[4];

			stream.read( (char*)FileTypeBlocID, 4 );
			VERIFY( Utils::Check4CharCode( FileTypeBlocID, "RIFF" ) );
			stream.read( (char*)SizeTemp, 4 );
			FileSize = SizeTemp[0] | SizeTemp[1] << 8 | SizeTemp[2] << 16 | SizeTemp[3] << 24;
			stream.read( (char*)FileFormatID, 4 );
			VERIFY( Utils::Check4CharCode( FileFormatID, "WAVE" ) );

			uint8_t FormatBlocID[4];
			uint32_t BlocSize;
			stream.read( (char*)FormatBlocID, 4 );
			VERIFY( Utils::Check4CharCode( FormatBlocID, "fmt " ) );
			stream.read( (char*)SizeTemp, 4 );
			BlocSize = SizeTemp[0] | SizeTemp[1] << 8 | SizeTemp[2] << 16 | SizeTemp[3] << 24;

			uint16_t AudioFormat;
			uint16_t NbChannel;
			uint32_t BytePerSec;
			uint16_t BytePerBloc;
			uint16_t BitsPerSample;
			stream.read( (char*)SizeTemp, 2 );
			AudioFormat = SizeTemp[0] | SizeTemp[1] << 8;
			stream.read( (char*)SizeTemp, 2 );
			NbChannel = SizeTemp[0] | SizeTemp[1] << 8;
			stream.read( (char*)SizeTemp, 4 );
			xSnd->m_uFrequency = SizeTemp[0] | SizeTemp[1] << 8 | SizeTemp[2] << 16 | SizeTemp[3] << 24;
			stream.read( (char*)SizeTemp, 4 );
			BytePerSec = SizeTemp[0] | SizeTemp[1] << 8 | SizeTemp[2] << 16 | SizeTemp[3] << 24;
			stream.read( (char*)SizeTemp, 2 );
			BytePerBloc = SizeTemp[0] | SizeTemp[1] << 8;
			stream.read( (char*)SizeTemp, 2 );
			BitsPerSample = SizeTemp[0] | SizeTemp[1] << 8;
			VERIFY( AudioFormat == 1 ); // PCM
			VERIFY( BitsPerSample == 16 );

			uint8_t DataBlocID[4];
			stream.read( (char*)DataBlocID, 4 );
			stream.read( (char*)SizeTemp, 4 );
			xSnd->m_uDataSize = SizeTemp[0] | SizeTemp[1] << 8 | SizeTemp[2] << 16 | SizeTemp[3] << 24;

			VERIFY( Utils::Check4CharCode( DataBlocID, "data" ) );

			xSnd->m_pSamples = new ALubyte[xSnd->m_uDataSize / sizeof( ALubyte )];

			stream.read( (char*)xSnd->m_pSamples, xSnd->m_uDataSize / sizeof( ALubyte ) );

			alGenBuffers( 1, &xSnd->m_uBufferID );
			Debug::Assert_AL();

			xSnd->m_iChannels = NbChannel;
			if (NbChannel == 1)
			{
				xSnd->m_eFormat = AL_FORMAT_MONO16;
			}
			else if (NbChannel == 2)
			{
				xSnd->m_eFormat = AL_FORMAT_STEREO16;
			}
			else
			{
				Debug::TODO();
			}

			alBufferData( xSnd->m_uBufferID, xSnd->m_eFormat, &xSnd->m_pSamples[0], xSnd->m_uDataSize, xSnd->m_uFrequency );
			Debug::Assert_AL();

			return xSnd;
		}

		SoundPtr LoadMP3( const StringPath& _path ) const
		{
			Debug::TODO();

			return nullptr;
		}
	};


	entt::resource_cache<Sound> s_CacheSound{};

	SoundHandle GetSound( const StringPath& _sFilepath )
	{
		return s_CacheSound.load<SoundLoader>( entt::hashed_string( _sFilepath.string().c_str() ), _sFilepath );
	}

	void ClearSoundCache()
	{
		s_CacheSound.clear();
	}

	SoundHandle PickSoundFromCache()
	{
		SoundHandle hNewSound;

		bool bOpened;
		if (ImGui::BeginPopupModal( "Pick a loaded sound###PickSoundFromCache", &bOpened ))
		{
			static ImGuiTextFilter texFilter;
			texFilter.Draw();

			s_CacheSound.each( [&]( SoundHandle _hSound ) {
				if (_hSound)
				{
					if (!texFilter.PassFilter( _hSound->GetFilePathStr().c_str() ))
						return;

					if (ImGui::Selectable( _hSound->GetFilePathStr().c_str() ))
					{
						hNewSound = _hSound;
						return;
					}

					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::Text( _hSound->GetFilePathStr().c_str() );
						ImGui::EndTooltip();
					}
				}
			} );

			if (ImGui::Button( "Load from file" ))
			{
				StringPath filePath;
				if (Dialog::OpenFile( filePath, Dialog::SOUND_FILTER ))
				{
					hNewSound = GetSound( filePath );
				}
			}

			if (hNewSound)
			{
				ImGuiIO& io = ImGui::GetIO();
				if (!io.KeyCtrl)
					ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}


		return hNewSound;
	}
}