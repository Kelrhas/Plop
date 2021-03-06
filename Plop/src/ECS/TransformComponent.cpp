#include "Plop_pch.h"
#include "TransformComponent.h"


#include <glm/gtc/type_ptr.hpp>
#include <imgui_custom.h>
#include <ImGuizmo.h>

#include "ECS/LevelBase.h"
#include "ECS/ECSHelper.h"
#include "ECS/BaseComponents.h"
#include "Utils/JsonTypes.h"

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

		auto owner = GetComponentOwner( xLevel, *this );
		Entity parent( owner.GetComponent<GraphNodeComponent>().parent, xLevel );
		if (parent)
		{
			// TODO: cache the parent worldmatrix or our own world and local matrix
			return parent.GetComponent<TransformComponent>().GetWorldMatrix() * mLocalMatrix;
		}

		return mLocalMatrix;
	}

	void TransformComponent::SetWorldPosition( const glm::vec3& _vPos )
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

		glm::vec3 vLocalPos = mParentWorldMatrix * glm::vec4( _vPos, 0.f );

		SetLocalPosition( vLocalPos );
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
			return (parent.GetComponent<TransformComponent>().GetWorldMatrix() * glm::vec4( vLocalPos, 1.f )).xyz;
		}

		return vLocalPos;
	}

	void TransformComponent::SetWorldRotation( const glm::quat& _qRot )
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

		glm::quat qLocalRot = mParentWorldMatrix * glm::mat4( _qRot );

		SetLocalRotation( qLocalRot );
	}

	glm::quat TransformComponent::GetWorldRotation() const
	{
		return glm::quat( GetWorldMatrix() );
	}

	void TransformComponent::TranslateWorld( const glm::vec3& _vTrans )
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

}

namespace MM
{
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

}