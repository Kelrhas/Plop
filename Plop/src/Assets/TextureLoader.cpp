#include "Plop_pch.h"
#include "TextureLoader.h"

#include <entt/entt.hpp>

#include "Utils/OSDialogs.h"
#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGL_Texture.h"

namespace Plop
{
	struct TextureLoader : entt::resource_loader<TextureLoader, Plop::Texture>
	{
		std::shared_ptr<Plop::Texture> load( const String& _name ) const
		{
			switch (Renderer::GetAPI())
			{
				case RenderAPI::API::OPENGL:		return std::make_shared<OpenGL_Texture2D>( _name );
			}
			ASSERT( false, "Render API not supported" );
			return nullptr;
		}
	};

	entt::resource_cache<Texture> s_CacheTexture{};

	TextureHandle AssetLoader::GetTexture( const String& _sFilename )
	{
		return s_CacheTexture.load<TextureLoader>( entt::hashed_string( _sFilename.c_str()), _sFilename );
	}

	TextureHandle AssetLoader::PickTextureFromCache()
	{
		TextureHandle hNewTexture;

		bool bOpened;
		if (ImGui::BeginPopupModal( "Pick a loaded texture###PickTextureFromCache", &bOpened ))
		{
			static ImGuiTextFilter texFilter;
			texFilter.Draw();
			static int iNbImagePerRow = 3;
			ImGui::DragInt( "# column", &iNbImagePerRow, 0.1f, 1, 10 );
			static bool bCheckerBg = true;
			ImGui::Checkbox( "Checker background", &bCheckerBg );

			ImGui::BeginChild( "", ImVec2( 0, -ImGui::GetFrameHeightWithSpacing() ) );

			ImGuiStyle& style = ImGui::GetStyle();
			float fContentWidth = ImGui::GetContentRegionAvail().x - style.ItemSpacing.x;
			float fSpacing = style.ItemSpacing.x + 2 * style.FramePadding.x;
			float fWidth = (fContentWidth - (iNbImagePerRow - 1) * (fSpacing)) / iNbImagePerRow;
			int iCurrentItem = 0;
			s_CacheTexture.each( [&]( TextureHandle _hTexture )
				{
					if (_hTexture)
					{
						if (!texFilter.PassFilter( _hTexture->GetName() ))
							return;

						if (bCheckerBg)
						{
							ImVec2 vCursorPos = ImGui::GetCursorPos();
							ImGui::SetCursorPosX( vCursorPos.x + ImGui::GetStyle().FramePadding.x );
							ImGui::SetCursorPosY( vCursorPos.y + ImGui::GetStyle().FramePadding.y );
							ImGui::Image( (ImTextureID)Renderer2D::s_xCheckerTex->GetNativeHandle(), ImVec2( fWidth, fWidth ), ImVec2( 0, 0 ), ImVec2( fWidth / 10.f, fWidth / 10.f ) );

							ImGui::SetCursorPos( vCursorPos );
						}


						// TODO: fix aspect ratio

						if (ImGui::ImageButton( (ImTextureID)_hTexture->GetNativeHandle(), ImVec2( fWidth, fWidth ), ImVec2( 0, 1 ), ImVec2( 1, 0 ) ))
						{
							hNewTexture = _hTexture;
							return;
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text( _hTexture->GetName() );
							ImGui::EndTooltip();
						}
						++iCurrentItem;
						if ((iCurrentItem % iNbImagePerRow) != 0)
							ImGui::SameLine( iCurrentItem * ImGui::GetContentRegionAvail().x / iNbImagePerRow );

					}
				} );

			ImGui::EndChild();

			if (ImGui::Button( "Load from file" ))
			{
				StringPath filePath;
				if (Dialog::OpenFile( filePath, Dialog::IMAGE_FILTER ))
				{
					hNewTexture = GetTexture( filePath.string() );
				}
			}

			if (hNewTexture)
			{
				ImGuiIO& io = ImGui::GetIO();
				if(!io.KeyCtrl)
					ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}


		return hNewTexture;
	}
}