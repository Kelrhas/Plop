#include "Plop_pch.h"
#include "BaseComponents.h"

#include <glm/gtc/type_ptr.hpp>

#include <Application.h>
#include <Assets/TextureLoader.h>
#include <Renderer/Texture.h>

namespace Plop
{
	SpriteRendererComponent::SpriteRendererComponent()
	{
		xSprite = std::make_shared<Plop::Sprite>();
	}
}

namespace MM
{
	template <>
	void ComponentEditorWidget<Plop::NameComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::NameComponent>( e );
		ImGui::Text( comp.sName.c_str() );
	}

	template <>
	void ComponentEditorWidget<Plop::TransformComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::TransformComponent>( e );
		ImGui::DragFloat4( "0", &comp.mTransform[0].x, 0.1f );
		ImGui::DragFloat4( "1", &comp.mTransform[1].x, 0.1f );
		ImGui::DragFloat4( "2", &comp.mTransform[2].x, 0.1f );
		ImGui::DragFloat4( "3", &comp.mTransform[3].x, 0.1f );
	}

	template <>
	void ComponentEditorWidget<Plop::SpriteRendererComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::SpriteRendererComponent>( e );
		if (comp.xSprite)
		{
			if (comp.xSprite->GetTextureHandle())
			{
				glm::vec2 vUVMin = comp.xSprite->GetUVMin();
				glm::vec2 vUVMax = comp.xSprite->GetUVMax();

				{
					float fWidth = ImGui::GetContentRegionAvail().x;
					float fUVRatio = (vUVMax.x - vUVMin.x) / (vUVMax.y - vUVMin.y);
					ImVec2 vSpriteImageSize( fWidth, fWidth );
					if (fUVRatio > 1)
						vSpriteImageSize.y /= fUVRatio;
					else
						vSpriteImageSize.x *= fUVRatio;

					if (ImGui::ImageButton( (ImTextureID)comp.xSprite->GetTexture().GetNativeHandle(), vSpriteImageSize, ImVec2( vUVMin.x, vUVMax.y ), ImVec2( vUVMax.x, vUVMin.y ), 1, ImVec4( 0, 0, 0, 0 ), comp.xSprite->GetTint() ))
						ImGui::OpenPopup( "###SpriteEditor" );
				}

				ImGui::ColorEdit4( "Tint", glm::value_ptr( comp.xSprite->GetTint() ), ImGuiColorEditFlags_AlphaBar );

				ImGui::SetNextWindowViewport( ImGui::GetWindowViewport()->ID );
				ImVec2 vPopupCenter = ImGui::GetWindowViewport()->GetCenter();
				glm::vec2 vPopupSize = ImGui::GetWindowViewport()->GetWorkSize();
				ImGui::SetNextWindowSize( vPopupSize * 0.5f, ImGuiCond_Appearing );
				//ImGui::SetNextWindowSizeConstraints( ImVec2(50, 50), vPopupSize * 0.5f );
				ImGui::SetNextWindowPos( vPopupCenter, ImGuiCond_Appearing, ImVec2( 0.5f, 0.5f ) );

				if (ImGui::BeginPopup( "###SpriteEditor" ))
				{
					glm::ivec2 vSpriteCount = comp.xSprite->GetSize();
					glm::vec2 vSpriteSize = ((vUVMax - vUVMin) / (glm::vec2)vSpriteCount);
					glm::ivec2 vSpritesheetCount = glm::ivec2( roundf( 1.f / vSpriteSize.x ), roundf( 1.f / vSpriteSize.y ) );
					glm::ivec2 vSpriteIndices = glm::vec2( roundf( vUVMin.x / vSpriteSize.x ), roundf( vUVMin.y / vSpriteSize.y ) );


					/*if (ImGui::DragFloat2( "UV min", glm::value_ptr( vUVMin ), 0.1f, 0.f, 1.f ))
						comp.xSprite->SetUV( vUVMin, vUVMax );
					if (ImGui::DragFloat2( "UV max", glm::value_ptr( vUVMax ), 0.1f, 0.f, 1.f ))
						comp.xSprite->SetUV( vUVMin, vUVMax );*/

					if (ImGui::DragInt2( "Sprite sheet count", glm::value_ptr( vSpritesheetCount ), 0.1f, 1, 512 ))
						comp.xSprite->SetSpriteIndex( vSpriteIndices, vSpritesheetCount, vSpriteCount );

					static const glm::ivec2 vZERO( 0 );
					if (ImGui::SliderInt2( "Sprite indices", glm::value_ptr( vSpriteIndices ), 0, glm::max( vSpritesheetCount.x, vSpritesheetCount.y ) - 1 ))
					{
						vSpriteIndices = glm::clamp( vSpriteIndices, glm::ivec2( 0 ), vSpritesheetCount - 1 );
						comp.xSprite->SetSpriteIndex( vSpriteIndices, vSpritesheetCount, vSpriteCount );
					}

					if (ImGui::SliderInt2( "Sprite count", glm::value_ptr( vSpriteCount ), 1, glm::max( vSpritesheetCount.x, vSpritesheetCount.y ) ))
					{
						vSpriteCount = glm::clamp( vSpriteCount, glm::ivec2( 1 ), vSpritesheetCount );
						comp.xSprite->SetSpriteIndex( vSpriteIndices, vSpritesheetCount, vSpriteCount );
					}


					float fPopupWidth = ImGui::GetContentRegionAvail().x;
					float fImageRatio = (float)comp.xSprite->GetTexture().GetWidth() / (float)comp.xSprite->GetTexture().GetHeight();


					ImVec2 vImageSize( fPopupWidth, fPopupWidth );
					if (fImageRatio > 1)
						vImageSize.y /= fImageRatio;
					else
						vImageSize.x *= fImageRatio;

					if (ImGui::ImageButton( (ImTextureID)comp.xSprite->GetTexture().GetNativeHandle(), vImageSize, ImVec2( 0, 1 ), ImVec2( 1, 0 ), 0 ))
						ImGui::OpenPopup( "###PickTextureFromCache" );

					Plop::TextureHandle hNewTex = Plop::AssetLoader::PickTextureFromCache();
					if (hNewTex)
					{
						comp.xSprite->SetTextureHandle( hNewTex );
					}

					ImGui::EndPopup();
				}
			}
			else
			{
				if (ImGui::Button( "Load texture" ))
					ImGui::OpenPopup( "###PickTextureFromCache" );

				Plop::TextureHandle hNewTex = Plop::AssetLoader::PickTextureFromCache();
				if (hNewTex)
				{
					comp.xSprite->SetTextureHandle( hNewTex );
				}
			}
		}
		else
		{
			ImGui::Text( "Invalid Sprite" );
		}

	}

	template <>
	void ComponentEditorWidget<Plop::CameraComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::CameraComponent>( e );
		if (comp.xCamera)
		{
			bool bOrtho = comp.xCamera->IsOrthographic();
			if (ImGui::Checkbox( "Orthographic", &bOrtho ))
			{
				if (bOrtho)
					comp.xCamera->SetOrthographic();
				else
					comp.xCamera->SetPerspective();
			}

			if (bOrtho)
			{
				float fOrthoSize = comp.xCamera->GetOrthographicSize();
				if (ImGui::DragFloat( "Size", &fOrthoSize, 0.1f, 0.f, FLT_MAX ))
					comp.xCamera->SetOrthographicSize( fOrthoSize );
			}
			else
			{
				float fFOV = glm::degrees( comp.xCamera->GetPerspectiveFOV() );
				if (ImGui::DragFloat( "FOV(°)", &fFOV, 0.1f, 10, 170 ))
					comp.xCamera->SetPerspectiveFOV( glm::radians( fFOV ) );
			}

			float fNear = comp.xCamera->GetNear();
			float fFar = comp.xCamera->GetFar();
			if (ImGui::DragFloat( "Near", &fNear, 0.1f, 0.f, fFar ))
				comp.xCamera->SetNear( fNear );

			if (ImGui::DragFloat( "Far", &fFar, 0.1f, fNear, FLT_MAX ))
				comp.xCamera->SetFar( fFar );
		}
		else
		{
			if (ImGui::Button( "Create orthographic" ))
			{
				comp.xCamera = std::make_shared<Plop::Camera>();
				comp.xCamera->SetOrthographic();
				comp.xCamera->Init();
			}

			if (ImGui::Button( "Create perspective" ))
			{
				comp.xCamera = std::make_shared<Plop::Camera>();
				comp.xCamera->SetPerspective();
				comp.xCamera->Init();
			}
		}
	}
}
