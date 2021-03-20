#include "Plop_pch.h"
#include "BaseComponents.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <imgui_custom.h>
#include <ImGuizmo.h>

#include "Application.h"
#include "Assets/TextureLoader.h"
#include "Renderer/Texture.h"
#include "Utils/JsonTypes.h"
#include "Camera/Camera.h"
#include "ECS/ECSHelper.h"


namespace Plop
{
	float TransformComponent::Distance2D( const TransformComponent& _other ) const
	{
		glm::vec2 vThisPos = GetWorldPosition();
		glm::vec2 vOtherPos = _other.GetWorldPosition();

		return glm::distance( vThisPos, vOtherPos );
	}

	float TransformComponent::Distance2DSquare( const TransformComponent& _other ) const
	{
		glm::vec2 vThisPos = GetWorldPosition();
		glm::vec2 vOtherPos = _other.GetWorldPosition();

		return glm::distance2( vThisPos, vOtherPos );
	}

	// LOCAL

	void TransformComponent::SetLocalMatrix( const glm::mat4& _m )
	{
		glm::vec3 vRotation;
		ImGuizmo::DecomposeMatrixToComponents( glm::value_ptr( _m ), glm::value_ptr( vPosition ), glm::value_ptr( vRotation ), glm::value_ptr( vScale ) );
		qRotation = glm::quat( glm::radians( vRotation ) );
	}

	glm::mat4 TransformComponent::GetLocalMatrix() const
	{
		glm::mat4 mTransform = glm::translate( glm::identity<glm::mat4>(), vPosition );
		mTransform = mTransform * glm::toMat4( qRotation );
		mTransform = glm::scale( mTransform, vScale );
		return mTransform;
	}


	void TransformComponent::TranslateLocal( const glm::vec3& _vTrans )
	{
		vPosition += _vTrans;
	}


	// WORLD

	void TransformComponent::SetWorldMatrix( const glm::mat4& _m )
	{
		glm::mat4 mParentWorldMatrix = glm::identity<glm::mat4>();

		auto xLevel = LevelBase::GetCurrentLevel().lock();
		auto owner = GetComponentOwner( xLevel, *this );
		Entity parent( owner.GetComponent<GraphNodeComponent>().parent, xLevel );
		if (parent)
		{
			// TODO: cache the parent worldmatrix or our own world and local matrix
			mParentWorldMatrix = parent.GetComponent<TransformComponent>().GetWorldMatrix();
		}

		mParentWorldMatrix = glm::inverse( mParentWorldMatrix );

		glm::mat4 mLocalMatrix = mParentWorldMatrix * _m;

		SetLocalMatrix( mLocalMatrix );
	}

	glm::mat4 TransformComponent::GetWorldMatrix() const
	{
		glm::mat4 mLocalMatrix = GetLocalMatrix();

		auto xLevel = LevelBase::GetCurrentLevel().lock();

		auto owner = GetComponentOwner( xLevel, *this);
		Entity parent( owner.GetComponent<GraphNodeComponent>().parent, xLevel );
		if (parent)
		{
			// TODO: cache the parent worldmatrix or our own world and local matrix
			return parent.GetComponent<TransformComponent>().GetWorldMatrix() * mLocalMatrix;
		}

		return mLocalMatrix;
	}

	glm::vec3 TransformComponent::GetWorldPosition() const
	{
		glm::vec3 vLocalPos = GetLocalPosition();

		auto xLevel = LevelBase::GetCurrentLevel().lock();

		auto owner = GetComponentOwner( xLevel, *this );
		Entity parent( owner.GetComponent<GraphNodeComponent>().parent, xLevel );
		if (parent)
		{
			// TODO: cache the parent worldmatrix or our own world and local matrix
			return (parent.GetComponent<TransformComponent>().GetWorldMatrix() * glm::vec4(vLocalPos, 1.f)).xyz;
		}

		return vLocalPos;
	}

	void TransformComponent::TranslateWorld(const glm::vec3& _vTrans)
	{
		glm::mat4 mParentWorldMatrix = glm::identity<glm::mat4>();

		auto xLevel = LevelBase::GetCurrentLevel().lock();
		auto owner = GetComponentOwner( xLevel, *this );
		Entity parent( owner.GetComponent<GraphNodeComponent>().parent, xLevel );
		if (parent)
		{
			// TODO: cache the parent worldmatrix or our own world and local matrix
			mParentWorldMatrix = parent.GetComponent<TransformComponent>().GetWorldMatrix();
		}

		mParentWorldMatrix = glm::inverse( mParentWorldMatrix );

		glm::vec3 vTransLocal = mParentWorldMatrix * glm::vec4( _vTrans, 1.f );

		TranslateLocal( vTransLocal );
	}

	SpriteRendererComponent::SpriteRendererComponent()
	{
		xSprite = std::make_shared<Plop::Sprite>();
	}

	SpriteRendererComponent& SpriteRendererComponent::operator=( const SpriteRendererComponent& _other )
	{
		xSprite = _other.xSprite;
		return *this;
	}

	CameraComponent& CameraComponent::operator=( const CameraComponent& _other )
	{
		if (_other.xCamera)
		{
			xCamera = std::make_shared<Camera>();
			xCamera->Init();
			if (_other.xCamera->IsOrthographic())
				xCamera->SetOrthographic();
			else
				xCamera->SetPerspective();
			xCamera->SetAspectRatio( _other.xCamera->GetAspectRatio() );
			xCamera->SetNear( _other.xCamera->GetNear() );
			xCamera->SetFar( _other.xCamera->GetFar() );
			xCamera->SetOrthographicSize( _other.xCamera->GetOrthographicSize() );
			xCamera->SetPerspectiveFOV( _other.xCamera->GetPerspectiveFOV() );
		}
		return *this;
	}
}

namespace MM
{
	template <>
	json ComponentToJson<Plop::NameComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::NameComponent>( e );
		return json( comp.sName );
	}


	template <>
	void ComponentEditorWidget<Plop::TransformComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		float fResetBtnWidth = ImGui::CalcTextSize( "X" ).x + ImGui::GetStyle().FramePadding.x * 2;

		// TODO: add a switch local/world
		ImGui::Text( "Local space" );
		ImGui::Separator();

		auto& comp = reg.get<Plop::TransformComponent>( e );
		glm::vec3 vPos = comp.GetLocalPosition();
		if (ImGui::DragFloat3( "Pos", glm::value_ptr( vPos ), 0.1f ))
			comp.SetLocalPosition( vPos );
		ImGui::SameLine( ImGui::GetWindowContentRegionMax().x - fResetBtnWidth );
		if (ImGui::SmallButton( "X##Pos" ))
			comp.SetLocalPosition( VEC3_0 );
		ImGui::Custom::Tooltip( "Reset" );

		// TODO: Use matrices in Runtime environment and 2 vec3 & 1 quat in editor environment

		static bool bIsEditing = false;
		static glm::vec3 vTemp = VEC3_0;
		glm::vec3 vRot = bIsEditing ? vTemp : glm::degrees( glm::eulerAngles( comp.GetLocalRotation() ) );
		bool bChanged = ImGui::DragFloat3( "Rot", glm::value_ptr( vRot ) );

		if (ImGui::IsItemActivated())
		{
			vTemp = vRot;
			bIsEditing = true;
		}
		else if (!ImGui::IsItemActive())
			bIsEditing = false;

		if (bIsEditing && bChanged)
		{
			vTemp = vRot;
			comp.SetLocalRotation( glm::quat( glm::radians( vRot ) ) );
		}

		ImGui::SameLine( ImGui::GetWindowContentRegionMax().x - fResetBtnWidth );
		if (ImGui::SmallButton( "X##Rot" ))
			comp.SetLocalRotation( glm::identity<glm::quat>() );
		ImGui::Custom::Tooltip( "Reset" );


		glm::vec3 vScale = comp.GetLocalScale();
		if (ImGui::DragFloat3( "Sca", glm::value_ptr( vScale ), 0.1f, 0.01f ))
			comp.SetLocalScale( vScale );
		ImGui::SameLine( ImGui::GetWindowContentRegionMax().x - fResetBtnWidth );
		if (ImGui::SmallButton( "X##Sca" ))
			comp.SetLocalScale( VEC3_1 );
		ImGui::Custom::Tooltip( "Reset" );
	}

	template <>
	json ComponentToJson<Plop::TransformComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::TransformComponent>( e );
		json j;
		j["Position"] = comp.GetLocalPosition();
		j["Rotation"] = comp.GetLocalRotation();
		j["Scale"] = comp.GetLocalScale();
		return j;
	}

	template<>
	void ComponentFromJson<Plop::TransformComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<Plop::TransformComponent>( e );
		if (_j.contains( "Position" ))
			comp.SetLocalPosition( _j["Position"] );
		if (_j.contains( "Rotation" ))
			if (_j["Rotation"].size() == 3)
				comp.SetLocalRotation( glm::quat( glm::vec3(_j["Rotation"]) ) );
			else
				comp.SetLocalRotation( _j["Rotation"] );
		if (_j.contains( "Scale" ))
			comp.SetLocalScale( _j["Scale"] );
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
	json ComponentToJson<Plop::SpriteRendererComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::SpriteRendererComponent>( e );
		json j;
		if (comp.xSprite)
		{
			if (comp.xSprite->GetTextureHandle())
			{
				j["texture"] = comp.xSprite->GetTextureHandle()->GetName();
			}
			j["uvmin"] = comp.xSprite->GetUVMin();
			j["uvmax"] = comp.xSprite->GetUVMax();
			j["tint"] = comp.xSprite->GetTint();
			j["size"] = comp.xSprite->GetSize();
		}
		return j;
	}

	template<>
	void ComponentFromJson<Plop::SpriteRendererComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<Plop::SpriteRendererComponent>( e );
		ASSERT( (bool)comp.xSprite, "SpritePtr should be created in component CTOR" );
		
		if (comp.xSprite)
		{
			if (_j.contains( "texture" ))
			{
				auto hTex = Plop::AssetLoader::GetTexture( _j["texture"] );
				comp.xSprite->SetTextureHandle( hTex );
			}
			comp.xSprite->SetUV( _j["uvmin"], _j["uvmax"] );
			comp.xSprite->SetTint( _j["tint"] );
			comp.xSprite->SetSize( _j["size"] );
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
				//if (ImGui::DragFloat( "FOV(°)", &fFOV, 0.1f, 10, 170 )) // no ° symbol in ImGui font
				if (ImGui::DragFloat( "FOV(deg)", &fFOV, 0.1f, 10, 170 ))
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

	template <>
	json ComponentToJson<Plop::CameraComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::CameraComponent>( e );
		json j;
		if (comp.xCamera)
		{
			j["ortho"] = comp.xCamera->IsOrthographic();
			j["near"] = comp.xCamera->GetNear();
			j["far"] = comp.xCamera->GetFar();
			j["orthoSize"] = comp.xCamera->GetOrthographicSize();
			j["perspFOV"] = comp.xCamera->GetPerspectiveFOV();
		}
		return j;
	}

	template<>
	void ComponentFromJson<Plop::CameraComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<Plop::CameraComponent>( e );
		if (!comp.xCamera)
		{
			comp.xCamera = std::make_shared<Plop::Camera>();
			comp.xCamera->Init();
		}

		comp.xCamera->SetOrthographicSize( _j["orthoSize"] );
		comp.xCamera->SetPerspectiveFOV( _j["perspFOV"] );
		if(_j["ortho"] == true)
			comp.xCamera->SetOrthographic();
		else
			comp.xCamera->SetPerspective();

		comp.xCamera->SetNear( _j["near"] );
		comp.xCamera->SetFar( _j["far"] );
	}

}
