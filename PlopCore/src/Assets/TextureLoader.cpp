#include "Config.h"

#include "TextureLoader.h"

#include "Debug/Debug.h"
#include "Platform/OpenGL/OpenGL_Texture.h"
#include "Renderer/Renderer.h"
#include "Utils/OSDialogs.h"

#include <entt/entt.hpp>
#include <imgui.h>

namespace Plop
{
	struct TextureLoader final
	{
		using result_type = TexturePtr;

		TexturePtr operator()(const StringPath &_path) const
		{
			switch (Renderer::GetAPI())
			{
				case RenderAPI::API::OPENGL: return std::make_shared<OpenGL_Texture2D>(_path);
			}
			ASSERTM( false, "Render API not supported" );
			return nullptr;
		}
	};

	entt::resource_cache<Texture, TextureLoader> s_CacheTexture{};

	TextureHandle AssetLoader::GetTexture(const StringPath &_sFilepath)
	{
		String sName = _sFilepath.stem().string();
		return s_CacheTexture.load(entt::hashed_string(sName.c_str()), _sFilepath).first->second;
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

			for(auto [id,_hTexture] : s_CacheTexture)
			{
				if (_hTexture)
				{
					if (!texFilter.PassFilter( _hTexture->GetName() ))
						continue;

					if (bCheckerBg)
					{
						ImVec2 vCursorPos = ImGui::GetCursorPos();
						ImGui::SetCursorPosX( vCursorPos.x + ImGui::GetStyle().FramePadding.x );
						ImGui::SetCursorPosY( vCursorPos.y + ImGui::GetStyle().FramePadding.y );
						ImGui::Image( (ImTextureID)Renderer::GetDefaultTextureHandle(Renderer::DefaultTexture::CHECKER), ImVec2( fWidth, fWidth ), ImVec2( 0, 0 ), ImVec2( fWidth / 10.f, fWidth / 10.f ) );

						ImGui::SetCursorPos( vCursorPos );
					}


					// TODO: fix aspect ratio

					const ImVec2 vUvMin = Renderer::USE_INVERTED_Y_UV ? ImVec2(0, 1) : ImVec2(0, 0);
					const ImVec2 vUvMax = Renderer::USE_INVERTED_Y_UV ? ImVec2(1, 0) : ImVec2(1, 1);
					if (ImGui::ImageButton((ImTextureID)_hTexture->GetNativeHandle(), ImVec2(fWidth, fWidth), vUvMin, vUvMax))
					{
						hNewTexture = _hTexture;
						continue;
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
			}

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