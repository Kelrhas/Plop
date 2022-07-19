#include "Plop_pch.h"
#include "Component_SpriteRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Assets/SpritesheetLoader.h"
#include "Utils/JsonTypes.h"

namespace Plop
{
	Component_SpriteRenderer::Component_SpriteRenderer()
	{
		xSprite = std::make_shared<Plop::Sprite>();
	}

	Component_SpriteRenderer& Component_SpriteRenderer::operator=( const Component_SpriteRenderer& _other )
	{
		xSprite = _other.xSprite;
		return *this;
	}

	void Component_SpriteRenderer::EditorUI()
	{
		if (xSprite)
		{
			auto hSpritesheet = xSprite->GetSpritesheetHandle();
			if (hSpritesheet)
			{
				glm::vec2 vCurrentUVMin = xSprite->GetUVMin();
				glm::vec2 vCurrentUVMax = xSprite->GetUVMax();

				if (hSpritesheet->GetNativeHandle())
				{
					float fWidth = ImGui::GetContentRegionAvail().x;
					float fUVRatio = (vCurrentUVMax.x - vCurrentUVMin.x) / (vCurrentUVMax.y - vCurrentUVMin.y);
					ImVec2 vSpriteImageSize( fWidth, fWidth );
					//if (fUVRatio > 1)
					//	vSpriteImageSize.y /= fUVRatio;
					//else
					//	vSpriteImageSize.x *= fUVRatio;

					if (ImGui::ImageButton( (ImTextureID)hSpritesheet->GetNativeHandle(), vSpriteImageSize, ImVec2( vCurrentUVMin.x, vCurrentUVMax.y ), ImVec2( vCurrentUVMax.x, vCurrentUVMin.y ), 1, ImVec4( 0, 0, 0, 0 ), xSprite->GetTint() ))
						ImGui::OpenPopup( "###SpriteEditor" );
				}

				ImGui::ColorEdit4( "Tint", glm::value_ptr( xSprite->GetTint() ), ImGuiColorEditFlags_AlphaBar );

				//ImGui::SetNextWindowViewport( ImGui::GetWindowViewport()->ID );
				ImVec2 vPopupCenter = ImGui::GetWindowViewport()->GetCenter();
				glm::vec2 vPopupSize = (glm::vec2)ImGui::GetWindowViewport()->GetWorkSize() * 0.75f;
				ImGui::SetNextWindowSize( vPopupSize, ImGuiCond_Always );
				//ImGui::SetNextWindowSizeConstraints( ImVec2(500, 200), vPopupSize * 0.8f );
				ImGui::SetNextWindowPos( vPopupCenter, ImGuiCond_Appearing, ImVec2( 0.5f, 0.5f ) );


				if (ImGui::BeginPopup( "###SpriteEditor" ))
				{
					// CONSTANTS
					static const ImColor BACKGROUND_COLOR( 0.5f, 0.5f, 0.5f, 0.35f );
					static const ImColor GRID_COLOR( 1.f, 1.f, 1.f, 0.35f );
					static const ImColor CURRENT_COLOR( 1.f, 1.f, 1.f, 0.35f );
					static const ImColor HIGHLIGHT_COLOR( 0.2f, 0.2f, 1.f, 0.5f );
					static const float ALIAS_LIST_WIDTH = 250.f;


					const glm::vec2 vSpriteSize = vCurrentUVMax - vCurrentUVMin;
					const glm::ivec2 vSpritesheetCount = glm::ivec2( roundf( 1.f / vSpriteSize.x ), roundf( 1.f / vSpriteSize.y ) );
					const glm::ivec2 vSpriteIndices = glm::vec2( roundf( vCurrentUVMin.x / vSpriteSize.x ), roundf( vCurrentUVMin.y / vSpriteSize.y ) );

					const float fRegionHeight = ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing();
					const float fImageRegionWidth = ImGui::GetWindowContentRegionWidth() - ALIAS_LIST_WIDTH;

					glm::vec2 vImageScreenPos = VEC2_0;
					glm::vec2 vImageDisplaySize = VEC2_0;

					bool bSpriteChanged = false;

					//ImGui::PushStyleColor( ImGuiCol_ChildBg, BACKGROUND_COLOR.Value );
					if (ImGui::BeginChild( "Spritesheet", ImVec2( fImageRegionWidth, fRegionHeight ), false, ImGuiWindowFlags_NoScrollbar ))
					{
						const float fImageRatio = (float)xSprite->GetTextureHandle()->GetWidth() / (float)xSprite->GetTextureHandle()->GetHeight();

						vImageDisplaySize = glm::vec2( ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().x / fImageRatio );

						if (vImageDisplaySize.y > fRegionHeight)
						{
							vImageDisplaySize.x = fRegionHeight * fImageRatio;
							vImageDisplaySize.y = fRegionHeight;

							const float fLeftMargin = (fImageRegionWidth - vImageDisplaySize.x) / 2.f;
							ImGui::SetCursorPosX( ImGui::GetCursorPosX() + fLeftMargin );
						}
						else
						{
							const float fTopMargin = (fRegionHeight - vImageDisplaySize.y) / 2.f;
							ImGui::SetCursorPosY( ImGui::GetCursorPosY() + fTopMargin );
						}

						glm::vec2 vImageWindowPos = ImGui::GetCursorPos();
						vImageScreenPos = vImageWindowPos + (glm::vec2)ImGui::GetWindowPos();


						ImDrawList* draw_list = ImGui::GetWindowDrawList();
						draw_list->AddRectFilled( vImageScreenPos, vImageScreenPos + vImageDisplaySize, BACKGROUND_COLOR );

						ImGui::Image( (ImTextureID)xSprite->GetTextureHandle()->GetNativeHandle(), vImageDisplaySize, ImVec2( 0, 1 ), ImVec2( 1, 0 ) );

						if (ImGui::IsItemHovered())
						{

							glm::vec2 vMousePos = (glm::vec2)ImGui::GetIO().MousePos - vImageScreenPos;

							vMousePos /= vImageDisplaySize; // gives UV

							glm::ivec2 vIndex = vMousePos * glm::vec2( hSpritesheet->GetNbColumn(), hSpritesheet->GetNbRow() );
							vIndex.y = hSpritesheet->GetNbRow() - vIndex.y - 1;

							glm::vec2 vUVMin;
							glm::vec2 vUVMax;
							hSpritesheet->GetSpriteUV( vIndex, vUVMin, vUVMax );
							vUVMin.y = 1.f - vUVMin.y;
							vUVMax.y = 1.f - vUVMax.y;
							draw_list->AddRectFilled( vImageScreenPos + vUVMin * vImageDisplaySize, vImageScreenPos + vUVMax * vImageDisplaySize, HIGHLIGHT_COLOR );

							if (ImGui::IsItemClicked())
							{
								xSprite->SetSpritesheet( hSpritesheet, vIndex );
								bSpriteChanged = true;
							}
						}


						// draw the grid
						for (uint8_t uRow = 0; uRow < hSpritesheet->GetNbRow() + 1; ++uRow)
						{
							float fRowPos = vImageScreenPos.y + glm::min( uRow * vImageDisplaySize.y / hSpritesheet->GetNbRow(), fRegionHeight - 1.f );
							draw_list->AddLine( ImVec2( vImageScreenPos.x, fRowPos ), ImVec2( vImageScreenPos.x + vImageDisplaySize.x, fRowPos ), GRID_COLOR );
						}
						for (uint8_t uCol = 0; uCol < hSpritesheet->GetNbColumn() + 1; ++uCol)
						{
							float fColPos = vImageScreenPos.x + glm::min( uCol * vImageDisplaySize.x / hSpritesheet->GetNbColumn(), fImageRegionWidth - 1.f );
							draw_list->AddLine( ImVec2( fColPos, vImageScreenPos.y ), ImVec2( fColPos, vImageScreenPos.y + vImageDisplaySize.y ), GRID_COLOR );
						}

						// draw the current selection
						{
							glm::vec2 vUVMin = { vCurrentUVMin.x, 1 - vCurrentUVMin.y };
							glm::vec2 vUVMax = { vCurrentUVMax.x, 1 - vCurrentUVMax.y };
							draw_list->AddRectFilled( vImageScreenPos + vUVMin * vImageDisplaySize, vImageScreenPos + vUVMax * vImageDisplaySize, CURRENT_COLOR );
						}
					}
					ImGui::EndChild();
					//ImGui::PopStyleColor();
					ImGui::SameLine();



					//ImGui::PushStyleColor( ImGuiCol_ChildBg, HIGHLIGHT_COLOR.Value );
					if (ImGui::BeginChild( "Aliases", ImVec2( 0, fRegionHeight ) ))
					{
						ImGui::Text( "Aliases" );
						ImGui::Separator();

						static ImGuiTextFilter aliasFilter;
						ImGui::Text( "Search:" );
						ImGui::SameLine();
						aliasFilter.Draw( "###AliasFilter" );
						ImGui::Indent( 10.f );
						for (const auto& aliasData : hSpritesheet->GetAliasData())
						{
							if (!aliasFilter.PassFilter( aliasData.first.c_str() ))
								continue;

							glm::uvec2 vSpriteIndices( aliasData.second.uColumn, aliasData.second.uRow );
							if (ImGui::Selectable( aliasData.first.c_str() ))
							{
								xSprite->SetSpritesheet( hSpritesheet, vSpriteIndices );
								bSpriteChanged = true;
							}
							if (ImGui::IsItemHovered())
							{
								ImDrawList* draw_list = ImGui::GetWindowDrawList();
								glm::vec2 vUVMin;
								glm::vec2 vUVMax;
								hSpritesheet->GetSpriteUV( vSpriteIndices, vUVMin, vUVMax );
								vUVMin.y = 1.f - vUVMin.y;
								vUVMax.y = 1.f - vUVMax.y;
								draw_list->PushClipRect( vImageScreenPos, vImageScreenPos + vImageDisplaySize );
								draw_list->AddRectFilled( vImageScreenPos + vUVMin * vImageDisplaySize, vImageScreenPos + vUVMax * vImageDisplaySize, HIGHLIGHT_COLOR );
								draw_list->PopClipRect();

							}
						}
						ImGui::Unindent( 10.f );
					}
					ImGui::EndChild();
					//ImGui::PopStyleColor();

					// keep open if the control key is down
					if (bSpriteChanged && !ImGui::GetIO().KeyCtrl)
						ImGui::CloseCurrentPopup();


					bool bOpenSpritesheetPopup = false;
					if (ImGui::Button( "Load another spritesheet" ))
						bOpenSpritesheetPopup = true;

					ImGui::EndPopup();

					if (bOpenSpritesheetPopup)
					{
						ImGui::CloseCurrentPopup();
						ImGui::OpenPopup( "###PickSpritesheetFromCache" );
					}
				}
			}
			else
			{
				if (ImGui::Button( "Load spritesheet" ))
					ImGui::OpenPopup( "###PickSpritesheetFromCache" );
			}

			Plop::SpritesheetHandle hNewSpritesheet = Plop::AssetLoader::PickSpritesheetFromCache();
			if (hNewSpritesheet)
			{
				xSprite->SetSpritesheet( hNewSpritesheet, VEC2_0 );
				ImGui::OpenPopup( "###SpriteEditor" );
			}
		}
		else
		{
			ImGui::Text( "Invalid Sprite" );
		}

	}

	json Component_SpriteRenderer::ToJson() const
	{
		json j;
		if (xSprite)
		{
			if (xSprite->GetSpritesheetHandle())
			{
				j["Spritesheet"] = xSprite->GetSpritesheetHandle()->GetFilePath().lexically_relative(std::filesystem::current_path()).string().c_str();
				if (auto sAlias = xSprite->GetAlias(); !sAlias.empty())
					j["Alias"] = sAlias;
				else
					j["Coord"] = xSprite->GetCoord();
				j["Tint"] = xSprite->GetTint();
			}
		}
		return j;
	}

	void Component_SpriteRenderer::FromJson( const json& _j )
	{
		ASSERTM( (bool)xSprite, "SpritePtr should be created in component CTOR" );

		if (xSprite)
		{
			if (_j.contains( "Spritesheet" ))
			{
				String sPath = _j["Spritesheet"];
				auto hTex = Plop::AssetLoader::GetSpritesheet( sPath );
				if (_j.contains( "Alias" ))
				{
					String sAlias = _j["Alias"];
					xSprite->SetSpritesheet( hTex, sAlias );
				}
				else
				{
					glm::ivec2 vCoord = _j["Coord"];
					xSprite->SetSpritesheet( hTex, vCoord );
				}

				if (_j.contains( "Tint" ))
					xSprite->SetTint( _j["Tint"] );
			}
		}
	}
}

#ifndef USE_COMPONENT_MGR
namespace MM
{
	template <>
	void ComponentEditorWidget<Plop::Component_SpriteRenderer>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::Component_SpriteRenderer>( e );
		comp.EditorUI();
	}

	template <>
	json ComponentToJson<Plop::Component_SpriteRenderer>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::Component_SpriteRenderer>( e );
		return comp.ToJson();
	}

	template<>
	void ComponentFromJson<Plop::Component_SpriteRenderer>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<Plop::Component_SpriteRenderer>( e );
		comp.FromJson( _j );
	}

}
#endif